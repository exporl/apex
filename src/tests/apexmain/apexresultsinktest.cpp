#include <QtTest>

#include "apexmaintest.h"

#include "apexdata/calibration/calibrationdata.h"
#include "apexdata/connection/connectiondata.h"
#include "apexdata/datablock/datablocksdata.h"
#include "apexdata/device/devicesdata.h"
#include "apexdata/experimentdata.h"
#include "apexdata/filter/filtersdata.h"
#include "apexdata/interactive/parameterdialogresults.h"
#include "apexdata/mainconfigfiledata.h"
#include "apexdata/parameters/generalparameters.h"
#include "apexdata/parameters/parametermanagerdata.h"
#include "apexdata/procedure/constantproceduredata.h"
#include "apexdata/procedure/proceduredata.h"
#include "apexdata/result/resultparameters.h"
#include "apexdata/screen/screensdata.h"

#include "apexmain/apexcontrol.h"
#include "apexmain/resultsink/apexresultsink.h"
#include "apexmain/runner/experimentrundelegate.h"

#include "common/temporarydirectory.h"
#include "common/testutils.h"

using namespace apex;
using namespace apex::data;
using namespace cmn;

void ApexMainTest::testApexResultSink_resultscriptAndResultparameters()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory tempDir;
    QString resultfilePath = tempDir.addFile("resultfile.apr");

    ExperimentData experimentData = createExperimentData();
    experimentData.resultParameters()->setExtraScript("a_resultscript();");
    experimentData.resultParameters()->setResultParameter("name", "value");

    ExperimentRunDelegate experimentRunDelegate(experimentData,
                                                MainConfigFileData(), NULL);

    ApexResultSink apexResultSink(experimentRunDelegate,
                                  QDateTime::currentDateTime());
    apexResultSink.SetFilename(resultfilePath);

    apexResultSink.Finished(false);

    QString resultfileContent = readFileAsString(resultfilePath);
    QVERIFY(resultfileContent.contains(
        "<parameter name=\"name\">value</parameter>"));
    QVERIFY(resultfileContent.contains(
        "<resultscript>a_resultscript();</resultscript>"));

    TEST_EXCEPTIONS_CATCH
}

ExperimentData ApexMainTest::createExperimentData()
{
    return ExperimentData(
        "config.apx", new ScreensData, new ConstantProcedureData,
        new ConnectionsData, new CalibrationData, new GeneralParameters(),
        new ResultParameters(), new ParameterDialogResults,
        new QMap<QString, RandomGeneratorParameters *>(), new DevicesData,
        new FiltersData, new DevicesData, new DatablocksData, new StimuliData,
        "some-description", new ParameterManagerData);
}
