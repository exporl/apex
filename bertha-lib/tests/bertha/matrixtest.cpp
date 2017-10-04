#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/stringutils.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/testutils.h"

#include "tests.h"

#include <QFile>

using namespace bertha;
using namespace cmn;

void BerthaTest::testMixer_data()
{
    QTest::addColumn<QString>("switchMatrix");
    QTest::addColumn<QString>("output1Compare");
    QTest::addColumn<QString>("output2Compare");
    QTest::addColumn<bool>("logGains");
    QTest::newRow("normal") << "[[1 0][0 1]]"
                            << "testdata/sin1000Hz.wav"
                            << "testdata/sin50Hz.wav" << false;
    QTest::newRow("switched") << "[[0 1][1 0]]"
                              << "testdata/sin50Hz.wav"
                              << "testdata/sin1000Hz.wav" << false;
    QTest::newRow("logGains") << "[[10 -10][10 -10]]"
                              << "testdata/sin1000Hz_10dB_sin50-10dB.wav"
                              << "testdata/sin1000Hz_10dB_sin50-10dB.wav"
                              << true;
}

void BerthaTest::testMixer()
{
    TEST_EXCEPTIONS_TRY

    const QString xmlFile =
        QLatin1String("testexperiments/mixerexperiment.xml");

    QFETCH(QString, switchMatrix);
    QFETCH(QString, output1Compare);
    QFETCH(QString, output2Compare);
    QFETCH(bool, logGains);

    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());
    const QString comparePath1 =
        Paths::searchFile(output1Compare, Paths::dataDirectories());
    const QString comparePath2 =
        Paths::searchFile(output2Compare, Paths::dataDirectories());

    QTemporaryFile tempFile1;
    tempFile1.open();
    QTemporaryFile tempFile2;
    tempFile2.open();

    const QString outputPathCh1 = tempFile1.fileName();
    const QString outputPathCh2 = tempFile2.fileName();
    QString outputPath =
        outputPathCh1 + QString::fromLatin1(",") + outputPathCh2;

    QFile file(xmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QByteArray data = file.readAll();

    data.replace("OUTPUTFILEPLACEHOLDER", outputPath.toUtf8());
    ExperimentProcessor processor(XmlLoader().loadContents(data));

    processor.setParameterValue(QString::fromLatin1("switchblock"),
                                QString::fromLatin1("logarithmicGain"),
                                logGains);
    processor.setParameterValue(QString::fromLatin1("switchblock"),
                                QString::fromLatin1("matrix"), switchMatrix);

    processor.prepare();
    processor.start();
    QTest::qWait(250);
    processor.release();

    cmn::SndFile outputFilePtrCh1;
    cmn::SndFile outputFilePtrCh2;
    cmn::SndFile compareFilePtrCh1;
    cmn::SndFile compareFilePtrCh2;

    SF_INFO fileInfo;
    fileInfo.format = 0;
    outputFilePtrCh1.reset(outputPathCh1, SFM_READ, &fileInfo);
    if (!outputFilePtrCh1)
        QFAIL("Unable to open output channel 1 wave file.");

    fileInfo.format = 0;
    outputFilePtrCh2.reset(outputPathCh2, SFM_READ, &fileInfo);
    if (!outputFilePtrCh2)
        QFAIL("Unable to open output channel 2 wave file.");

    fileInfo.format = 0;
    compareFilePtrCh1.reset(comparePath1, SFM_READ, &fileInfo);
    if (!compareFilePtrCh1)
        QFAIL("Unable to open compare wave file channel 1.");

    fileInfo.format = 0;
    compareFilePtrCh2.reset(comparePath2, SFM_READ, &fileInfo);
    if (!compareFilePtrCh2)
        QFAIL("Unable to open compare wave file channel 2.");

    unsigned blockSize = 44100;
    QVector<float> outputDataCh1(blockSize);
    QVector<float> outputDataCh2(blockSize);

    QVector<float *> outputSplitData;
    outputSplitData.append(outputDataCh1.data());
    outputSplitData.append(outputDataCh2.data());

    const unsigned output1Count = sf_read_float(
        outputFilePtrCh1.get(), outputDataCh1.data(), outputDataCh1.size());
    QCOMPARE(output1Count, blockSize);

    const unsigned output2Count = sf_read_float(
        outputFilePtrCh2.get(), outputDataCh2.data(), outputDataCh2.size());
    QCOMPARE(output2Count, blockSize);

    QVector<float> compareDataCh1(blockSize);
    QVector<float> compareDataCh2(blockSize);

    QVector<float *> compareSplitData;
    compareSplitData.append(compareDataCh1.data());
    compareSplitData.append(compareDataCh2.data());

    const unsigned compare1Count = sf_read_float(
        compareFilePtrCh1.get(), compareDataCh1.data(), compareDataCh1.size());
    QCOMPARE(compare1Count, blockSize);

    const unsigned compare2Count = sf_read_float(
        compareFilePtrCh2.get(), compareDataCh2.data(), compareDataCh2.size());
    QCOMPARE(compare2Count, blockSize);

    const double compareValue = 1.0 / 32768;
    for (unsigned i = 0; i < 2; ++i) {

        ARRAYFUZZCOMP(outputSplitData[i], compareSplitData[i], compareValue,
                      blockSize);

        /*for (unsigned j = 0; j < blockSize; ++j) {
            float outputdata = outputSplitData[i][j];
            float comparedata = compareSplitData[i][j];
            if (!(qAbs(outputdata - comparedata) < compareValue)) {
                QFAIL(qPrintable(QString::fromLatin1
                    ("Compared values at [%4][%5] differ more than %1.\n"
                    "  Actual: %2\n"
                    "  Expected: %3\n"
                    "  Difference: %6")
                    .arg(compareValue)
                    .arg(outputdata)
                    .arg(comparedata)
                    .arg(i)
                    .arg(j)
                    .arg(qAbs(outputdata - comparedata))));
            }
        }*/
    }

    TEST_EXCEPTIONS_CATCH
}
