/******************************************************************************
 * Copyright (C) 2009  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/datablock/datablockdata.h"

#include "apexdata/experimentdata.h"
#include "apexdata/mainconfigfiledata.h"

#include "apexdata/map/apexmap.h"

#include "apexdata/randomgenerator/randomgeneratorparameters.h"

#include "apexdata/stimulus/nucleus/nicstream/stimulation_mode.h"

#include "apexmain/accessmanager.h"

#include "apexmain/calibration/soundlevelmeter.h"

#include "apexmain/cohstimulus/cohdatablock.h"

#include "apexmain/device/soundcardsettings.h"

#include "apexmain/experimentparser.h"

#include "apexmain/experimentstore/manageddirectory.h"

#include "apexmain/gui/soundcardsdialog.h"

#include "apexmain/interactive/interactiveparameters.h"

#include "apexmain/mainconfigfileparser.h"

#include "apexmain/randomgenerator/uniformrandomgenerator.h"

#include "apexmain/resultsink/resultviewer.h"
#include "apexmain/resultsink/rtresultsink.h"

#include "apextools/apexpaths.h"
#include "apextools/apexpluginloader.h"
#include "apextools/apextools.h"
#include "apextools/version.h"

#include "apextools/xml/xmltools.h"

#include "coh/aseqloader.h"
#include "coh/cohnicxmldumper.h"
#include "coh/cohtextdumper.h"

#include "common/debug.h"
#include "common/exception.h"
#include "common/paths.h"
#include "common/pluginloader.h"
#include "common/testutils.h"

#include "commongui/webview.h"

#include "apexmaintest.h"

#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QTest>
#include <QUrl>

#if !defined(Q_OS_ANDROID)
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#endif

using namespace apex;
using namespace apex::stimulus;
using namespace apex::data;
using namespace cmn;
using namespace coh;

void ApexMainTest::initTestCase()
{
    enableCoreDumps(QCoreApplication::applicationFilePath());
    networkError = false;
}

void ApexMainTest::testAseq()
{
    TEST_EXCEPTIONS_TRY

    {
        const unsigned char data[] = {
            82, 73, 70,  70, 72, 0,  0,   0,   65, 83, 69,  81, 73, 78, 70,
            79, 20, 0,   0,  0,  0,  0,   128, 63, 67, 72,  65, 78, 3,  0,
            0,  0,  77,  65, 71, 78, 3,   0,   0,  0,  67,  72, 65, 78, 3,
            0,  0,  0,   1,  2,  3,  0,   77,  65, 71, 78,  12, 0,  0,  0,
            0,  0,  160, 64, 0,  0,  192, 64,  0,  0,  224, 64};

        QString result =
            QL1S("Sequence: repeats 1\n"
                 "  Biphasic: active nan reference nan level nan width nan gap "
                 "nan period nan channel 1 magnitude 5.0 trigger 0\n"
                 "  Biphasic: active nan reference nan level nan width nan gap "
                 "nan period nan channel 2 magnitude 6.0 trigger 0\n"
                 "  Biphasic: active nan reference nan level nan width nan gap "
                 "nan period nan channel 3 magnitude 7.0 trigger 0\n");

        try {
            QScopedPointer<CohSequence> parsed(loadCohSequenceAseq(
                QByteArray((char *)data, sizeof(data)), true));
            QCOMPARE(dumpCohSequenceText(parsed.data()), result);
        } catch (const std::exception &e) {
            QFAIL(e.what());
        }
    }

    {
        const unsigned char data[] = {
            82,  73, 70, 70, 134, 0,  0,  0,   65, 83,  69, 81, 73, 78, 70,
            79,  44, 0,  0,  0,   0,  0,  128, 63, 65,  69, 76, 69, 4,  0,
            0,   0,  82, 69, 76,  69, 1,  0,   0,  0,   67, 85, 82, 76, 4,
            0,   0,  0,  80, 72,  87, 73, 1,   0,  0,   0,  80, 69, 82, 73,
            4,   0,  0,  0,  65,  69, 76, 69,  4,  0,   0,  0,  10, 11, 12,
            13,  82, 69, 76, 69,  1,  0,  0,   0,  255, 0,  67, 85, 82, 76,
            4,   0,  0,  0,  22,  23, 24, 25,  80, 72,  87, 73, 4,  0,  0,
            0,   0,  0,  32, 65,  80, 69, 82,  73, 16,  0,  0,  0,  0,  0,
            220, 66, 0,  0,  240, 66, 0,  0,   2,  67,  0,  0,  12, 67};

        QString result =
            QL1S("Sequence: repeats 1\n"
                 "  Biphasic: active 10 reference -1 level 22 width 10.0 gap "
                 "nan period 110.0 channel nan magnitude nan trigger 0\n"
                 "  Biphasic: active 11 reference -1 level 23 width 10.0 gap "
                 "nan period 120.0 channel nan magnitude nan trigger 0\n"
                 "  Biphasic: active 12 reference -1 level 24 width 10.0 gap "
                 "nan period 130.0 channel nan magnitude nan trigger 0\n"
                 "  Biphasic: active 13 reference -1 level 25 width 10.0 gap "
                 "nan period 140.0 channel nan magnitude nan trigger 0\n");

        // seq.electrodes=[10 11 12 13];
        // seq.return_electrodes=[-1];
        // seq.current_levels=[22 23 24 25];
        // seq.phase_widths=10;
        // seq.periods=[110 120 130 140];

        try {
            QScopedPointer<CohSequence> parsed(loadCohSequenceAseq(
                QByteArray((char *)data, sizeof(data)), true));
            QCOMPARE(dumpCohSequenceText(parsed.data()), result);
        } catch (const std::exception &e) {
            QFAIL(e.what());
        }
    }

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testCohDatablockInvalidFile()
{
    TEST_EXCEPTIONS_TRY

    DatablockData data;

    {
        QString file("invalidfilenameamzleirjalmijh");
        data.setFile(file);
        bool ex = false;
        try {
            CohDataBlock(data, file, 0);
        } catch (const std::exception &) {
            ex = true;
        }
        QCOMPARE(ex, true);
    }

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testCohDatablockAseq_data()
{
    QTest::addColumn<QString>("reference");

    QTest::newRow("sequence1.aseq") << "MP1";
    QTest::newRow("chanmag.aseq") << "MP1";
    QTest::newRow("peri.aseq") << "MP1";
    QTest::newRow("phga.aseq") << "MP1";
    QTest::newRow("phwi.aseq") << "MP1";
    QTest::newRow("rele.aseq") << "MP2";
    QTest::newRow("powerup1.aseq") << "MP1";
    QTest::newRow("powerup2.aseq") << "MP1";
    QTest::newRow("powerup3.aseq") << "MP1";
    QTest::newRow("powerup4.aseq") << "MP1";
}

void ApexMainTest::testCohDatablockAseq()
{
    TEST_EXCEPTIONS_TRY

    DatablockData data;
    ApexMap map;
    ChannelMap basemap;

    QFETCH(QString, reference);

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType(reference));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i = 1; i <= 22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22 - i + 1);
        basemap.setThresholdLevel(1 + i);
        basemap.setComfortLevel(255 - i);
        QVERIFY(basemap.isValid());
        map[basemap.channelNumber()] = basemap;
    }

    // parse aseq files and write XML results
    QString filename(QTest::currentDataTag());

    QString filePath(Paths::searchFile(QL1S("tests/libapex/") + filename,
                                       Paths::dataDirectories()));
    data.setFile(filePath);
    CohDataBlock datablock(data, filePath, 0);

    QScopedPointer<CohSequence> xmlSequence(datablock.mappedData(&map, 100));
    QList<QByteArray> xmlData(
        dumpCohSequenceNicXml(xmlSequence.data()).split('\n'));

    // Read reference data
    QFile file(
        Paths::searchFile(QL1S("tests/libapex/") + filename + QL1S(".xml"),
                          Paths::dataDirectories()));
    QVERIFY(file.open(QIODevice::ReadOnly));
    QTextStream t(&file);
    for (int i = 0; i < xmlData.length(); ++i) {
        QCOMPARE(QString::fromLatin1(xmlData.at(i)), t.readLine(0));
    }
    file.close();

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testAseqParser()
{
    TEST_EXCEPTIONS_TRY

    QFile file(Paths::searchFile(QL1S("tests/libapex/quantization.aseq"),
                                 Paths::dataDirectories()));
    if (!file.open(QIODevice::ReadOnly)) {
        QFAIL("Can't open file for reading");
    }

    QString result =
        QL1S("Sequence: repeats 1\n"
             "  Biphasic: active nan reference nan level nan width nan gap nan "
             "period 120.0 channel 1 magnitude 0.1 trigger 0\n"
             "  Biphasic: active nan reference nan level nan width nan gap nan "
             "period 120.2 channel 2 magnitude 0.2 trigger 0\n"
             "  Biphasic: active nan reference nan level nan width nan gap nan "
             "period 120.4 channel 3 magnitude 0.1 trigger 0\n"
             "  Biphasic: active nan reference nan level nan width nan gap nan "
             "period 120.8 channel 4 magnitude 0.3 trigger 0\n");

    QScopedPointer<CohSequence> parsed(
        loadCohSequenceAseq(file.readAll(), true));
    QCOMPARE(dumpCohSequenceText(parsed.data()), result);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testCohDatablockAseqMapping()
{
    TEST_EXCEPTIONS_TRY

    DatablockData data;
    ApexMap map;
    ChannelMap basemap;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i = 1; i <= 22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22 - i + 1);
        basemap.setThresholdLevel(1 + i);
        basemap.setComfortLevel(255 - i);
        QVERIFY(basemap.isValid());
        map[basemap.channelNumber()] = basemap;
    }

    // parse aseq files and write XML results
    QString filePath(Paths::searchFile(QL1S("tests/libapex/mapping.aseq"),
                                       Paths::dataDirectories()));
    data.setFile(filePath);
    CohDataBlock datablock(data, filePath, 0);

    QScopedPointer<CohSequence> xmlSequence(datablock.mappedData(&map, 100));
    QList<QByteArray> xmlData(
        dumpCohSequenceNicXml(xmlSequence.data()).split('\n'));

    QFile file(Paths::searchFile(QL1S("tests/libapex/mapping.aseq.xml"),
                                 Paths::dataDirectories()));
    QVERIFY(file.open(QIODevice::ReadOnly));
    QTextStream t(&file);
    for (int i = 0; i < xmlData.length(); ++i) {
        QCOMPARE(QString::fromLatin1(xmlData.at(i)), t.readLine(0));
    }
    file.close();

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testCohDatablock_invalid()
{
    TEST_EXCEPTIONS_TRY

    DatablockData data;
    ApexMap map;
    ChannelMap basemap;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i = 1; i <= 22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22 - i + 1);
        basemap.setThresholdLevel(1 + i);
        basemap.setComfortLevel(255 - i);
        QVERIFY(basemap.isValid());
        map[basemap.channelNumber()] = basemap;
    }

    QString file(Paths::searchFile(QL1S("tests/libapex/invalid1.aseq"),
                                   Paths::dataDirectories()));
    data.setFile(file);

    try {
        CohDataBlock datablock(data, file, 0);
        datablock.mappedData(&map, 100);
        QFAIL("invalid datablock did not cause exception");
    } catch (...) {
        // expected
    }

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testCohInvalidCL()
{
    TEST_EXCEPTIONS_TRY

    ChannelMap basemap;
    DatablockData data;
    ApexMap map;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i = 1; i <= 22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22 - i + 1);
        basemap.setThresholdLevel(1 + i);
        basemap.setComfortLevel(100);
        QVERIFY(basemap.isValid());
        map[basemap.channelNumber()] = basemap;
    }

    QString file(Paths::searchFile(QL1S("tests/libapex/chancl.aseq"),
                                   Paths::dataDirectories()));
    data.setFile(file);

    try {
        CohDataBlock datablock(data, file, 0);
        datablock.mappedData(&map, 100);
        QFAIL("invalid datablock did not cause exception");
    } catch (...) {
        // expected
    }

    TEST_EXCEPTIONS_CATCH
}

class RandomGeneratorTestParameters : public RandomGeneratorParameters
{
public:
    void setParameter(QString name, QString value)
    {
        SetParameter(name, "", value, QDomElement());
    }
    QString min()
    {
        return QString::number(m_dMin);
    }
    QString max()
    {
        return QString::number(m_dMax);
    }
};

template <typename T>
T convert(QString, bool *)
{
    QVERIFY(false);
}

template <>
int convert(QString s, bool *ok)
{
    return s.toInt(ok);
}

template <>
double convert(QString s, bool *ok)
{
    return s.toDouble(ok);
}

template <typename T>
void ApexMainTest::verifyInterval(RandomGeneratorTestParameters *params, T min,
                                  T max)
{
    UniformRandomGenerator *urg = new UniformRandomGenerator(params);
    for (unsigned int i = 0; i < 100; ++i) {
        QString stringValue = urg->GetNextValue();
        bool ok = false;
        T value = convert<T>(stringValue, &ok);

        QVERIFY(ok);
        QVERIFY(value <= max);
        QVERIFY(value >= min);
    }
}

void ApexMainTest::testUniformInt()
{
    TEST_EXCEPTIONS_TRY

    long min = 0;
    long max = 10;

    RandomGeneratorTestParameters params;
    params.setParameter("type", "int");
    params.setParameter("min", QString::number(min));
    params.setParameter("max", QString::number(max));

    UniformRandomGenerator *urg = new UniformRandomGenerator(&params);
    urg->doDeterministicGeneration();
    double p = 1 / double(max - min + 1);
    unsigned long n = 1000 / p;

    QMap<int, unsigned long> freq;
    for (unsigned long i = 0; i < n; ++i) {
        bool ok = false;
        long value = convert<int>(urg->GetNextValue(), &ok);
        ++freq[value];
    }

    unsigned long squaredSum = 0;
    foreach (unsigned long f, freq) {
        squaredSum += (f * f);
    }

    double v = (1 / (n * p)) * squaredSum - n;

    QVERIFY(v >= 2.558);
    QVERIFY(v <= 23.21);

    TEST_EXCEPTIONS_CATCH
}

double f(double val, double min, double max)
{
    return (val - min) / (max - min);
}

void ApexMainTest::testUniformDouble()
{
    TEST_EXCEPTIONS_TRY

    double min = 0;
    double max = 10;

    RandomGeneratorTestParameters params;
    params.setParameter("type", "double");
    params.setParameter("min", QString::number(min));
    params.setParameter("max", QString::number(max));

    UniformRandomGenerator *urg = new UniformRandomGenerator(&params);
    urg->doDeterministicGeneration();
    unsigned long n = 1000;

    QVector<double> numbers;
    for (unsigned long i = 0; i < n; ++i) {
        bool ok;
        double value = convert<double>(urg->GetNextValue(), &ok);
        numbers.push_back(value);
    }

    qSort(numbers);

    QVector<double> transformed1;
    QVector<double> transformed2;
    for (size_t j = 1; j <= n; ++j) {
        size_t i = j - 1;
        transformed1.push_back(double(j) / n - f(numbers[i], min, max));
        transformed2.push_back(f(numbers[i], min, max) - double(j - 1) / n);
    }

    double kMin =
        qSqrt(n) * *std::max_element(transformed1.begin(), transformed1.end());
    double kMax =
        qSqrt(n) * *std::max_element(transformed2.begin(), transformed2.end());

    QVERIFY(kMin >= 0.02912);
    QVERIFY(kMin <= 1.444);

    QVERIFY(kMax >= 0.02912);
    QVERIFY(kMax <= 1.444);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testRandomInterval()
{
    TEST_EXCEPTIONS_TRY

    RandomGeneratorTestParameters params;
    params.setParameter("type", "int");
    params.setParameter("min", "0");
    params.setParameter("max", "10");
    verifyInterval<int>(&params, 0, 10);

    params.setParameter("min", "1");
    verifyInterval<int>(&params, 1, 10);

    params.setParameter("min", "2.4");
    verifyInterval<int>(&params, 2, 10);

    params.setParameter("max", "9.4");
    verifyInterval<int>(&params, 2, 9);

    params.setParameter("min", "-2.3");
    verifyInterval<int>(&params, -2, 9);

    params.setParameter("max", "-1.3");
    verifyInterval<int>(&params, -2, -1);

    params.setParameter("type", "double");
    params.setParameter("min", "0");
    params.setParameter("max", "10");
    verifyInterval<double>(&params, 0, 10);

    params.setParameter("min", "1");
    verifyInterval<double>(&params, 1, 10);

    params.setParameter("min", "2.4");
    verifyInterval<double>(&params, 2.4, 10);

    params.setParameter("max", "9.4");
    verifyInterval<double>(&params, 2, 9.4);

    params.setParameter("min", "-2.3");
    verifyInterval<double>(&params, -2.3, 9.4);

    params.setParameter("max", "-1.3");
    verifyInterval<double>(&params, -2.3, -1.3);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testMainConfigFileParser()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileParser::Get().parse();
    const data::MainConfigFileData &d = MainConfigFileParser::Get().data();

    QCOMPARE(d.soundCardName("RME", "asio"), QString("ASIO Hammerfall DSP"));
    QCOMPARE(d.soundCardName("invalid", "asio"), QString());
    QCOMPARE(d.prefix("regression"), QString("../stimuli"));
    QCOMPARE(d.prefix("invalid"), QString(""));
    QCOMPARE(d.pluginScriptLibrary(), QString("pluginscriptlibrary.js"));

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testUpgradeTo3_1_1()
{
    TEST_EXCEPTIONS_TRY

    // Load the xml file to test. This is a file from version 3.1.0
    QFile testFile(
        Paths::searchFile(QL1S("tests/libapex/results-upgradeoldschema.apx"),
                          Paths::dataDirectories()));
    testFile.open(QIODevice::ReadOnly);

    // Create the experiment parser
    apex::data::ExperimentData *testData;

    apex::ExperimentParser parser(testFile.fileName());

    // Parse the xml file, this should call the upgradeTo3_1_1 function
    // apex::data::ExperimentData* testData;
    testData = parser.parse(false);

    // Get the results from the parsing
    apex::data::ResultParameters *parameterList = testData->resultParameters();

    // now test the result of the parsing...
    QCOMPARE(parameterList->showResultsAfter(),
             true); // This is supposed to be true
    QCOMPARE(parameterList->showRTResults(), false);
    QCOMPARE(parameterList->resultPage(), QUrl("apex:resultsviewer.html"));
    QVERIFY(parameterList->matlabScript().isEmpty());
    // QVERIFY(parameterList->GetXsltScript().isEmpty());
    // QVERIFY(parameterList->getJavascriptScript().isEmpty());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testResultViewerConvertToCSV_data()
{
    QTest::addColumn<QString>("resultFilePath");
    QTest::addColumn<QString>("testFilePath");
    QTest::addColumn<QString>("desiredFilePath");

    QTest::newRow("Simple")
        << Paths::searchFile(QL1S("tests/libapex/subject_test-r.apr"),
                             Paths::dataDirectories())
        << Paths::searchFile(QL1S("tests/libapex/testJavascriptResult2.xml"),
                             Paths::dataDirectories())
        << Paths::searchFile(QL1S("tests/libapex/desiredJavascriptResult.xml"),
                             Paths::dataDirectories());
}

void ApexMainTest::testResultViewerConvertToCSV()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    // First, create the result parameters
    data::ResultParameters rvparam;
    rvparam.setShowResultsAfter(true);
    rvparam.setSaveResults(false);

    // Fetch the filenames:
    QFETCH(QString, resultFilePath);
    QFETCH(QString, testFilePath);
    QFETCH(QString, desiredFilePath);

    QFileInfo resultFileInfo(resultFilePath);
    QString resultFile =
        resultFileInfo.absoluteFilePath(); // To get the right path on all
    // operating systems.

    apex::ResultViewer testViewer(&rvparam, resultFile);

    // Export the contents to a file:
    QFileInfo testFileInfo(testFilePath);
    QString testFile = testFileInfo.absoluteFilePath();

    // export the new data to the existing result file.
    testViewer.addtofile(testFile);
    QDomDocument testResult = readXmlResults(testFile);

    // Get the comparison values
    QFileInfo desiredFileInfo(desiredFilePath);
    QDomDocument desiredContent =
        readXmlResults(desiredFileInfo.absoluteFilePath());

    QCOMPARE(compareXml(testResult, desiredContent), QString());

    // Now remove the part that has been written to the text file, so the test
    // can be done again...
    QFile file(testFile);
    file.open(QIODevice::ReadWrite);
    QByteArray fileData = file.readAll();
    QString fileText(fileData);

    QString processedTag = "<processed>";

    int startIndex = fileText.lastIndexOf(processedTag) + processedTag.length();
    int endIndex = fileText.lastIndexOf("</processed>");

    fileText.remove(startIndex, endIndex - startIndex);

    // Now clear the old file, and write contents to the new file

    file.resize(0);
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << fileText;
    file.close();

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testResultViewer_data()
{
    QTest::addColumn<QString>("resultFilePath");
    QTest::addColumn<QString>("resultViewerPath");
    QTest::addColumn<QString>("plotConfig"); // sets up config object correctly
    QTest::addColumn<QString>(
        "testCommand"); // JSON.stringify(plots.<type>.data) +
                        // JSON.stringify(plots.text.data)
    QTest::addColumn<QString>("plotDataJSON"); // Target result JSON

    // Paths::searchFile(QL1S("data/resultsviewer/resultsviewer.html"),
    // Paths::dataDirectories())
    QTest::newRow("line")
        << Paths::searchFile(QL1S("data/tests/libapex/results-test-line.apr"),
                             Paths::dataDirectories())
        << Paths::searchFile(QL1S("data/resultsviewer/resultsviewer.html"),
                             Paths::dataDirectories())
        << "config = { global: { removefromanswer: "
           "[\"_button\",\"button_\",\"button\",\"b_\",\"answer_\",\"_answer\","
           "\"answer\"] }, line: { show: true, parameterkey: "
           "\"parametervalue\", trialsformean: 6, reversalsformean: 6 }, "
           "matrix: { show: false }, polar: { show:false }, text: { show: true "
           "} };"
        << "JSON.stringify( plots.line.data ) + JSON.stringify( "
           "plots.text.data );"
        << "[{\"values\":[-5,-3,-1,-3,-1,-3,-1,1,3,1,-1,1,-1],\"reversals\":[-"
           "1,-3,-1,-3,3,-1,1],\"meanrevs\":-0.6666666666666666,\"meanrevstd\":"
           "2.1343747458109497,\"meantrials\":0.6666666666666666,"
           "\"meantrialstd\":1.3743685418725535}][{\"line\":{\"allvalues\":\"<"
           "tr><td>Parametervalues</td><td>-5,-3,-1,-3,-1,-3,-1,1,3,1,-1,1,-1</"
           "td></tr>\",\"lastvalue\":\"<tr><td>Lastvalue</td><td>-1</td></"
           "tr>\",\"reversals\":\"<tr><td>Reversals</"
           "td><td>-1,-3,-1,-3,3,-1,1</td></"
           "tr>\",\"meanrevs\":\"<tr><tdclass=\\\"dataname\\\">Mean(std)<br>"
           "last6reversals</"
           "tdclass=\\\"dataname\\\"><td>-0.6667(&plusmn;2.134)</td></"
           "tr>\",\"meantrials\":\"<tr><tdclass=\\\"dataname\\\">Mean(std)<br>"
           "last6trials</tdclass=\\\"dataname\\\"><td>0.6667(&plusmn1.374)</"
           "td></"
           "tr>\",\"linedatatable\":\"<tableclass=\\\"datatable\\\"><tr><td>"
           "Parametervalues</td><td>-5,-3,-1,-3,-1,-3,-1,1,3,1,-1,1,-1</td></"
           "tr><tr><td>Lastvalue</td><td>-1</td></tr><tr><td>Reversals</"
           "td><td>-1,-3,-1,-3,3,-1,1</td></"
           "tr><tr><tdclass=\\\"dataname\\\">Mean(std)<br>last6reversals</"
           "tdclass=\\\"dataname\\\"><td>-0.6667(&plusmn;2.134)</td></"
           "tr><tr><tdclass=\\\"dataname\\\">Mean(std)<br>last6trials</"
           "tdclass=\\\"dataname\\\"><td>0.6667(&plusmn1.374)</td></tr></"
           "table>\",\"paramtable\":\"<tableclass=\\\"stripedtable\\\"><tr><th>"
           "Trial</th><th>Answer</th><th>Correct</th><th>Parameter</th></"
           "tr><tr><td>1</td><td>wrong</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>-5</"
           "td></tr><tr><td>2</td><td>wrong</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>-3</"
           "td></tr><tr><td>3</td><td>correct</"
           "td><td><spanclass=\\\"correct\\\">Correct</span></td><td>-1</td></"
           "tr><tr><td>4</td><td>wrong</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>-3</"
           "td></tr><tr><td>5</td><td>correct</"
           "td><td><spanclass=\\\"correct\\\">Correct</span></td><td>-1</td></"
           "tr><tr><td>6</td><td>wrong</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>-3</"
           "td></tr><tr><td>7</td><td>wrong</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>-1</"
           "td></tr><tr><td>8</td><td>wrong</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>1</"
           "td></tr><tr><td>9</td><td>correct</"
           "td><td><spanclass=\\\"correct\\\">Correct</span></td><td>3</td></"
           "tr><tr><td>10</td><td>correct</"
           "td><td><spanclass=\\\"correct\\\">Correct</span></td><td>1</td></"
           "tr><tr><td>11</td><td>wrong</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>-1</"
           "td></tr><tr><td>12</td><td>correct</"
           "td><td><spanclass=\\\"correct\\\">Correct</span></td><td>1</td></"
           "tr><tr><td>13</td><td>n/a</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td><td>-1</"
           "td></tr></table>\"}}]";

    QTest::newRow("matrixpolar")
        << Paths::searchFile(
               QL1S("data/tests/libapex/results-test-matrixpolar.apr"),
               Paths::dataDirectories())
        << Paths::searchFile(QL1S("data/resultsviewer/resultsviewer.html"),
                             Paths::dataDirectories())
        << "$.extend( config, { global: { stimuli: "
           "[\"-90\",\"-75\",\"-60\",\"-45\",\"-30\",\"-15\",\"0\",\"15\","
           "\"30\",\"45\",\"60\",\"75\",\"90\"],  removefromanswer: "
           "[\"_button\",\"button_\",\"button\",\"b_\",\"answer_\",\"_answer\","
           "\"answer\"] },line: {show: false }, matrix: { show: true },polar: "
           "{ show: true, mindegree: -90, maxdegree: 90 }, text: { show: true "
           "} } );"
        << "JSON.stringify( plots.matrix.data );"
        << "[{\"values\":[{\"x\":0,\"y\":1,\"z\":1},{\"x\":1,\"y\":0,\"z\":1},{"
           "\"x\":2,\"y\":3,\"z\":1},{\"x\":3,\"y\":2,\"z\":2},{\"x\":4,\"y\":"
           "3,\"z\":1},{\"x\":6,\"y\":3,\"z\":1},{\"x\":7,\"y\":1,\"z\":1},{"
           "\"x\":7,\"y\":4,\"z\":1},{\"x\":7,\"y\":6,\"z\":1},{\"x\":8,\"y\":"
           "4,\"z\":1},{\"x\":8,\"y\":6,\"z\":1},{\"x\":9,\"y\":7,\"z\":1},{"
           "\"x\":10,\"y\":8,\"z\":2},{\"x\":12,\"y\":5,\"z\":1},{\"x\":12,"
           "\"y\":6,\"z\":1},{\"x\":12,\"y\":8,\"z\":1}],\"percentages\":[0,0,"
           "0,0,0,0,0,0.3333333333333333,0,0,0,0,0],\"xlabels\":[\"-90\",\"-"
           "75\",\"-60\",\"-45\",\"-30\",\"-15\",\"0\",\"15\",\"30\",\"45\","
           "\"60\",\"75\",\"90\"],\"ylabels\":[\"-90\",\"-60\",\"-30\",\"-15\","
           "\"15\",\"30\",\"45\",\"60\",\"75\"],\"raw\":[[0,1,0,0,0,0,0,0,0],["
           "1,0,0,0,0,0,0,0,0],[0,0,0,1,0,0,0,0,0],[0,0,2,0,0,0,0,0,0],[0,0,0,"
           "1,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,1,0,0,0,0,0],[0,1,0,0,1,0,"
           "1,0,0],[0,0,0,0,1,0,1,0,0],[0,0,0,0,0,0,0,1,0],[0,0,0,0,0,0,0,0,2],"
           "[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,1,1,0,1]]}]";

    QTest::newRow("matrixpolartext")
        << Paths::searchFile(
               QL1S("data/tests/libapex/results-test-matrixpolar.apr"),
               Paths::dataDirectories())
        << Paths::searchFile(QL1S("data/resultsviewer/resultsviewer.html"),
                             Paths::dataDirectories())
        << "$.extend( config, { global: { stimuli: "
           "[\"-90\",\"-75\",\"-60\",\"-45\",\"-30\",\"-15\",\"0\",\"15\","
           "\"30\",\"45\",\"60\",\"75\",\"90\"],  removefromanswer: "
           "[\"_button\",\"button_\",\"button\",\"b_\",\"answer_\",\"_answer\","
           "\"answer\"] },line: {show: false }, matrix: { show: true },polar: "
           "{ show: true, mindegree: -90, maxdegree: 90 }, text: { show: true "
           "} } );"
        << "JSON.stringify( plots.text.data );"
        << "[{\"matrix\":{\"correctpercentage\":\"<tr><tdcolspan=\\\"13\\\">"
           "Totalpercentagecorrect:5.5556%</td></"
           "tr>\",\"correctpercentages\":\"<tr><thcolspan=\\\"13\\\">Correct%"
           "perstimulus</th></tr><tr><th>-90</th><th>-75</th><th>-60</"
           "th><th>-45</th><th>-30</th><th>-15</th><th>0</th><th>15</"
           "th><th>30</th><th>45</th><th>60</th><th>75</th><th>90</th></"
           "tr><tr><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</td><td>33.33%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</"
           "td><tdclass=\\\"lightcell\\\">0.00%</td></"
           "tr>\",\"matrixtable\":\"<tableclass=\\\"stripedtable\\\"><tbody><"
           "tr><tr><tr><tr><tr><tr><tr><tr><tr><tr><th></th><th>-90</"
           "th><th>-75</th><th>-60</th><th>-45</th><th>-30</th><th>-15</"
           "th><th>0</th><th>15</th><th>30</th><th>45</th><th>60</th><th>75</"
           "th><th>90</th></tr><th>-90</th><tdclass=\\\"lightcell\\\">0</"
           "td><td>1</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td></tr><th>-60</th><td>1</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><td>1</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td></tr><th>-30</"
           "th><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><td>2</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td></tr><th>-15</"
           "th><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><td>1</td><tdclass=\\\"lightcell\\\">0</td><td>1</"
           "td><tdclass=\\\"lightcell\\\">0</td><td>1</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td></tr><th>15</th><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><td>1</td><td>1</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td></tr><th>30</"
           "th><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><td>1</td></tr><th>45</th><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><td>1</td><td>1</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><td>1</td></tr><th>60</th><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><td>1</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td></tr><th>75</th><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><tdclass=\\\"lightcell\\\">0</"
           "td><tdclass=\\\"lightcell\\\">0</td><td>2</"
           "td><tdclass=\\\"lightcell\\\">0</td><td>1</td></tr></tbody></"
           "table>\",\"answertable\":\"<tableclass=\\\"stripedtable\\\"><tr><"
           "th>Trial</th><th>Stimulus</th><th>Answer</th><th>Correct</th></"
           "tr><tr><td>1</td><td>-75</td><td>-90</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>2</td><td>30</td><td>15</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>3</td><td>-60</td><td>-15</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>4</td><td>15</td><td>-60</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>5</td><td>-45</td><td>-30</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>6</td><td>15</td><td>15</"
           "td><td><spanclass=\\\"correct\\\">Correct</span></td></"
           "tr><tr><td>7</td><td>-45</td><td>-30</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>8</td><td>30</td><td>45</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>9</td><td>60</td><td>75</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>10</td><td>0</td><td>-15</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>11</td><td>15</td><td>45</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>12</td><td>90</td><td>45</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>13</td><td>45</td><td>60</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>14</td><td>60</td><td>75</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>15</td><td>-30</td><td>-15</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>16</td><td>90</td><td>30</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>17</td><td>-90</td><td>-60</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></"
           "tr><tr><td>18</td><td>90</td><td>75</"
           "td><td><spanclass=\\\"incorrect\\\">Incorrect</span></td></tr></"
           "table>\"}}]";
}

void ApexMainTest::testResultViewer()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    // First, create the result parameters
    data::ResultParameters rvparam;
    rvparam.setShowResultsAfter(true);
    rvparam.setSaveResults(false);

    // Fetch the filenames:
    QFETCH(QString, resultFilePath);
    QFETCH(QString, resultViewerPath);
    QFETCH(QString, plotConfig);
    QFETCH(QString, testCommand);
    QFETCH(QString, plotDataJSON);

    QFileInfo resultFileInfo(resultFilePath);
    QString resultFile = resultFileInfo.absoluteFilePath();
    // To get the right path on all operating systems.

    // Read results file
    QFile resultsFile(resultFilePath);
    resultsFile.open(QIODevice::ReadOnly);
    QByteArray resultsFileData = resultsFile.readAll();
    resultsFile.close();

    QUrl ResultViewerUrl(QUrl::fromUserInput(resultViewerPath));
    apex::RTResultSink resultSink(ResultViewerUrl, rvparam.resultParameters(),
                                  rvparam.extraScript());

    // load page, execute javascript... (happens in separate thread)
    QSignalSpy spy(&resultSink, SIGNAL(loadingFinished(bool)));
    while (spy.count() == 0) {
        QCoreApplication::processEvents();
        QTest::qWait(10);
    }

    resultSink.evaluateJavascript(plotConfig);
    resultSink.newResults(resultsFileData);
    resultSink.plot();
    QCoreApplication::processEvents();

    QString resultJSON = resultSink.evaluateJavascript(testCommand).toString();

    resultJSON = resultJSON.simplified().replace(" ", "");
    plotDataJSON = plotDataJSON.simplified().replace(" ", "");

    QCOMPARE(resultJSON, plotDataJSON);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

#if !defined(Q_OS_ANDROID)
static QUrl createUrl(const QString &filename)
{
    return QDir::current().relativeFilePath(Paths::searchFile(
        QL1S("tests/libapex/") + filename, Paths::dataDirectories()));
}
#endif

void ApexMainTest::wait()
{
    for (int i = 0; i < 5; ++i) {
        QCoreApplication::processEvents();
        QTest::qSleep(100);
    }
}

QPair<QWebFrame *, AccessManager *> ApexMainTest::initAccesManager()
{
#if !defined(Q_OS_ANDROID)
    networkError = false;
    QWebView *webView = new QWebView();
    QWebPage *page = new QWebPage(webView);
    AccessManager *am = new AccessManager(page);
    page->setNetworkAccessManager(am);
    LoadChecker *loadChecker = new LoadChecker(this);
    connect(page->networkAccessManager(), SIGNAL(finished(QNetworkReply *)),
            loadChecker, SLOT(check(QNetworkReply *)));

    return QPair<QWebFrame *, AccessManager *>(page->mainFrame(), am);
#else
    return QPair<QWebFrame *, AccessManager *>(NULL, NULL);
#endif
}

// Load a local file that loads another local file
void ApexMainTest::testAccessManagerLoadLocal()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page = createUrl("test_local.html");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, false);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

// Load a local file that loads another local file
void ApexMainTest::testAccessManagerLoadLocalWithScheme()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page = createUrl("test_local.html");
    p_page.setScheme("file");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, false);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testAccessManagerLoadLocalAndApex()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page = createUrl("test_global.html");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, false);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testAccessManagerLoadLocalAndApexWithScheme()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page = createUrl("test_global.html");
    p_page.setScheme("file");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, false);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testAccessManagerLoadApex()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page("apex:resultsviewer.html");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, false);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testAccessManagerLoadApexAndLocal()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page("apex:resultsviewer-config.js");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, false);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testAccessManagerLoadUnknownLocal()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page("unk.html");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, true);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testAccessManagerLoadUnknownApex()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QUrl p_page("apex:unk.html");
    QPair<QWebFrame *, AccessManager *> pair = initAccesManager();

    pair.first->load(pair.second->prepare(p_page));
    wait();

    QCOMPARE(networkError, true);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

struct MocWarning : public InteractiveParameters::Callback {
    MocWarning() : InteractiveParameters::Callback(), count(0)
    {
    }

    void warning(const QString &)
    {
        ++count;
    }

    int count;
};

void ApexMainTest::testInteractive()
{
    TEST_EXCEPTIONS_TRY

    QString filename = Paths::searchFile(QL1S("tests/libapex/setgain.apx"),
                                         Paths::dataDirectories());
    ExperimentParser expParser(filename);
    QDomDocument document = expParser.loadAndUpgradeDom();

    QString expectedPresentations = "80";
    QString expectedGain = "15";
    QString expectedFeedbackLength = "5";
    QString expectedText1 = "Text1";
    QString expectedText2 = "Text2";

    QStringList entryResults;
    entryResults << "Sub" << expectedGain << expectedPresentations << "invalid"
                 << "" << expectedFeedbackLength << expectedText1
                 << expectedText2;

    InteractiveParameters params(document);
    MocWarning *mocWarning = new MocWarning;
    params.apply(entryResults, mocWarning);
    document = params.document();

    QDomElement apex = document.firstChildElement(QSL("apex:apex"));
    QDomElement procedure = apex.firstChildElement(QSL("procedure"));
    QDomElement parameters = procedure.firstChildElement(QSL("parameters"));
    QDomElement presentations =
        parameters.firstChildElement(QSL("presentations"));

    QDomElement devices = apex.firstChildElement(QSL("devices"));
    QDomElement device = devices.firstChildElement(QSL("device"));
    QDomElement gain = device.firstChildElement(QSL("gain"));

    QDomElement screens = apex.firstChildElement(QSL("screens"));
    QDomElement reinforcement = screens.firstChildElement(QSL("reinforcement"));
    QDomElement feedback = reinforcement.firstChildElement(QSL("feedback"));

    QDomElement screen = screens.firstChildElement(QSL("screen"));
    QDomElement gridlayout = screen.firstChildElement(QSL("gridLayout"));
    QDomElement button1 = gridlayout.firstChildElement(QSL("button"));
    QDomElement button2 = button1.nextSiblingElement();
    QDomElement text1 = button1.firstChildElement(QSL("text"));
    QDomElement text2 = button2.firstChildElement(QSL("text"));

    QString actualPresentations = presentations.text();
    QString actualGain = gain.text();
    QString actualFeedbackLength = feedback.attribute(QSL("length"));
    QString actualText1 = text1.text();
    QString actualText2 = text2.text();

    QCOMPARE(actualPresentations, expectedPresentations);
    QCOMPARE(actualGain, expectedGain);
    QCOMPARE(actualFeedbackLength, expectedFeedbackLength);
    QCOMPARE(actualText1, expectedText1);
    QCOMPARE(actualText2, expectedText2);
    QCOMPARE(mocWarning->count, 1);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testInteractiveConstraints()
{
    TEST_EXCEPTIONS_TRY

    QString filename = Paths::searchFile(QL1S("tests/libapex/subject.apx"),
                                         Paths::dataDirectories());
    InteractiveParameters interactiveParameters(
        ExperimentParser(filename).loadAndUpgradeDom());
    QStringList invalidEntry(QSL("invalid"));
    QStringList validEntry(QSL("A01B01C"));
    MocWarning mocWarning;

    QVERIFY(interactiveParameters.apply(validEntry, &mocWarning));
    QCOMPARE(mocWarning.count, 0);

    QVERIFY(!interactiveParameters.apply(invalidEntry, &mocWarning));
    QCOMPARE(mocWarning.count, 1);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testInteractiveAndExpressions()
{
    TEST_EXCEPTIONS_TRY

    QString testFile = Paths::searchFile(QSL("tests/libapex/subject.apx"),
                                         Paths::dataDirectories());
    QMap<QString, QString> expressions;
    expressions.insert(QSL("apex:apex/results[1]/subject[1]"), QSL("a01a01a"));
    QStringList entryResults(QSL("a01a01b"));
    MocWarning mocWarning;

    InteractiveParameters interactiveParameters(
        ExperimentParser(testFile).loadAndUpgradeDom());
    QCOMPARE(interactiveParameters.entries().size(), 1);
    ParameterDialogResults expressionResults =
        interactiveParameters.applyExpressions(expressions);
    QCOMPARE(interactiveParameters.entries().size(), 0);

    ParameterDialogResults &dialogResults = *interactiveParameters.results();
    QVERIFY(interactiveParameters.apply(entryResults, &mocWarning));

    QCOMPARE(mocWarning.count, 0);

    /* Expressions should take precedence since it came first */
    QCOMPARE(dialogResults.size(), 0);
    QCOMPARE(expressionResults.size(), 1);
    QCOMPARE(expressionResults[0].newValue(), QSL("a01a01a"));

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testSoundLevelMeter()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QString dummy = QL1S("dummyslmslave");
    QList<SoundLevelMeterPluginCreator *> available =
        PluginLoader().availablePlugins<SoundLevelMeterPluginCreator>();
    QStringList plugins;
    Q_FOREACH (SoundLevelMeterPluginCreator *creator, available)
        plugins.append(creator->availablePlugins());
    QVERIFY(plugins.contains(dummy));
    SoundLevelMeterPluginCreator *creator =
        createPluginCreator<SoundLevelMeterPluginCreator>(dummy);
    QVERIFY(creator != NULL);
    try {
        QScopedPointer<SoundLevelMeter> slm(
            creator->createSoundLevelMeter(dummy));
        QVERIFY(!slm.isNull());
        QCOMPARE(slm->result(), 64.9);
    } catch (const std::exception &e) {
        QFAIL(e.what());
    }

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

