/******************************************************************************
 * Copyright (C) 2018  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexmaintest.h"

#include "apexmain/experimentparser.h"
#include "apexmain/resultsink/resultviewer.h"
#include "apexmain/resultsink/rtresultsink.h"

#include "apexdata/experimentdata.h"
#include "apexdata/result/resultparameters.h"

#include "common/exception.h"
#include "common/paths.h"
#include "common/testutils.h"

#include <QFile>

using namespace apex;
using namespace apex::data;
using namespace cmn;

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
#if defined(Q_OS_ANDROID)
    QSKIP("Skipped on Android");
#elif defined(WITH_WEBENGINE)
    QSKIP("Skipped with QWebEngine");
#else
    TEST_EXCEPTIONS_TRY

    // Fetch the filenames:
    QFETCH(QString, resultFilePath);
    QFETCH(QString, testFilePath);
    QFETCH(QString, desiredFilePath);

    QFileInfo resultFileInfo(resultFilePath);
    QString resultFile =
        resultFileInfo.absoluteFilePath(); // To get the right path on all
    // operating systems.

    apex::ResultViewer testViewer(resultFile);

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
        << Paths::searchFile(QL1S("tests/libapex/results-test-line.apr"),
                             Paths::dataDirectories())
        << Paths::searchFile(QL1S("resultsviewer/resultsviewer.html"),
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
           "tdclass=\\\"dataname\\\"><td>-0.6667(&plusmn;2.1344)</td></"
           "tr>\",\"meantrials\":\"<tr><tdclass=\\\"dataname\\\">Mean(std)<br>"
           "last6trials</tdclass=\\\"dataname\\\"><td>0.667(&plusmn1.374)</"
           "td></"
           "tr>\",\"linedatatable\":\"<tableclass=\\\"datatable\\\"><tr><td>"
           "Parametervalues</td><td>-5,-3,-1,-3,-1,-3,-1,1,3,1,-1,1,-1</td></"
           "tr><tr><td>Lastvalue</td><td>-1</td></tr><tr><td>Reversals</"
           "td><td>-1,-3,-1,-3,3,-1,1</td></"
           "tr><tr><tdclass=\\\"dataname\\\">Mean(std)<br>last6reversals</"
           "tdclass=\\\"dataname\\\"><td>-0.6667(&plusmn;2.1344)</td></"
           "tr><tr><tdclass=\\\"dataname\\\">Mean(std)<br>last6trials</"
           "tdclass=\\\"dataname\\\"><td>0.667(&plusmn1.374)</td></tr></"
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
        << Paths::searchFile(QL1S("tests/libapex/results-test-matrixpolar.apr"),
                             Paths::dataDirectories())
        << Paths::searchFile(QL1S("resultsviewer/resultsviewer.html"),
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
        << Paths::searchFile(QL1S("tests/libapex/results-test-matrixpolar.apr"),
                             Paths::dataDirectories())
        << Paths::searchFile(QL1S("resultsviewer/resultsviewer.html"),
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

    QTest::newRow("custom")
        << Paths::searchFile(QL1S("tests/libapex/results-test-line.apr"),
                             Paths::dataDirectories())
        << Paths::searchFile(QL1S("tests/libapex/resultsviewer-custom.html"),
                             Paths::dataDirectories())
        << "config = {};"
        << "customCommand();"
        << "custom output";
}

void ApexMainTest::testResultViewer()
{
#if defined(Q_OS_ANDROID)
    QSKIP("Skipped on Android");
#elif defined(WITH_WEBENGINE)
    QSKIP("Skipped with QWebEngine");
#else
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, resultFilePath);
    QFETCH(QString, resultViewerPath);
    QFETCH(QString, plotConfig);
    QFETCH(QString, testCommand);
    QFETCH(QString, plotDataJSON);

    QUrl ResultViewerUrl = QUrl::fromUserInput(resultViewerPath);
    ResultParameters parameters;

    apex::RTResultSink resultSink(ResultViewerUrl,
                                  parameters.resultParameters(),
                                  parameters.extraScript());

    QSignalSpy(&resultSink, SIGNAL(loadingFinished(bool))).wait();

    resultSink.runJavaScript(plotConfig);
    resultSink.newResults(readFileAsString(resultFilePath));
    resultSink.plot();

    QString resultJSON = resultSink.runJavaScript(testCommand);

    QCOMPARE(resultJSON.replace(" ", ""), plotDataJSON.replace(" ", ""));

    TEST_EXCEPTIONS_CATCH
#endif
}

void ApexMainTest::testResultViewerExtraScriptIsInjected()
{
#if defined(Q_OS_ANDROID)
    QSKIP("Skipped on Android");
#elif defined(WITH_WEBENGINE)
    QSKIP("Skipped with QWebEngine");
#else
    TEST_EXCEPTIONS_TRY

    QString resultfilePath = Paths::searchFile(
        QL1S("tests/libapex/results-test-with-extra-script.apr"),
        Paths::dataDirectories());

    QDir::setCurrent(QFileInfo(resultfilePath).absolutePath());
    ResultViewer resultViewer(resultfilePath);

    RTResultSink *resultSink = resultViewer.getResultSink();

    QSignalSpy(resultSink, SIGNAL(loadingFinished(bool))).wait();

    QString actualTestCommandResult = resultSink->runJavaScript("extraScript");

    QCOMPARE(actualTestCommandResult, QString("expected value"));

    TEST_EXCEPTIONS_CATCH
#endif
}

void ApexMainTest::testResultViewerResultParametersAreInjected()
{
#if defined(Q_OS_ANDROID)
    QSKIP("Skipped on Android");
#elif defined(WITH_WEBENGINE)
    QSKIP("Skipped with QWebEngine");
#else
    TEST_EXCEPTIONS_TRY

    QString resultfilePath = Paths::searchFile(
        QL1S("tests/libapex/results-test-with-result-parameters.apr"),
        Paths::dataDirectories());

    QDir::setCurrent(QFileInfo(resultfilePath).absolutePath());
    ResultViewer resultViewer(resultfilePath);

    RTResultSink *resultSink = resultViewer.getResultSink();

    QSignalSpy(resultSink, SIGNAL(loadingFinished(bool))).wait();

    QString actualTestCommandResult = resultSink->runJavaScript("params.name");

    QCOMPARE(actualTestCommandResult, QString("expected value"));

    TEST_EXCEPTIONS_CATCH
#endif
}

void ApexMainTest::testResultViewerCannotBeConstructedWithoutResultFile()
{
    TEST_EXCEPTIONS_TRY

    QVERIFY_EXCEPTION_THROWN(ResultViewer("/path/to/unexisting/resultfile.apr"),
                             Exception);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testResultViewerResultFileWithoutResultPageCanBeLoaded()
{
#if defined(Q_OS_ANDROID)
    QSKIP("Skipped on Android");
#elif defined(WITH_WEBENGINE)
    QSKIP("Skipped with QWebEngine");
#else
    TEST_EXCEPTIONS_TRY

    QString resultfilePath = Paths::searchFile(
        QL1S("tests/libapex/results-test-without-result-page.apr"),
        Paths::dataDirectories());

    ResultViewer resultViewer(resultfilePath);

    RTResultSink *resultSink = resultViewer.getResultSink();

    QSignalSpy(resultSink, SIGNAL(loadingFinished(bool))).wait();

    QString actualTestCommandResult =
        resultSink->runJavaScript("'expected value'");

    QCOMPARE(actualTestCommandResult, QString("expected value"));

    TEST_EXCEPTIONS_CATCH
#endif
}

void ApexMainTest::testResultViewerExtraScriptLoadedBeforeAnswersAreAdded()
{
#if defined(Q_OS_ANDROID)
    QSKIP("Skipped on Android");
#elif defined(WITH_WEBENGINE)
    QSKIP("Skipped with QWebEngine");
#else
    TEST_EXCEPTIONS_TRY

    QString resultfilePath =
        Paths::searchFile(QL1S("tests/libapex/results-test-with-trials.apr"),
                          Paths::dataDirectories());

    QDir::setCurrent(QFileInfo(resultfilePath).absolutePath());
    ResultViewer resultViewer(resultfilePath);

    RTResultSink *resultSink = resultViewer.getResultSink();

    QSignalSpy(resultSink, SIGNAL(loadingFinished(bool))).wait();

    QString actualTestCommandResult =
        resultSink->runJavaScript("document.body.innerHTML");

    QCOMPARE(actualTestCommandResult.trimmed(),
             QString("<trial>trial-1</trial>, <trial>trial-2</trial>"));

    TEST_EXCEPTIONS_CATCH
#endif
}

/*
void ApexMainTest::testResultsProcessor()
{
    TODO
}
*/