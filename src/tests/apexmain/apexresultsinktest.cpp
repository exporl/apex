#include <QMessageBox>
#include <QtTest>

#include "apexmaintest.h"

#include "apexdata/calibration/calibrationdata.h"
#include "apexdata/connection/connectiondata.h"
#include "apexdata/datablock/datablocksdata.h"
#include "apexdata/device/devicesdata.h"
#include "apexdata/filter/filtersdata.h"
#include "apexdata/interactive/parameterdialogresults.h"
#include "apexdata/mainconfigfiledata.h"
#include "apexdata/parameters/generalparameters.h"
#include "apexdata/parameters/parametermanagerdata.h"
#include "apexdata/procedure/constantproceduredata.h"
#include "apexdata/result/resultparameters.h"
#include "apexdata/screen/screensdata.h"

#include "apexmain/resultsink/apexresultsink.h"
#include "apexmain/runner/experimentrundelegate.h"

#include "common/temporarydirectory.h"
#include "common/testutils.h"

using namespace apex;
using namespace apex::data;
using namespace cmn;

namespace apex
{

class ApexResultSinkForTesting : public ApexResultSink
{
public:
    ApexResultSinkForTesting(ExperimentRunDelegate &runDelegate)
        : ApexResultSink(runDelegate),
          informedSavingFailed(false),
          shouldDiscardResultsAnswer(true)
    {
    }

    const QString askSaveLocation(const QString &suggestion) override
    {
        return saveLocationAnswers.isEmpty() ? suggestion
                                             : saveLocationAnswers.takeFirst();
    }

    bool askShouldDiscardResults() override
    {
        return shouldDiscardResultsAnswer;
    }

    bool save(const QString &filename) override
    {
        return saveCallAnswers.isEmpty() ? ApexResultSink::save(filename)
                                         : saveCallAnswers.takeFirst();
    }

    void informSavingFailed(const QString &path) override
    {
        Q_UNUSED(path)
        informedSavingFailed = true;
    }

    void addSaveLocation(const QString &path)
    {
        saveLocationAnswers.append(path);
    }

    void discardResults()
    {
        shouldDiscardResultsAnswer = true;
    }

    void dontDiscardResults()
    {
        shouldDiscardResultsAnswer = false;
    }

    void addSaveSuccess()
    {
        saveCallAnswers.append(true);
    }

    void addSaveFailure()
    {
        saveCallAnswers.append(false);
    }

    bool informedSavingFailed;

private:
    QStringList saveLocationAnswers;
    QList<bool> saveCallAnswers;
    bool shouldDiscardResultsAnswer;
};
};

