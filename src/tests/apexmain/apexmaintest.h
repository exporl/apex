/******************************************************************************
 * Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _EXPORL_SRC_TESTS_APEXMAIN_APEXMAINTEST_H_
#define _EXPORL_SRC_TESTS_APEXMAIN_APEXMAINTEST_H_

#include "../testmacros.h"

#include "apextools/xml/xercesinclude.h"

#include <QDomDocument>
#include <QNetworkReply>
#include <QWebFrame>

#include <QtTest>

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
        void testQic();
        void testAseq();
        void testL34DatablockAseq();
        void testAseqParser();
        void testL34DatablockAseq_data();
        void testL34DatablockAseqMapping();
        void testL34DatablockInvalidFile();
        void testL34Datablock_invalid();
        void testL34DatablockQic();
        void testL34InvalidCL();
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
        void cleanupTestCase();

    private:
        template <typename T>
        void verifyInterval(RandomGeneratorTestParameters* params, T min, T max);
        static QDomDocument readXmlResults(const QString &fileName);
        static QString compareXml(const QDomNode &actual, const QDomNode &expected);

        void wait();
        QPair<QWebFrame*, apex::AccessManager*> initAccesManager();
    public:
        bool networkError;
};

struct LoadChecker : QObject {
    Q_OBJECT
public:
    LoadChecker(ApexMainTest* testSuite) : testSuite(testSuite) {}
public slots:
    void check(QNetworkReply* reply) {
        testSuite->networkError = reply->error();
    }
private:
    ApexMainTest* testSuite;
};

#endif
