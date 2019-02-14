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

unsigned numberOfSamples(const QString &filename)
{
    return SndFile::frames(filename);
}

void BerthaTest::testFileInput()
{
    TEST_EXCEPTIONS_TRY

    const QString xmlFile =
        QLatin1String("testexperiments/fileinputexperiment.xml");
    const QString inputFile = QLatin1String("testdata/sin1000Hz.wav");

    TemporaryDirectory outputDir;
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());
    const QString comparePath =
        Paths::searchFile(inputFile, Paths::dataDirectories());
    const QString outputPath = outputDir.addFile(QLatin1String("output.wav"));

    unsigned nsamples = numberOfSamples(comparePath);

    QFile file(xmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = file.readAll();
    data.replace("OUTPUTFILEPLACEHOLDER", outputPath.toUtf8());
    ExperimentData expdata(XmlLoader().loadContents(data));

    // No looping
    {
        ExperimentProcessor processor(expdata);

        processor.setParameterValue(QString::fromLatin1("block"),
                                    QString::fromLatin1("fileName"),
                                    comparePath);

        processor.prepare();
        processor.start();
        QTest::qWait(2000);
        processor.release();

        cmn::SndFile outputFilePtr;
        cmn::SndFile compareFilePtr;

        SF_INFO outputSfinfo;
        SF_INFO compareSfinfo;
        outputSfinfo.format = 0;
        compareSfinfo.format = 0;
        QTest::qWait(200);
        outputFilePtr.reset(outputPath, SFM_READ, &outputSfinfo);
        compareFilePtr.reset(comparePath, SFM_READ, &compareSfinfo);

        QVERIFY(outputFilePtr);
        QVERIFY(compareFilePtr);

        QVector<float> outputData(2 * nsamples);
        const unsigned outputCount = sf_read_float(
            outputFilePtr.get(), outputData.data(), outputData.size());
        QCOMPARE(outputCount, 2 * nsamples);
        QVector<float> compareData(nsamples);
        const unsigned compareCount = sf_read_float(
            compareFilePtr.get(), compareData.data(), compareData.size());
        QCOMPARE(compareCount, nsamples);

        ARRAYFUZZCOMP(outputData, compareData, 1.0 / 32768, nsamples);
        for (unsigned i = nsamples; i < 2 * nsamples; ++i) {
            QVERIFY(qAbs(outputData[i]) == 0);
        }
    }

    // Looping and zero-padding
    {
        ExperimentProcessor processor(expdata);

        int blocksize = expdata.device().blockSize();

        processor.setParameterValue(QString::fromLatin1("block"),
                                    QString::fromLatin1("fileName"),
                                    comparePath);
        processor.setParameterValue(QLatin1String("block"),
                                    QLatin1String("loop"), 1);
        processor.setParameterValue(QLatin1String("block"),
                                    QLatin1String("zeroPadLoop"), 1);

        processor.prepare();
        processor.start();
        QTest::qWait(2000);
        processor.release();

        cmn::SndFile outputFilePtr;
        cmn::SndFile compareFilePtr;

        SF_INFO outputSfinfo;
        SF_INFO compareSfinfo;
        outputSfinfo.format = 0;
        compareSfinfo.format = 0;
        QTest::qWait(200);
        outputFilePtr.reset(outputPath, SFM_READ, &outputSfinfo);
        compareFilePtr.reset(comparePath, SFM_READ, &compareSfinfo);

        QVERIFY(outputFilePtr);
        QVERIFY(compareFilePtr);

        QVector<float> outputData(4 * nsamples);
        const unsigned outputCount = sf_read_float(
            outputFilePtr.get(), outputData.data(), outputData.size());

        QCOMPARE(outputCount, 4 * nsamples);
        QVector<float> compareData(nsamples);
        const unsigned compareCount = sf_read_float(
            compareFilePtr.get(), compareData.data(), compareData.size());
        QCOMPARE(compareCount, nsamples);

        for (unsigned i = 0; i < nsamples; ++i) {
            QVERIFY(qAbs(outputData[i] - compareData[i]) < 1.0 / 32768);
        }
        int edge = nsamples + blocksize - (nsamples % blocksize);
        for (unsigned i = nsamples; i < unsigned(edge); ++i) {
            QVERIFY(qAbs(outputData[i]) == 0);
        }
        for (unsigned i = edge; i < edge + nsamples; ++i) {
            QVERIFY(qAbs(outputData[i] - compareData[i - edge]) < 1.0 / 32768);
        }
    }

    // Looping without zero-padding
    {
        ExperimentProcessor processor(expdata);

        processor.setParameterValue(QString::fromLatin1("block"),
                                    QString::fromLatin1("fileName"),
                                    comparePath);
        processor.setParameterValue(QLatin1String("block"),
                                    QLatin1String("loop"), 1);
        processor.setParameterValue(QLatin1String("block"),
                                    QLatin1String("zeroPadLoop"), 0);

        processor.prepare();
        processor.start();
        QTest::qWait(2000);
        processor.release();

        cmn::SndFile outputFilePtr;
        cmn::SndFile compareFilePtr;

        SF_INFO outputSfinfo;
        SF_INFO compareSfinfo;
        outputSfinfo.format = 0;
        compareSfinfo.format = 0;
        QTest::qWait(200);
        outputFilePtr.reset(outputPath, SFM_READ, &outputSfinfo);
        compareFilePtr.reset(comparePath, SFM_READ, &compareSfinfo);

        QVERIFY(outputFilePtr);
        QVERIFY(compareFilePtr);

        QVector<float> outputData(4 * nsamples);
        const unsigned outputCount = sf_read_float(
            outputFilePtr.get(), outputData.data(), outputData.size());

        QCOMPARE(outputCount, 4 * nsamples);
        QVector<float> compareData(nsamples);
        const unsigned compareCount = sf_read_float(
            compareFilePtr.get(), compareData.data(), compareData.size());
        QCOMPARE(compareCount, nsamples);

        ARRAYFUZZCOMP(outputData, compareData, 1.0 / 32768, nsamples);
        ARRAYFUZZCOMP(outputData.mid(nsamples), compareData, 1.0 / 32768,
                      nsamples);
        ARRAYFUZZCOMP(outputData.mid(nsamples * 2), compareData, 1.0 / 32768,
                      nsamples);
    }

    TEST_EXCEPTIONS_CATCH
}
