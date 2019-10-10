#include "apexmaintest.h"

#include "apexmain/resultsink/resultfilepathcreator.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

using namespace apex;
using namespace cmn;

void ApexMainTest::
    testResultfilePathCreator_defaultAbsolutePath_nextToExperimentFileWithSameBaseName()
{
    TEST_EXCEPTIONS_TRY

    QString experimentfilePath("/path/to/experiment.apx");

    QString actualPath = ResultfilePathCreator().createDefaultAbsolutePath(
        experimentfilePath, QString());

    QVERIFY(actualPath.startsWith("/path/to/experiment-results"));

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::
    testResultfilePathCreator_defaultAbsolutePath_containsSubjectIfAvailable()
{
    TEST_EXCEPTIONS_TRY

    QString experimentfilePath("/path/to/experiment.apx");

    QString actualPath = ResultfilePathCreator().createDefaultAbsolutePath(
        experimentfilePath, "subject");

    QVERIFY(actualPath.startsWith("/path/to/experiment-subject"));

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testResultfilePathCreator_timestampIsAddedToMakeItUnique()
{
    TEST_EXCEPTIONS_TRY

    QString experimentfilePath("/path/to/experiment.apx");

    QString actualPath = ResultfilePathCreator().createDefaultAbsolutePath(
        experimentfilePath, QString());

    QVERIFY(QRegularExpression("/path/to/"
                               "experiment-results-\\d{4}\\d{2}\\d{2}T\\d{2}"
                               "\\d{2}\\d{2}.apr")
                .match(actualPath)
                .hasMatch());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testResultfilePathCreator_resultfileHasAprExtension()
{
    TEST_EXCEPTIONS_TRY

    QString experimentfilePath("/path/to/experiment.apx");

    QString actualPath = ResultfilePathCreator().createDefaultAbsolutePath(
        experimentfilePath, QString());

    QVERIFY(actualPath.endsWith(".apr"));

    TEST_EXCEPTIONS_CATCH
}
