#!/usr/bin/python2

import sys, argparse
import numpy as np
import soundfile as sf

def filewiseFuzzyCompare(referenceFiles, resultFiles):
    if len(referenceFiles) != len(resultFiles) or len(referenceFiles) == 0:
        return False
    for i in range(len(referenceFiles)):
        referenceData, sr = sf.read(referenceFiles[i])
        resultData, sr = sf.read(resultFiles[i])
        if not np.allclose(referenceData, resultData):
            return False
    return True

def filewiseFuzzyCompareWithTruncate(referenceFiles, resultFiles):
    if len(referenceFiles) != len(resultFiles) or len(referenceFiles) == 0:
        return False
    for i in range(len(referenceFiles)):
        referenceData, sr = sf.read(referenceFiles[i])
        resultData, sr = sf.read(resultFiles[i])
        if not resizeAndCompareChunks(referenceData, resultData):
            return False
    return True

# This test expects a continuous generator which generates silence only.
# It makes sure that the start of each wav file (=trial) is as
# expected (=the specified stimulus of that trial).
def compareExpectingContinuousSilence(referenceFiles, resultFiles):
    if len(referenceFiles) != len(resultFiles) or len(referenceFiles) == 0:
        return False

    referencesData = []
    resultsData = []

    for i in range(len(referenceFiles)):
        referenceData, sr = sf.read(referenceFiles[i])
        resultData, sr = sf.read(resultFiles[i])

        # remove leading zeros
        referenceData = np.apply_along_axis(np.trim_zeros, 0, referenceData, 'f')
        resultData = np.apply_along_axis(np.trim_zeros, 0, resultData, 'f')

        # append zeros so equal length
        if len(referenceData) < len(resultData):
            shape = (len(resultData) - len(referenceData),) + referenceData.shape[1:]
            referenceData = np.concatenate((referenceData, np.zeros(shape)))
        elif len(resultData) < len(referenceData):
            shape = (len(referenceData) - len(resultData),) + resultData.shape[1:]
            resultData = np.concatenate((resultData, np.zeros(shape)))

        if len(referencesData) == 0:
            referencesData = referenceData
            resultsData = resultData
        else:
            referencesData = np.concatenate((referencesData, referenceData))
            resultsData = np.concatenate((resultsData, resultData))

    return np.allclose(referencesData, resultsData)

# This test expects silence as a stimulus and continuous generators.
# It effectively concatenates all audio data, thereby testing
# the audio data over wav files(=trials).
def continuousCompare(referenceFiles, resultFiles):
    if len(referenceFiles) != len(resultFiles) or len(referenceFiles) == 0:
        return False
    referenceFile = None
    resultFile = None
    chunkSize = 4096
    hasMoreData = True
    while hasMoreData:
        # read chunkSize frames or until either of the files are depleted
        referenceFile, referenceData = assureDataFromFiles(referenceFile, referenceFiles, chunkSize)
        resultFile, resultData = assureDataFromFiles(resultFile, resultFiles, chunkSize)
        if referenceData is None or resultData is None:
            break;
        if not resizeAndCompareChunks(referenceData, resultData):
            return False
        hasMoreData = not (len(referenceData) < chunkSize or len(resultData) < chunkSize)
    if referenceFile is not None:
        referenceFile.close()
    if resultFile is not None:
        resultFile.close()
    return True

def assureDataFromFiles(soundFile, files, count):
    data = None
    while data is None or len(data) < count:
        if soundFile is None or soundFile.tell() == len(soundFile):
            if len(files) > 0:
                if soundFile is not None:
                    soundFile.close()
                soundFile = sf.SoundFile(files.pop(0), 'r')
            else:
                break
        if data is not None:
            data = np.append(data, soundFile.read(count - len(data)))
        else:
            data = soundFile.read(count)
    return soundFile, data

def resizeAndCompareChunks(chunk1, chunk2):
    # truncate to smallest chunk
    if len(chunk1) < len(chunk2):
        chunk2.resize(chunk1.shape, refcheck=False)
    elif len(chunk2) < len(chunk1):
        chunk1.resize(chunk2.shape, refcheck=False)
    if not np.allclose(chunk2, chunk1):
        return False
    return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('type')
    parser.add_argument('reference')
    parser.add_argument('result')
    args = parser.parse_args()

    result = None;
    if args.type == 'continuous':
        result = continuousCompare(args.reference.split('\n'), args.result.split('\n'))
    elif args.type == 'compareexpectingcontinuoussilence':
        result = compareExpectingContinuousSilence(args.reference.split('\n'), args.result.split('\n'))
    elif args.type == 'filewise':
        result =  filewiseFuzzyCompare(args.reference.split('\n'), args.result.split('\n'))
    elif args.type == 'filewisetruncate':
        result =  filewiseFuzzyCompareWithTruncate(args.reference.split('\n'), args.result.split('\n'))

    if result:
        print True
    else:
        print False
