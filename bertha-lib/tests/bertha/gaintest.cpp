#include "bertha/blockprocessor.h"
#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "tests.h"

#include <fftw3.h>

#include <QFile>

using namespace bertha;
using namespace cmn;

class TerminateOnStop : public QObject
{
    Q_OBJECT
public:
    TerminateOnStop() : stopped(false)
    {
    }

public Q_SLOTS:
    void stateChanged(bertha::ExperimentProcessor::State state)
    {
        if (state == ExperimentProcessor::ExperimentPrepared)
            stopped = true;
    }

public:
    bool stopped;
};

void BerthaTest::testGain_data()
{
    QTest::addColumn<QString>("xmlFile");
    QTest::addColumn<QString>("inputFile");
    QTest::addColumn<QString>("compareFile");
    QTest::newRow("gain") << "testexperiments/gaintest.xml"
                          << "testdata/sin1000Hz.wav"
                          << "testdata/sin1000Hz_gain6dB.wav";
}

void BerthaTest::testGain()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, xmlFile);
    QFETCH(QString, inputFile);
    QFETCH(QString, compareFile);

    TemporaryDirectory outputDir;
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());
    const QString inputPath =
        Paths::searchFile(inputFile, Paths::dataDirectories());
    const QString comparePath =
        Paths::searchFile(compareFile, Paths::dataDirectories());
    const QString outputPath = outputDir.addFile(QLatin1String("output.wav"));

    QFile file(xmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = file.readAll();
    data.replace("INPUTFILEPLACEHOLDER", inputPath.toUtf8());
    data.replace("OUTPUTFILEPLACEHOLDER", outputPath.toUtf8());

    ExperimentProcessor processor(XmlLoader().loadContents(data));

    processor.prepare();

    TerminateOnStop stopper;
    QObject::connect(
        &processor, SIGNAL(stateChanged(bertha::ExperimentProcessor::State)),
        &stopper, SLOT(stateChanged(bertha::ExperimentProcessor::State)));

    processor.start();

    while (!stopper.stopped)
        qApp->processEvents();

    processor.release();

    cmn::SndFile outputFilePtr;
    cmn::SndFile compareFilePtr;

    SF_INFO outputSfinfo;
    SF_INFO compareSfinfo;
    outputSfinfo.format = 0;
    compareSfinfo.format = 0;
    outputFilePtr.reset(outputPath, SFM_READ, &outputSfinfo);
    compareFilePtr.reset(comparePath, SFM_READ, &compareSfinfo);

    QVERIFY(outputFilePtr);
    QVERIFY(compareFilePtr);

    QVector<float> outputData(44100 + 1);
    const unsigned outputCount = sf_read_float(
        outputFilePtr.get(), outputData.data(), outputData.size());
    QCOMPARE(outputCount, 44100u + 1);
    QVector<float> compareData(44100 + 1);
    const unsigned compareCount = sf_read_float(
        compareFilePtr.get(), compareData.data(), compareData.size());
    QCOMPARE(compareCount, 44100u + 1);

    ARRAYFUZZCOMP(outputData, compareData, 1.0 / 32768, outputCount);

    TEST_EXCEPTIONS_CATCH
}

void BerthaTest::testGainRamp()
{
    TEST_EXCEPTIONS_TRY

    const QString inputFile = QLatin1String("testdata/sin1000Hz.wav");
    const QString compareFile = QLatin1String("testdata/sin1000Hz_ramp.wav");

    /* The comparison file was created using the following matlab code:

    [d,fs]=wavread('/home/tom/data/bertha/data/wav/sin1000Hz.wav');

    blocksize=2048;

    fromsamples=5*blocksize;
    len=0.2;
    lensamples=round(len*fs);
    gainchange=-12;

    from=fromsamples;
    to=fromsamples+round(len*fs)-1;

    d=d*10^(6/20);      % amplify the whole by 6dB

    slope=gainchange/lensamples;
    slope=quantize(slope, 1/2^32);       % bertha uses float

    % linear ramp in dB
    d(from:to) = d(from:to).* 10.^((slope*[0:lensamples-1]')/20);

    d(to+1:end)=d(to+1:end)*10^(gainchange/20);

    */

    const QString outputFile = QLatin1String("gainRampOutput.wav");
    bool writeOutput = true;

    const QString inputPath =
        Paths::searchFile(inputFile, Paths::dataDirectories());
    const QString comparePath =
        Paths::searchFile(compareFile, Paths::dataDirectories());

    BlockData d;
    d.setInputPorts(1);
    d.setOutputPorts(1);
    d.setPlugin(QLatin1String("Gain"));
    d.setParameter(QLatin1String("rampLength"), 0.2);
    d.setParameter(QLatin1String("gain"), 6);

    unsigned blockSize = 2048;
    unsigned sampleRate = 44100;
    BlockProcessor proc(d, blockSize, sampleRate);
    proc.prepare(0);

    cmn::SndFile inputFilePtr;
    cmn::SndFile compareFilePtr;
    cmn::SndFile outputFilePtr;

    SF_INFO fSfinfo;
    fSfinfo.format = 0;
    inputFilePtr.reset(inputPath, SFM_READ, &fSfinfo);
    if (writeOutput)
        outputFilePtr.reset(outputFile, SFM_WRITE, &fSfinfo);
    compareFilePtr.reset(comparePath, SFM_READ, &fSfinfo);

    QVERIFY(inputFilePtr);
    QVERIFY(compareFilePtr);

    QSharedPointer<PortDataType> inputData(
        reinterpret_cast<PortDataType *>(
            fftwf_malloc(sizeof(PortDataType) * blockSize)),
        fftwf_free);
    BlockInputPort *inPort = proc.blockInterface()->inputPorts()[0];
    inPort->inputData = inputData;

    QVector<float> compareData(blockSize);

    BlockOutputPort *outPort = proc.blockInterface()->outputPorts()[0];
    unsigned gainParameterNr = proc.parameterIndex(QLatin1String("gain"));

    unsigned blockCount = 0;

    unsigned inputCount;
    while ((inputCount = sf_read_float(inputFilePtr.get(), inputData.data(),
                                       blockSize))) {
        if (inputCount < blockSize)
            memset(inputData.data() + inputCount, 0,
                   (blockSize - inputCount) * sizeof(float));

        if (blockCount == 5)
            proc.setParameterValue(gainParameterNr, -6);

        /*if (blockCount == 8)
            proc.setParameterValue(gainParameterNr, 10);*/

        proc.process();
        if (writeOutput) {
            const unsigned outputCount = sf_write_float(
                outputFilePtr.get(), outPort->outputData.data(), inputCount);
            QCOMPARE(outputCount, inputCount);
        }

        sf_read_float(compareFilePtr.get(), compareData.data(), blockSize);

        // 16 bits wav file, hence maximal error of 1/2^15
        ARRAYFUZZCOMP(outPort->outputData.data(), compareData.data(),
                      1.0 / 32768, inputCount);
        ++blockCount;
    }

    TEST_EXCEPTIONS_CATCH
}

#include "gaintest.moc"