QDomDocument ApexMainTest::readXmlResults(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isOpen())
        qCDebug(APEX_RS) << "Result file could not be opened: " << fileName;
    QDomDocument document;
    document.setContent(file.readAll());
    return document;
}

QString ApexMainTest::compareXml(const QDomNode &actual,
                                 const QDomNode &expected)
{
    // node type
    if (actual.nodeType() != expected.nodeType())
        return QString::fromLatin1("Different node type: %1!=%2")
            .arg(actual.nodeType())
            .arg(expected.nodeType());

    // node name
    if (actual.nodeName() != expected.nodeName())
        return QString::fromLatin1("Different node name: %1!=%2")
            .arg(actual.nodeName(), expected.nodeName());

    // node value
    if (actual.nodeValue() != expected.nodeValue())
        return QString::fromLatin1("Different node value: %1!=%2")
            .arg(actual.nodeValue(), expected.nodeValue());

    // attributes
    QDomNamedNodeMap expectedAttributes = expected.attributes();
    QDomNamedNodeMap actualAttributes = actual.attributes();
    if (expectedAttributes.length() != actualAttributes.length())
        return QString::fromLatin1(
                   "Different number of attributes in %3: %1!=%2")
            .arg(actualAttributes.length())
            .arg(expectedAttributes.length())
            .arg(expected.nodeName());
    for (unsigned i = 0; i < unsigned(expectedAttributes.length()); ++i) {
        QDomNode expectedAttrItem = expectedAttributes.item(i);
        QDomNode actualAttrItem =
            actualAttributes.namedItem(expectedAttrItem.nodeName());
        QString result = compareXml(expectedAttrItem, actualAttrItem);
        if (!result.isEmpty())
            return result;
    }

    // child nodes
    QDomNodeList expectedNodes = expected.childNodes();
    QDomNodeList actualNodes = actual.childNodes();
    if (expectedNodes.length() != actualNodes.length())
        return QString::fromLatin1(
                   "Different number of child nodes in %3: %1!=%2")
            .arg(actualNodes.length())
            .arg(expectedNodes.length())
            .arg(expected.nodeName());
    for (unsigned i = 0; i < unsigned(expectedNodes.length()); ++i) {
        QDomNode expectedNode = expectedNodes.item(i);
        QDomNode actualNode = actualNodes.item(i);
        QString result = compareXml(expectedNode, actualNode);
        if (!result.isEmpty())
            return result;
    }

    return QString();
}

