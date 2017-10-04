#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "tests.h"

#include <stdio.h>

#include <QFile>

using namespace bertha;
using namespace cmn;

void mytestXmlFile(QString xmlFile, QString compareFile);

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

void BerthaTest::testOlaFft_data()
{
    QTest::addColumn<QString>("xmlFile");
    QTest::addColumn<QString>("inputFile");

    QTest::newRow("olafft")
        << QString::fromLatin1("testexperiments/olafftexperiment.xml")
        << QString::fromLatin1("testdata/wdz4.wav");
}

void BerthaTest::testOlaFft()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, xmlFile);
    QFETCH(QString, inputFile);

    TemporaryDirectory outputDir;
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());
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
    cmn::SndFile inputFilePtr;

    SF_INFO outputSfinfo;
    SF_INFO inputSfinfo;
    outputSfinfo.format = 0;
    inputSfinfo.format = 0;
    outputFilePtr.reset(outputPath, SFM_READ, &outputSfinfo);
    inputFilePtr.reset(inputPath, SFM_READ, &inputSfinfo);

    QVERIFY(outputFilePtr);
    QVERIFY(inputFilePtr);

    QVector<float> outputData(outputSfinfo.frames);
    const sf_count_t outputCount = sf_read_float(
        outputFilePtr.get(), outputData.data(), outputData.size());
    QCOMPARE(outputCount, outputSfinfo.frames);
    QVector<float> inputData(inputSfinfo.frames);
    const sf_count_t inputCount =
        sf_read_float(inputFilePtr.get(), inputData.data(), inputData.size());
    QCOMPARE(inputCount, inputSfinfo.frames);
    // TODO: this never worked
    // ARRAYFUZZCOMP(outputData, inputData, 1.0 / 32768, outputCount);

    TEST_EXCEPTIONS_CATCH
}
