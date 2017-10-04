#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"

#include "tests.h"

#include <QFile>

using namespace bertha;
using namespace cmn;

void testMwfXmlFile(QString xmlFile, QString compareFileCh1,
                    QString compareFileCh2);

void extractBlocks(float *input, unsigned channels, unsigned size,
                   float **output)
{
    for (unsigned i = 0; i < channels; ++i)
        for (unsigned j = 0; j < size; ++j)
            output[i][j] = input[j * channels + i];
}

void mergeBlocks(float **input, unsigned channels, unsigned size, float *output)
{
    for (unsigned i = 0; i < channels; ++i)
        for (unsigned j = 0; j < size; ++j)
            output[j * channels + i] = input[i][j];
}

void BerthaTest::testHrtfMwf_data()
{
    const QString hrtfSignalXmlFile =
        QLatin1String("testexperiments/hrtfsignaltest.xml");
    const QString threadedHrtfSignalXmlFile =
        QLatin1String("testexperiments/threadedhrtfsignaltest.xml");
    const QString hrtfSignalCompareFileCh1 =
        QLatin1String("testdata/hrtf_signal_man001_ch1.wav");
    const QString hrtfSignalCompareFileCh2 =
        QLatin1String("testdata/hrtf_signal_man001_ch2.wav");

    const QString hrtfNoiseXmlFile =
        QLatin1String("testexperiments/hrtfnoisetest.xml");
    const QString threadedHrtfNoiseXmlFile =
        QLatin1String("testexperiments/threadedhrtfnoisetest.xml");
    const QString hrtfNoiseCompareFileCh1 =
        QLatin1String("testdata/hrtf_noise_auditec16000_ch1.wav");
    const QString hrtfNoiseCompareFileCh2 =
        QLatin1String("testdata/hrtf_noise_auditec16000_ch2.wav");

    const QString wienerXmlFile =
        QLatin1String("testexperiments/wienertest.xml");
    const QString wienerCompareFileCh1 =
        QLatin1String("testdata/wiener_hrtfsignalnoisecombined_ch1.wav");
    const QString wienerCompareFileCh2 =
        QLatin1String("testdata/wiener_hrtfsignalnoisecombined_ch2.wav");

    const QString adapt0XmlFile =
        QLatin1String("testexperiments/adapttest_0.xml");
    const QString adapt0CompareFileCh1 =
        QLatin1String("testdata/adept_testoutput_0_ch1.wav");
    const QString adapt0CompareFileCh2 =
        QLatin1String("testdata/adept_testoutput_0_ch2.wav");

    const QString adapt10XmlFile =
        QLatin1String("testexperiments/adapttest_10.xml");
    const QString adapt10CompareFileCh1 =
        QLatin1String("testdata/adept_testoutput_10_ch1.wav");
    const QString adapt10CompareFileCh2 =
        QLatin1String("testdata/adept_testoutput_10_ch2.wav");

    QTest::addColumn<QString>("xmlFile");
    QTest::addColumn<QString>("compareFileCh1");
    QTest::addColumn<QString>("compareFileCh2");

    QTest::newRow("signal") << hrtfSignalXmlFile << hrtfSignalCompareFileCh1
                            << hrtfSignalCompareFileCh2;
    QTest::newRow("threadedsignal") << threadedHrtfSignalXmlFile
                                    << hrtfSignalCompareFileCh1
                                    << hrtfSignalCompareFileCh2;
    QTest::newRow("noise") << hrtfNoiseXmlFile << hrtfNoiseCompareFileCh1
                           << hrtfNoiseCompareFileCh2;
    QTest::newRow("threadednoise") << threadedHrtfNoiseXmlFile
                                   << hrtfNoiseCompareFileCh1
                                   << hrtfNoiseCompareFileCh2;
    QTest::newRow("wiener") << wienerXmlFile << wienerCompareFileCh1
                            << wienerCompareFileCh2;
    QTest::newRow("adapt0") << adapt0XmlFile << adapt0CompareFileCh1
                            << adapt0CompareFileCh2;
    QTest::newRow("adapt10") << adapt10XmlFile << adapt10CompareFileCh1
                             << adapt10CompareFileCh2;
}

