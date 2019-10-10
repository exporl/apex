/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "apexmaintest.h"

#include "apexmain/apexcontrol.h"
#include "apexmain/errorhandler.h"
#include "apexmain/experimentio.h"
#include "apexmain/gui/mainwindow.h"
#include "apexmain/runner/flowrunner.h"

#include "apexdata/experimentdata.h"

#include "common/paths.h"
#include "common/testutils.h"

#include <QAction>
#include <QFileDialog>
#include <QPushButton>
#include <QSignalSpy>

using namespace apex;
using namespace cmn;

static QAbstractButton *findButtonWithText(QWidget *widget, const QString &text)
{
    QList<QAbstractButton *> buttons =
        widget->findChildren<QAbstractButton *>();
    QAbstractButton *result = nullptr;

    Q_FOREACH (QAbstractButton *button, buttons)
        if (button->text() == text)
            result = button;
    return result;
}

void ApexMainTest::testFlowRunner()
{
    TEST_EXCEPTIONS_TRY

    ErrorHandler errorHandler;
    ApexControl control(true);

    QVERIFY(QSignalSpy(&control, &ApexControl::apexInitialized).wait());

    QString fileName = Paths::searchFile(QSL("tests/libapex/testrunner.apf"),
                                         Paths::dataDirectories());

    FlowRunner runner;
    runner.select(fileName);

    QVERIFY(
        QSignalSpy(&runner, SIGNAL(selected(data::ExperimentData *))).wait());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testRepeatButton()
{
    TEST_EXCEPTIONS_TRY

    ErrorHandler errorHandler;
    ApexControl control(true);
    QSignalSpy initializedSpy(&control, SIGNAL(apexInitialized()));
    QVERIFY(initializedSpy.wait());

    QString fileName =
        Paths::searchFile(QSL("tests/libapex/panel-showrepeatbutton.apx"),
                          Paths::dataDirectories());
    control.fileOpen(fileName);

    const QStateMachine *stateMachine =
        control.getCurrentExperimentControl().machine();
    QState *waitForStartState =
        stateMachine->findChild<QState *>(QSL("WaitForStart"));
    QSignalSpy startSpy(waitForStartState, SIGNAL(entered()));
    QVERIFY(startSpy.count() == 1 || startSpy.wait());

    /* Start experiment */
    QState *runningTrialState =
        stateMachine->findChild<QState *>(QSL("RunningTrial"));
    QSignalSpy runningTrialSpy(runningTrialState, SIGNAL(entered()));
    QState *responseState =
        stateMachine->findChild<QState *>(QSL("WaitingForResponse"));
    QSignalSpy responseSpy(responseState, SIGNAL(entered()));
    QTest::mouseClick(findButtonWithText(control.mainWindow(), QSL("Start")),
                      Qt::LeftButton);
    QVERIFY(runningTrialSpy.count() == 1 || runningTrialSpy.wait());
    QVERIFY(responseSpy.count() == 1 || responseSpy.wait());
    QVERIFY(control.isExperimentRunning());

    /* Repeat trial */
    QSignalSpy repeatTrialSpy(control.getCurrentExperimentControl().io(),
                              SIGNAL(repeatTrialRequest()));
    QState *restartState =
        stateMachine->findChild<QState *>(QSL("RestartingTrial"));
    QSignalSpy restartSpy(restartState, SIGNAL(entered()));
    QAbstractButton *repeatButton =
        findButtonWithText(control.mainWindow(), QSL("Repeat stimulus"));
    QVERIFY(repeatButton);
    QVERIFY(repeatButton->isVisible());
    QVERIFY(repeatButton->isEnabled());

    QTest::mouseClick(repeatButton, Qt::LeftButton);

    QVERIFY(repeatTrialSpy.count() == 1 || repeatTrialSpy.wait());
    QVERIFY(restartSpy.count() == 1);

    TEST_EXCEPTIONS_CATCH
}