void ApexMainTest::
    testApexResultSink_resultfileContainsResultscriptAndResultparameters()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;
    QString resultfilePath = tempDir.addFile("resultfile.apr");

    ExperimentData experimentData = createExperimentData();
    experimentData.generalParameters()->setAutoSave(true);
    experimentData.resultParameters()->setExtraScript("a_resultscript();");
    experimentData.resultParameters()->setResultParameter("name", "value");

    ExperimentRunDelegate runDelegate(experimentData, MainConfigFileData(),
                                      nullptr);

    ApexResultSink resultSink(runDelegate);
    resultSink.setFilename(resultfilePath);

    resultSink.saveResultfile();

    QString resultfileContent = readFileAsString(resultfilePath);
    QVERIFY(resultfileContent.contains(
        "<parameter name=\"name\">value</parameter>"));
    QVERIFY(resultfileContent.contains(
        "<resultscript>a_resultscript();</resultscript>"));

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testApexResultSink_resultfileContainsExperimentStartTime()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;
    QString resultfilePath = tempDir.addFile("resultfile.apr");

    ExperimentData experimentData = createExperimentData();
    experimentData.generalParameters()->setAutoSave(true);

    ExperimentRunDelegate runDelegate(experimentData, MainConfigFileData(),
                                      nullptr);

    QDateTime experimentStartTime =
        QDateTime::fromString("2012-11-10T12:13:14", Qt::ISODate);
    ApexResultSink resultSink(runDelegate);
    resultSink.setExperimentStartTime(experimentStartTime);
    resultSink.setFilename(resultfilePath);

    resultSink.saveResultfile();

    QString resultfileContent = readFileAsString(resultfilePath);
    QVERIFY(resultfileContent.contains(
        "<startdate>2012-11-10T12:13:14</startdate>"));

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::
    testApexResultSink_byDefaultResultfileIsSavedNextToExperimentFile()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;
    QString expectedPath = tempDir.addFile("experiment-results.apr");

    ExperimentData experimentData = createExperimentData();
    experimentData.setFileName(tempDir.file("experiment.apx"));

    ExperimentRunDelegate runDelegate(experimentData, MainConfigFileData(),
                                      nullptr);

    ApexResultSinkForTesting resultSink(runDelegate);

    QString actualPath = resultSink.saveResultfile();

    QVERIFY(actualPath.startsWith(tempDir.file("experiment-results")));
    tempDir.addFile(QFileInfo(actualPath).fileName());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::
    testApexResultSink_locationOfResultfileIsReturnedAndResultfileIsLocatedThere()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;
    QString expectedPath = tempDir.addFile("results.apr");

    ExperimentData experimentData = createExperimentData();
    experimentData.setFileName(tempDir.file("experiment.apx"));

    ExperimentRunDelegate runDelegate(experimentData, MainConfigFileData(),
                                      nullptr);

    ApexResultSinkForTesting resultSink(runDelegate);
    resultSink.addSaveLocation(expectedPath);

    QString actualPath = resultSink.saveResultfile();

    QCOMPARE(actualPath, expectedPath);
    QVERIFY(QFile::exists(actualPath));

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::
    testApexResultSink_canSaveResultfileAfterCancelSavingResults()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;
    QString expectedPath = tempDir.addFile("results.apr");

    ExperimentData experimentData = createExperimentData();
    experimentData.setFileName(tempDir.file("experiment.apx"));

    ExperimentRunDelegate runDelegate(experimentData, MainConfigFileData(),
                                      nullptr);

    ApexResultSinkForTesting resultSink(runDelegate);
    resultSink.addSaveLocation(QString());
    resultSink.dontDiscardResults();
    resultSink.addSaveLocation(expectedPath);

    QString actualPath = resultSink.saveResultfile();

    QCOMPARE(actualPath, expectedPath);

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::
    testApexResultSink_canDiscardResultsAfterCancelSavingResults()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;

    ExperimentData experimentData = createExperimentData();
    experimentData.setFileName(tempDir.file("experiment.apx"));

    ExperimentRunDelegate runDelegate(experimentData, MainConfigFileData(),
                                      nullptr);

    ApexResultSinkForTesting resultSink(runDelegate);
    resultSink.addSaveLocation(QString());
    resultSink.discardResults();

    QString actualPath = resultSink.saveResultfile();

    QVERIFY(actualPath.isEmpty());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::
    testApexResultSink_canSelectAnotherSaveLocationAfterSavingFails()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;
    QString expectedPath = tempDir.addFile("expected-results.apr");

    ExperimentData experimentData = createExperimentData();
    experimentData.setFileName(tempDir.file("experiment.apx"));
    experimentData.generalParameters()->setAutoSave(true);

    ExperimentRunDelegate runDelegate(experimentData, MainConfigFileData(),
                                      nullptr);

    ApexResultSinkForTesting resultSink(runDelegate);
    resultSink.addSaveFailure();
    resultSink.addSaveLocation(expectedPath);
    resultSink.addSaveSuccess();

    QString actualPath = resultSink.saveResultfile();

    QCOMPARE(actualPath, expectedPath);
    QVERIFY(resultSink.informedSavingFailed);

    TEST_EXCEPTIONS_CATCH
}

ExperimentData ApexMainTest::createExperimentData()
{
    return ExperimentData(
        "config.apx", new ScreensData, new ConstantProcedureData,
        new ConnectionsData, new CalibrationData, new GeneralParameters,
        new ResultParameters(), new ParameterDialogResults,
        new QMap<QString, RandomGeneratorParameters *>(), new DevicesData,
        new FiltersData, new DevicesData, new DatablocksData, new StimuliData,
        "some-description", new ParameterManagerData);
}
