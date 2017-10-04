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

#ifndef _APEX_SRC_TESTS_PROCEDURES_PROCEDURESTEST_H_
#define _APEX_SRC_TESTS_PROCEDURES_PROCEDURESTEST_H_

#include "apexdata/procedure/procedureinterface.h"
#include "apexdata/procedure/trial.h"

#include <QSet>

#include <QtTest>

#include <memory>

namespace apex
{

class ExperimentRunDelegate;

namespace data
{
class ExperimentData;
class MainConfigFileData;
class ConstantProcedureData;
class TrainingProcedureData;
}
}

typedef QList<QStringList>::Iterator ListIt;
class ProceduresTest : public QObject
{
    Q_OBJECT

public:
    QString name() const;

private Q_SLOTS:
    void initTestCase();
    void testEqualCorrector();
    void testProcedureApi();
    void testConstantProcedureParser();
    void testConstantProcedure();
    void testScriptProcedure();
    void testDummyProcedure();
    void testTrainingProcedure();
    void testAdaptiveProcedure();
    void testKaernbachProcedure();
    void testMultiProcedureOneByOne();
    void testMultiProcedureSequential();
    void testMultiProcedureRandom();

    void testScriptProcedureInvalidTrial();

private:
    void createConstantProcedure(apex::data::ConstantProcedureData *&data,
                                 QStringList &expectedStimuli);
    void createTrainingProcedure(apex::data::TrainingProcedureData *&data,
                                 QString &answerElement,
                                 QStringList &trial1Stimuli);
    void testTrial(const apex::data::Trial &trial, const QString id,
                   const int count) const;

    apex::data::ExperimentData *makeDummyExperimentData();
    apex::data::ExperimentData *makeStimulusExperimentData();
    apex::ExperimentRunDelegate *
    makeDummyExperimentRunDelegate(const apex::data::MainConfigFileData &mcd);
    apex::ExperimentRunDelegate *makeStimulusExperimentRunDelegate(
        const apex::data::MainConfigFileData &mcd);
    ListIt indexOf(const QString &text, QList<QStringList> &list);

    void checkResultXml(QString xml, QSet<QString> elements, QString type = "",
                        QString id = "",
                        QList<QStringList> alternatives = QList<QStringList>());
};

#endif