void BerthaTest::testHrtfMwf()
{
#ifdef Q_OS_ANDROID
    QSKIP("Skipped on Android");
#endif

    QFETCH(QString, xmlFile);
    QFETCH(QString, compareFileCh1);
    QFETCH(QString, compareFileCh2);

    try {
        const unsigned channels = 2;
        const unsigned blockSize = 8192;

        QTemporaryFile tempFile1;
        tempFile1.open();
        QTemporaryFile tempFile2;
        tempFile2.open();

        const QString xmlPath =
            Paths::searchFile(xmlFile, Paths::dataDirectories());
        ExperimentProcessor processor(XmlLoader().loadFile(xmlPath));
        processor.setParameterValue(
            QString::fromLatin1("device1"), QString::fromLatin1("outputFiles"),
            tempFile1.fileName() + QLatin1Char(',') + tempFile2.fileName());

        processor.prepare();
        processor.start();
        QTest::qWait(250);
        processor.release();

        const QString comparePathCh1 =
            Paths::searchFile(compareFileCh1, Paths::dataDirectories());
        const QString comparePathCh2 =
            Paths::searchFile(compareFileCh2, Paths::dataDirectories());

        cmn::SndFile outputFilePtrCh1;
        cmn::SndFile outputFilePtrCh2;

        cmn::SndFile compareFilePtrCh1;
        cmn::SndFile compareFilePtrCh2;

        SF_INFO fileInfo;
        fileInfo.format = 0;
        outputFilePtrCh1.reset(tempFile1.fileName(), SFM_READ, &fileInfo);
        if (!outputFilePtrCh1)
            QFAIL("Unable to open output channel 1 wave file.");

        fileInfo.format = 0;
        outputFilePtrCh2.reset(tempFile2.fileName(), SFM_READ, &fileInfo);
        if (!outputFilePtrCh2)
            QFAIL("Unable to open output channel 2 wave file.");

        fileInfo.format = 0;
        compareFilePtrCh1.reset(comparePathCh1, SFM_READ, &fileInfo);
        if (!compareFilePtrCh1)
            QFAIL("Unable to open compare wave file channel 1.");

        fileInfo.format = 0;
        compareFilePtrCh2.reset(comparePathCh2, SFM_READ, &fileInfo);
        if (!compareFilePtrCh2)
            QFAIL("Unable to open compare wave file channel 2.");

        QVector<float> outputDataCh1(blockSize);
        QVector<float> outputDataCh2(blockSize);

        QVector<float *> outputSplitData;
        outputSplitData.append(outputDataCh1.data());
        outputSplitData.append(outputDataCh2.data());

        const unsigned output1Count = sf_read_float(
            outputFilePtrCh1.get(), outputDataCh1.data(), outputDataCh1.size());
        QCOMPARE(output1Count, 8192u);

        const unsigned output2Count = sf_read_float(
            outputFilePtrCh2.get(), outputDataCh2.data(), outputDataCh2.size());
        QCOMPARE(output2Count, 8192u);

        QVector<float> compareDataCh1(blockSize);
        QVector<float> compareDataCh2(blockSize);

        QVector<float *> compareSplitData;
        compareSplitData.append(compareDataCh1.data());
        compareSplitData.append(compareDataCh2.data());

        const unsigned compare1Count =
            sf_read_float(compareFilePtrCh1.get(), compareDataCh1.data(),
                          compareDataCh1.size());
        QCOMPARE(compare1Count, 8192u);

        const unsigned compare2Count =
            sf_read_float(compareFilePtrCh2.get(), compareDataCh2.data(),
                          compareDataCh2.size());
        QCOMPARE(compare2Count, 8192u);

        const double compareValue = 1.0 / (65536 - 2);
        for (unsigned i = 0; i < channels; ++i) {
            for (unsigned j = 0; j < blockSize; ++j) {
                float outputdata = outputSplitData[i][j];
                float comparedata = compareSplitData[i][j];
                if (!(qAbs(outputdata - comparedata) < compareValue)) {
                    QFAIL(qPrintable(
                        QString::fromLatin1(
                            "Compared values at [%4][%5] differ more than %1.\n"
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
            }
        }
    } catch (const std::exception &e) {
        QFAIL(qPrintable(QString::fromLatin1("No exception expected: %1")
                             .arg(QString::fromLocal8Bit(e.what()))));
    }
}
