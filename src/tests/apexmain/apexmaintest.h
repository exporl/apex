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

class QWebFrame;

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
    void testAccessManagerLoadLocal();
    void testAccessManagerLoadLocalWithScheme();
    void testAccessManagerLoadLocalAndApex();
    void testAccessManagerLoadLocalAndApexWithScheme();
    void testAccessManagerLoadApex();
    void testAccessManagerLoadApexAndLocal();
    void testAccessManagerLoadUnknownLocal();
    void testAccessManagerLoadUnknownApex();
    void testInteractive();
    void testInteractiveConstraints();
    void testInteractiveAndExpressions();
    void testSoundLevelMeter();

    void testMainConfigFileParser();
    void testUpgradeTo3_1_1();
    void testResultViewerConvertToCSV_data();
    void testResultViewerConvertToCSV();
    void testResultViewer_data();
    void testResultViewer();
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

    void testFlowRunner();

    /* This test instantiates ApexControl which doesn't destruct correctly,
     * run this last.
     */
    void testRepeatButton();

    void cleanupTestCase();

private:
    template <typename T>
    void verifyInterval(RandomGeneratorTestParameters *params, T min, T max);
    static QDomDocument readXmlResults(const QString &fileName);
    static QString compareXml(const QDomNode &actual, const QDomNode &expected);

    void wait();
    QPair<QWebFrame *, apex::AccessManager *> initAccesManager();

public:
    bool networkError;
};

struct LoadChecker : QObject {
    Q_OBJECT
public:
    LoadChecker(ApexMainTest *testSuite) : testSuite(testSuite)
    {
    }
public slots:
    void check(QNetworkReply *reply)
    {
        testSuite->networkError = reply->error();
    }

private:
    ApexMainTest *testSuite;
};

#endif
