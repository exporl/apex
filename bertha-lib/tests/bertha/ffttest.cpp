#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "tests.h"

#include <QFile>

using namespace bertha;
using namespace cmn;

static void testXmlFile(const QString &xmlFile, const QString &wavInputFile,
                        const QString &compareFile);

namespace
{

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
}

void BerthaTest::testFft()
{
    TEST_EXCEPTIONS_TRY

    const QString xmlFile =
        QLatin1String("testexperiments/fftfileexperiment.xml");
    const QString inputFile = QLatin1String("testdata/sin1000Hz.wav");
    const QString compareFile = QLatin1String("testdata/sin1000Hz.wav");

    testXmlFile(xmlFile, inputFile, compareFile);

    TEST_EXCEPTIONS_CATCH
}

static void testXmlFile(const QString &xmlFile, const QString &inputFile,
                        const QString &compareFile)
{
    TEST_EXCEPTIONS_TRY

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

    ARRAYFUZZCOMP(outputData, compareData, 2.0 / 65536, outputCount);

    TEST_EXCEPTIONS_CATCH
}

#include "ffttest.moc"
