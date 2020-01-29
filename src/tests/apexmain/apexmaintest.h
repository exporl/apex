/******************************************************************************
 * Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _APEX_SRC_TESTS_APEXMAIN_APEXMAINTEST_H_
#define _APEX_SRC_TESTS_APEXMAIN_APEXMAINTEST_H_

#include "../testmacros.h"

#include <QDomDocument>
#include <QNetworkReply>
#include <QtTest>

#include "apexdata/experimentdata.h"

namespace apex
{
class AccessManager;
namespace data
{
class DatablockData;
class ConnectionData;
class CorrectorData;
}
}
class RandomGeneratorTestParameters;

/** Test main apex library (libapex) **/
class ApexMainTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void testAseq();
    void testCohDatablockAseq();
    void testAseqParser();
    void testCohDatablockAseq_data();
    void testCohDatablockAseqMapping();
    void testCohDatablockInvalidFile();
    void testCohDatablock_invalid();
    void testCohInvalidCL();
    void testRandomInterval();
    void testUniformInt();
    void testUniformDouble();
    void testAccessManagerToLocalFile();
    void testInteractive();
    void testInteractiveConstraints();
    void testInteractiveAndExpressions();
    void testSoundLevelMeter();

    void testMainConfigFileParser();
    void testUpgradeTo3_1_1();
    void testResultViewerIntegration();
    void testResultViewer_data();
    void testResultViewer();
    void testResultViewerExtraScriptIsInjected();
    void testResultViewerResultParametersAreInjected();
    void testResultViewerCannotBeConstructedWithoutResultFile();
    void testResultViewerExtraScriptLoadedBeforeAnswersAreAdded();
    void testStandaloneUpgrader();
    void testSoundcardsDialog();
    void testSoundcardSettings();

    void hrtfMwf();
    void hrtfMwf_data();
    void hrtfMwfAdaptive();
    void hrtfMwfAdaptive_data();
    void fileSink();
    void fileSink_data();

    void testNetworkDeviceController();

    void testStudyDialog();
    void testStudy();
    void testStudyManager();
    void testStudyFull();
    void testManagedDirectory();
    void testManagedDirectoryWithExternalWorkdir();
    void testStudy_makeResultfilePath_privateStudy();
    void
    testStudy_makeResultfilePath_privateStudy_noRelativeResultfilePathSpecified();
    void testStudy_makeResultfilePath_publicStudy();
    void
    testStudy_makeResultfilePath_publicStudy_noRelativeResultfilePathSpecified();
    void
    testResultfilePathCreator_defaultAbsolutePath_nextToExperimentFileWithSameBaseName();
    void
    testResultfilePathCreator_defaultAbsolutePath_containsSubjectIfAvailable();
    void testResultfilePathCreator_timestampIsAddedToMakeItUnique();
    void testResultfilePathCreator_resultfileHasAprExtension();

    void testApexResultSink_resultfileContainsResultscriptAndResultparameters();
    void testApexResultSink_resultfileContainsExperimentStartTime();
    void testApexResultSink_byDefaultResultfileIsSavedNextToExperimentFile();
    void
    testApexResultSink_locationOfResultfileIsReturnedAndResultfileIsLocatedThere();
    void testApexResultSink_canSaveResultfileAfterCancelSavingResults();
    void testApexResultSink_canDiscardResultsAfterCancelSavingResults();
    void testApexResultSink_canSelectAnotherSaveLocationAfterSavingFails();

    /* These tests instantiate ApexControl which doesn't destruct correctly,
     * run this last.
     */
    void testSshKeys();
    void testFlowRunner();
    void testRepeatButton();

    void cleanupTestCase();

private:
    template <typename T>
    void verifyInterval(RandomGeneratorTestParameters *params, T min, T max);
    static QDomDocument readXmlResults(const QString &fileName);
    static QString compareXml(const QDomNode &actual, const QDomNode &expected);

    apex::data::ExperimentData createExperimentData();
    const QString readFileAsString(const QString &filePath);
    void createFile(const QString &path,
                    const QString &content = QString("some-content"));
    void testAccessManagerToLocalFile(const QUrl &input,
                                      const QString &expected,
                                      const QString &baseApexPath = QString());
};

#endif
