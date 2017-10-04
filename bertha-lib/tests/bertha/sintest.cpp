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

void BerthaTest::testSin_data()
{
    QTest::addColumn<QString>("xmlFile");
    QTest::addColumn<QString>("compareFile");

    QTest::newRow("50") << "testexperiments/sin50Hztest.xml"
                        << "testdata/sin50Hz.wav";
    QTest::newRow("1000") << "testexperiments/sin1000Hztest.xml"
                          << "testdata/sin1000Hz.wav";
    QTest::newRow("combined")
        << "testexperiments/sin1000_sin50Hz-halfwavetest.xml"
        << "testdata/sin1000_sin50Hz-halfwave.wav";
}

void BerthaTest::testSin()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, xmlFile);
    QFETCH(QString, compareFile);

    TemporaryDirectory outputDir;
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());
    const QString comparePath =
        Paths::searchFile(compareFile, Paths::dataDirectories());
    const QString outputPath = outputDir.addFile(QLatin1String("output.wav"));

    QFile file(xmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = file.readAll();
    data.replace("OUTPUTFILEPLACEHOLDER", outputPath.toUtf8());

    ExperimentProcessor processor(XmlLoader().loadContents(data));

    processor.prepare();
    processor.start();
    for (unsigned i = 0; (i < 50) && (processor.state() !=
                                      ExperimentProcessor::ExperimentPrepared);
         ++i)
        QTest::qWait(100);
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
    for (unsigned i = 0; i < outputCount; ++i) {
        QVERIFY(qAbs(outputData[i] - compareData[i]) < 1.0 / 32768);
    }

    TEST_EXCEPTIONS_CATCH
}
