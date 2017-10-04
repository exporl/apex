#!/usr/bin/python2

import sys
import numpy as np
import soundfile as sf

def filewiseFuzzyCompare(referenceFiles, resultFiles):
    if len(referenceFiles) != len(resultFiles):
        return False
    for i in range(len(referenceFiles)):
        referenceData, sr = sf.read(referenceFiles[i])
        resultData, sr = sf.read(resultFiles[i])
        if not np.allclose(referenceData, resultData):
            return False
    return True

def continuousCompare(referenceFiles, resultFiles):
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


if __name__ == '__main__':
    if sys.argv[1] == 'help':
        print 'Usage: "linux-experimenttest-audio-compare-functions.py continuous/filewise reference result"'
        sys.exit()
    if len(sys.argv) < 4:
        print 'less than 4 arguments'
        sys.exit(1)

    if sys.argv[1] == 'continuous':
        size = len(sys.argv[2:])
        if continuousCompare(sys.argv[2].split('\n'), sys.argv[3].split('\n')):
            print True
        else:
            print False
    elif sys.argv[1] == 'filewise':
        if filewiseFuzzyCompare(sys.argv[2].split('\n'), sys.argv[3].split('\n')):
            print True
        else:
            print False
    else:
        print False