void ApexMainTest::testStandaloneUpgrader()
{
#if !defined(Q_OS_ANDROID)
    TEST_EXCEPTIONS_TRY

    QProcess upgraderProcess;

#ifdef Q_OS_WIN32
    QString processFile = QL1S("apexupgrader.exe");
#else
    QString processFile = QL1S("apexupgrader");
#endif
    processFile = Paths::searchFile(processFile, Paths::binDirectories());
    upgraderProcess.setProgram(processFile);

    // Set the right command line arguments:
    QStringList argumentsList;
    QString arg2 = Paths::searchFile(QL1S("tests/libapex/upgraderTest.apx"),
                                     Paths::dataDirectories());

    argumentsList.append(arg2);
    upgraderProcess.setArguments(argumentsList);

    upgraderProcess.setProcessChannelMode(
        QProcess::MergedChannels); // to redirect standard output
    upgraderProcess.start();

    // Wait untill process finished before assesing results:
    upgraderProcess.waitForFinished(); // default timeout 30s
    // qCDebug(APEX_RS) << upgraderProcess.readAllStandardOutput();

    // Now read the contents of the tranformed file, and compare with desired
    // file.
    // Transformed:
    QFile upgradeResultFile(arg2 + QSL(".upgraded"));
    QVERIFY(
        upgradeResultFile.exists()); // Otherwise conversion failed by default.
    QVERIFY(upgradeResultFile.open(QIODevice::ReadOnly));

    // qCDebug(APEX_RS) << "Upgrade resultfile: "<<upgradeResultFile.fileName();

    // Check if the version of the written file matches the current version:
    QDomDocument doc;
    doc.setContent(&upgradeResultFile);
    const QString sVersion = doc.documentElement().attribute("xmlns:apex");

    // qCDebug(APEX_RS) << "Found version string: "<<sVersion;

    QVector<int> upgradedVersion;
    upgradedVersion.resize(3);
    QRegExp re("http://med.kuleuven.be/exporl/apex/(\\d+)\\.(\\d+)\\.?(\\d*)/");
    if (re.lastIndexIn(sVersion) != -1) {
        upgradedVersion[0] = re.capturedTexts()[1].toInt();
        upgradedVersion[1] = re.capturedTexts()[2].toInt();
        if (re.captureCount() == 2)
            upgradedVersion[2] = 0;
        else
            upgradedVersion[2] = re.capturedTexts()[3].toInt();
    }
    /*qCDebug(APEX_RS) << "Upgraded version:
       "<<upgradedVersion.at(0)<<"."<<upgradedVersion.at(1)
                <<"."<<upgradedVersion.at(2);*/

    // get the current version:
    QString currentVersionString = QString(APEX_SCHEMA_VERSION);
    QVector<int> currentVersion;
    currentVersion.resize(3);
    currentVersion[0] = currentVersionString.section(".", 0, 0).toInt();
    currentVersion[1] = currentVersionString.section(".", 1, 1).toInt();
    currentVersion[2] = currentVersionString.section(".", 2, 2).toInt();

    /*qCDebug(APEX_RS) << "Current version:
       "<<currentVersion.at(0)<<"."<<currentVersion.at(1)
                <<"."<<currentVersion.at(2);*/

    QCOMPARE(upgradedVersion, currentVersion);

    // Remove the newly created file so the test can be done again...
    upgradeResultFile.remove();

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testSoundcardSettings()
{
    TEST_EXCEPTIONS_TRY

    SoundcardSettings soundcardSettings;

    soundcardSettings.removeSavedSettings();
    QCOMPARE(soundcardSettings.hasData(), false);

    QString device("SoundCard");
    QString hostApi("MyHostAPI");
    soundcardSettings.saveDevice(hostApi, device);
    QCOMPARE(soundcardSettings.device(), device);
    QCOMPARE(soundcardSettings.hostApi(), hostApi);
    QCOMPARE(soundcardSettings.hasData(), true);

    soundcardSettings.removeSavedSettings();

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testSoundcardsDialog()
{
    TEST_EXCEPTIONS_TRY

    // simply test for segfaults etc
    SoundcardSettings scs;
    SoundcardsDialog d;
    d.displayHostApis();
    if (scs.hasData())
        d.setSelection(scs.hostApi(), scs.device());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testManagedDirectory()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryDir dir1;

    QVERIFY(dir1.isValid());
    ManagedDirectory managedDir(dir1.path());
    managedDir.init();
    managedDir.setAuthor(QSL("dummy"), QSL("dummy@dummy.com"));
    QVERIFY(ManagedDirectory::exists(dir1.path()));

    QFile newFile(dir1.path() + QL1S("/newFile"));
    QVERIFY(newFile.open(QIODevice::ReadWrite));
    newFile.write(QL1S("someData\n").data());
    newFile.close();
    managedDir.add(newFile.fileName());

    QTemporaryDir dir2;
    ManagedDirectory managedDir2(dir2.path());
    managedDir2.init();
    managedDir2.setRemote(QSL("origin"), dir1.path());
    managedDir2.pull();
    QVERIFY(QFile::exists(dir2.path() + QL1S("/newFile")));

    QFile newFile2(dir1.path() + QL1S("/newFile2"));
    QVERIFY(newFile2.open(QIODevice::ReadWrite));
    newFile2.write(QL1S("someData\n").data());
    newFile2.close();
    managedDir.add(newFile2.fileName());
    managedDir2.pull();
    QVERIFY(QFile::exists(dir2.path() + QL1S("/newFile2")));

    managedDir.reClone();
    QVERIFY(managedDir.objects().contains(QSL("newFile")) &&
            managedDir.objects().contains(QSL("newFile2")));
    managedDir2.reClone();
    QVERIFY(managedDir2.objects().contains(QSL("newFile")) &&
            managedDir.objects().contains(QSL("newFile2")));

    QTemporaryDir dir3;
    ManagedDirectory managedDir3(dir3.path());
    QVERIFY_EXCEPTION_THROWN(managedDir3.open(), ApexException);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::cleanupTestCase()
{
}

QTEST_MAIN(ApexMainTest)
