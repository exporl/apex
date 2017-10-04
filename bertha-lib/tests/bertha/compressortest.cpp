#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "tests.h"

#include <QDir>
#include <QFile>

using namespace bertha;
using namespace cmn;

void testXmlCompressorFile(QString xmlFile, QString inputFile,
                           QString compareFile);

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

void BerthaTest::testCompressor_data()
{
    QTest::addColumn<QString>("xmlFile");
    QTest::addColumn<QString>("inputFile");
    QTest::addColumn<QString>("compareFile");
    QTest::newRow("compressor") << "testexperiments/compressorexperiment.xml"
                                << "testdata/istsinputspeech.wav"
                                << "testdata/istscompressedspeech.wav";
}

void BerthaTest::testCompressor()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, xmlFile);
    QFETCH(QString, inputFile);
    QFETCH(QString, compareFile);

    TemporaryDirectory outputDir;
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());
    const QString comparePath =
        Paths::searchFile(compareFile, Paths::dataDirectories());
    const QString inputPath =
        Paths::searchFile(inputFile, Paths::dataDirectories());
    const QString outputPath = outputDir.addFile(QLatin1String("output.wav"));

    QFile file(xmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = file.readAll();
    data.replace("OUTPUTFILEPLACEHOLDER", outputPath.toUtf8());
    data.replace("INPUTFILEPLACEHOLDER", inputPath.toUtf8());

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

    QVector<float> outputData(compareSfinfo.frames);
    const sf_count_t outputCount = sf_read_float(
        outputFilePtr.get(), outputData.data(), outputData.size());
    QCOMPARE(outputCount, compareSfinfo.frames);
    QVector<float> compareData(compareSfinfo.frames);
    const sf_count_t compareCount = sf_read_float(
        compareFilePtr.get(), compareData.data(), compareData.size());
    QCOMPARE(compareCount, compareSfinfo.frames);

    ARRAYFUZZCOMP(outputData, compareData, 1.0 / 32768, outputCount);

    TEST_EXCEPTIONS_CATCH
}
