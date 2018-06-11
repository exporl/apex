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

#ifdef Q_OS_ANDROID
#include "apexmain/apexandroidnative.h"
#endif
#include "apexmain/gui/studydialog.h"
#include "apexmain/study/manageddirectory.h"
#include "apexmain/study/study.h"
#include "apexmain/study/studymanager.h"

#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "common/testutils.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHostInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTextBrowser>
using namespace apex;

void ApexMainTest::testStudyDialog()
{
#ifndef Q_OS_ANDROID
    TEST_EXCEPTIONS_TRY

    StudyDialog dialog;
    QSignalSpy dialogAcceptSpy(&dialog, SIGNAL(accepted()));
    QTimer::singleShot(5000, &dialog, SLOT(reject()));
    QTimer::singleShot(500, [&]() {
        QSignalSpy sharePublicKeySpy(&dialog, SIGNAL(sharePublicKey(QString)));
        dialog.findChild<QAbstractButton *>(QSL("sharePublicKeyButton"))
            ->click();
        QCOMPARE(sharePublicKeySpy.count(), 1);

        QSignalSpy linkStudySpy(
            &dialog,
            SIGNAL(linkStudy(QString, QString, QString, QString, QString)));
        QSignalSpy experimentUrlSetSpy(&dialog,
                                       SIGNAL(experimentsUrlSet(QString)));

        dialog.findChild<QTabWidget *>(QSL("tabWidget"))
            ->setCurrentWidget(dialog.findChild<QWidget *>(QSL("studiesTab")));

        QCOMPARE(
            dialog.findChild<QStackedWidget *>(QSL("studiesStackedWidget"))
                ->currentWidget(),
            dialog.findChild<QWidget *>(QSL("studyInfoPage")));
        dialog.findChild<QAbstractButton *>(QSL("newStudyButton"))->click();
        QCOMPARE(
            dialog.findChild<QStackedWidget *>(QSL("studiesStackedWidget"))
                ->currentWidget(),
            dialog.findChild<QWidget *>(QSL("linkStudyPage")));
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());
        QVERIFY(
            dialog.findChild<QLineEdit *>(QSL("studyNameEdit"))->isEnabled());
        QVERIFY(dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))
                    ->isEnabled());
        QVERIFY(dialog.findChild<QCheckBox *>(QSL("uploadResultsCheckBox"))
                    ->isEnabled());
        QVERIFY(
            dialog.findChild<QLineEdit *>(QSL("resultsUrlEdit"))->isEnabled());
        QVERIFY(
            !dialog.findChild<QComboBox *>(QSL("experimentsBranchComboBox"))
                 ->isEnabled());

        QTest::mouseClick(
            dialog.findChild<QCheckBox *>(QSL("uploadResultsCheckBox")),
            Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(1, 1));

        QTest::keyClicks(
            dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit")),
            QSL("https://someurl.domain/somename.git"));
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->clearFocus();
        QCOMPARE(experimentUrlSetSpy.count(), 1);
        dialog.linkStudySetExperimentsBranches(QStringList() << QSL("master"));
        QVERIFY(dialog.findChild<QComboBox *>(QSL("experimentsBranchComboBox"))
                    ->isEnabled());

        /* results url autofill */
        QCOMPARE(
            dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->text(),
            dialog.findChild<QLineEdit *>(QSL("resultsUrlEdit"))->text());
        /* name contains last part of url minus .git if present */
        QCOMPARE(dialog.findChild<QLineEdit *>(QSL("studyNameEdit"))->text(),
                 QSL("somename"));

        /* same branch as experiments branch is invalid */
        dialog.linkStudySetResultsBranch(QSL("master"));
        dialog.findChild<QLineEdit *>(QSL("resultsBranchEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("resultsBranchEdit"))->clearFocus();
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());
        /* empty branch is invalid */
        dialog.linkStudySetResultsBranch(QString());
        dialog.findChild<QLineEdit *>(QSL("resultsBranchEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("resultsBranchEdit"))->clearFocus();
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());

        /* different from experiments branch is valid */
        dialog.linkStudySetResultsBranch(QSL("resultsbranch"));
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->clearFocus();
        QCOMPARE(experimentUrlSetSpy.count(), 2);
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                    ->isEnabled());

        /* empty resultsurl + uploadresultscheckbox checked is invalid */
        dialog.findChild<QLineEdit *>(QSL("resultsUrlEdit"))->clear();
        dialog.findChild<QLineEdit *>(QSL("resultsUrlEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("resultsUrlEdit"))->clearFocus();
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());
        /* empty resultsurl + upload results unchecked is valid */
        QTest::mouseClick(
            dialog.findChild<QCheckBox *>(QSL("uploadResultsCheckBox")),
            Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(1, 1));
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                    ->isEnabled());

        /* auto fill resultsurl again */
        QTest::mouseClick(
            dialog.findChild<QCheckBox *>(QSL("uploadResultsCheckBox")),
            Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(1, 1));
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->clearFocus();
        QCOMPARE(experimentUrlSetSpy.count(), 3);
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                    ->isEnabled());

        /* empty name is invalid */
        dialog.findChild<QLineEdit *>(QSL("studyNameEdit"))->setText(QString());
        dialog.findChild<QLineEdit *>(QSL("studyNameEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("studyNameEdit"))->clearFocus();
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());

        /* refocus and autofill name */
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->setFocus();
        dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->clearFocus();
        QCOMPARE(experimentUrlSetSpy.count(), 4);
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                    ->isEnabled());

        dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))->click();
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());
        QVERIFY(
            !dialog.findChild<QLineEdit *>(QSL("studyNameEdit"))->isEnabled());
        QVERIFY(!dialog.findChild<QLineEdit *>(QSL("experimentsUrlEdit"))
                     ->isEnabled());
        QVERIFY(
            !dialog.findChild<QComboBox *>(QSL("experimentsBranchComboBox"))
                 ->isEnabled());
        QVERIFY(!dialog.findChild<QCheckBox *>(QSL("uploadResultsCheckBox"))
                     ->isEnabled());
        QVERIFY(
            !dialog.findChild<QLineEdit *>(QSL("resultsUrlEdit"))->isEnabled());
        QVERIFY(!dialog.findChild<QAbstractButton *>(QSL("closeButton"))
                     ->isEnabled());
        dialog.updateStudies(QStringList(QSL("somename")), QSL("somename"),
                             QSL("status message"));
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("closeButton"))
                    ->isEnabled());
        QCOMPARE(linkStudySpy.count(), 1);
        QCOMPARE(dialog.findChild<QComboBox *>(QSL("studiesComboBox"))
                     ->currentText(),
                 QSL("somename"));
        QVERIFY(
            dialog
                .findChild<QTextBrowser *>(QSL("activeStudyStatusTextBrowser"))
                ->find(QSL("status message")));

        /* button visibility check on active study */
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("activateStudyButton"))
                    ->isHidden());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("startStudyButton"))
                    ->isVisible());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("pauseStudyButton"))
                    ->isVisible());

        QSignalSpy syncStudySpy(&dialog, SIGNAL(syncActiveStudy()));
        dialog.findChild<QAbstractButton *>(QSL("syncStudyButton"))->click();
        QVERIFY(
            !dialog.findChild<QWidget *>(QSL("studiesButtons"))->isEnabled());
        QCOMPARE(syncStudySpy.count(), 1);
        dialog.updateStudies(QStringList(QSL("somename")), QSL("somename"),
                             QSL("status message"));
        QVERIFY(
            dialog.findChild<QWidget *>(QSL("studiesButtons"))->isEnabled());

        QSignalSpy pauseStudySpy(&dialog, SIGNAL(pauseActiveStudy()));
        dialog.findChild<QAbstractButton *>(QSL("pauseStudyButton"))->click();
        QVERIFY(
            !dialog.findChild<QWidget *>(QSL("studiesButtons"))->isEnabled());
        QCOMPARE(pauseStudySpy.count(), 1);
        dialog.updateStudies(QStringList(QSL("somename")), QString(),
                             QString());
        QVERIFY(
            dialog.findChild<QWidget *>(QSL("studiesButtons"))->isEnabled());

        /* button visibility check on paused study */
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("activateStudyButton"))
                    ->isVisible());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("startStudyButton"))
                    ->isHidden());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("pauseStudyButton"))
                    ->isHidden());
        QSignalSpy activateStudySpy(&dialog, SIGNAL(activateStudy(QString)));
        dialog.findChild<QAbstractButton *>(QSL("activateStudyButton"))
            ->click();
        QVERIFY(
            !dialog.findChild<QWidget *>(QSL("studiesButtons"))->isEnabled());
        QVERIFY(
            dialog
                .findChild<QGroupBox *>(QSL("activeStudyStatusMessageGroupBox"))
                ->isVisible());
        QCOMPARE(activateStudySpy.count(), 1);
        dialog.updateStudies(QStringList(QSL("somename")), QSL("somename"),
                             QSL("status message"));
        QVERIFY(
            dialog.findChild<QWidget *>(QSL("studiesButtons"))->isEnabled());

        /* start and delete button */
        QSignalSpy startStudySpy(&dialog, SIGNAL(startActiveStudy()));
        dialog.findChild<QAbstractButton *>(QSL("startStudyButton"))->click();
        QCOMPARE(startStudySpy.count(), 1);

        /* QMessageBox */
        QTimer::singleShot(500, [&]() {
            Q_FOREACH (QObject *child, dialog.children()) {
                if (child->inherits("QMessageBox")) {
                    QMessageBox *mb = qobject_cast<QMessageBox *>(child);
                    mb->button(QMessageBox::Yes)->click();
                }
            }
        });
        QSignalSpy deleteStudySpy(&dialog, SIGNAL(deleteStudy(QString)));
        dialog.findChild<QAbstractButton *>(QSL("deleteStudyButton"))->click();
        dialog.updateStudies(QStringList(), QString(), QString());
        QCOMPARE(deleteStudySpy.count(), 1);
        QCOMPARE(dialog.findChild<QComboBox *>(QSL("studiesComboBox"))->count(),
                 0);
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("activateStudyButton"))
                    ->isHidden());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("startStudyButton"))
                    ->isHidden());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("pauseStudyButton"))
                    ->isHidden());

        /* Active study is shown */
        QStringList twoStudies;
        twoStudies << QSL("somename") << QSL("somename2");
        dialog.updateStudies(twoStudies, QSL("somename"),
                             QSL("status message"));
        QCOMPARE(dialog.findChild<QComboBox *>(QSL("studiesComboBox"))->count(),
                 2);
        QCOMPARE(dialog.findChild<QComboBox *>(QSL("studiesComboBox"))
                     ->currentText(),
                 QSL("somename"));
        QCOMPARE(dialog.findChild<QComboBox *>(QSL("studiesComboBox"))
                     ->currentData()
                     .toBool(),
                 true);
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("activateStudyButton"))
                    ->isHidden());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("startStudyButton"))
                    ->isVisible());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("pauseStudyButton"))
                    ->isVisible());

        /* select second study */
        QComboBox *studiesBox =
            dialog.findChild<QComboBox *>(QSL("studiesComboBox"));
        QSignalSpy studiesBoxActivated(studiesBox, SIGNAL(activated(int)));
        QTest::mouseClick(studiesBox, Qt::LeftButton);
        QModelIndex index = studiesBox->model()->index(
            studiesBox->findText(QSL("somename2")), 0);
        QVERIFY(index.isValid());
        studiesBox->view()->setCurrentIndex(index);
        QTest::keyPress(studiesBox->view(), Qt::Key_Return);
        QCOMPARE(studiesBox->currentText(), QSL("somename2"));
        QCOMPARE(studiesBox->currentData().toBool(), false);
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("activateStudyButton"))
                    ->isVisible());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("startStudyButton"))
                    ->isHidden());
        QVERIFY(dialog.findChild<QAbstractButton *>(QSL("pauseStudyButton"))
                    ->isHidden());

        dialog.findChild<QAbstractButton *>(QSL("closeButton"))->click();
    });

    dialog.exec();
    QCOMPARE(dialogAcceptSpy.count(), 1);

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("Skipped on Android");
#endif
}

void ApexMainTest::testStudy()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryDir dir1;
    ManagedDirectory managedDir(dir1.path());
    managedDir.init();
    QVERIFY(managedDir.exists());
    managedDir.setAuthor(QSL("dummy"), QSL("dummy@dummy.com"));
    QFile newFile(dir1.path() + QL1S("/index.apf"));
    QVERIFY(newFile.open(QIODevice::ReadWrite));
    newFile.write(QL1S("someData\n").data());
    newFile.close();
    managedDir.add(newFile.fileName());

    QDir(dir1.path()).mkpath(QSL("experiments"));
    QFile newFile2(dir1.path() + QL1S("/experiments/experiment.apx"));
    QVERIFY(newFile2.open(QIODevice::ReadWrite));
    newFile2.write(QL1S("someData\n").data());
    newFile2.close();
    managedDir.add(newFile2.fileName());

    QTemporaryDir dir2;
    Study study(QSL("teststudy"), dir1.path(), QSL("master"), dir1.path(),
                QSL("results"), dir2.path());
    QVERIFY(study.isPrivate());
    QVERIFY(study.statusMessage().isEmpty());
    QSignalSpy updateExperimentsDoneSpy(&study,
                                        SIGNAL(updateExperimentsDone()));
    study.updateExperiments();
    QVERIFY(updateExperimentsDoneSpy.wait());
    QVERIFY(study.statusMessage().contains(QSL("experiments/experiment.apx")));
    QVERIFY(study.indexExperiment().endsWith(QSL("index.apf")));
    QFile::copy(newFile2.fileName(),
                dir1.path() + QL1S("/experiments/experiment-2.apx"));
    managedDir.add(dir1.path() + QL1S("/experiments/experiment-2.apx"));
    study.fetchExperiments(true);
    study.checkoutExperiments(true);
    QVERIFY(
        study.statusMessage().contains(QSL("experiments/experiment-2.apx")));

    QVERIFY(study.belongsToStudy(study.indexExperiment()));
    QVERIFY(study.belongsToStudy(QSL("index.apf")));
    QVERIFY(study.belongsToStudy(QSL("experiments/experiment.apx")));
    QVERIFY(study.belongsToStudy(QSL("experiments/experiment-2.apx")));
    QVERIFY(study.belongsToStudy(
        QDir(dir2.path()).filePath(QSL("teststudy/experiments/index.apf"))));
    QVERIFY(study.belongsToStudy(
        QDir(dir2.path())
            .filePath(
                QSL("teststudy/experiments/experiments/experiment.apx"))));

    QRegularExpression resultRegex(
        dir2.path() + QL1S("/teststudy/results/") +
#ifdef Q_OS_ANDROID
        QRegularExpression::escape(
            android::ApexAndroidBridge::getDeviceSerialNumber()) +
#else
        QRegularExpression::escape(QString::fromLatin1("%1_%2")
                                       .arg(ApexTools::getUser())
                                       .arg(QHostInfo::localHostName())) +
#endif
        QL1S("/experiments/experiment-\\d{4}\\d{2}\\d{2}T\\d{"
             "2}\\d{2}\\d{2}Z.apr"));
    QVERIFY(
        resultRegex
            .match(study.makeResultsFilePath(QSL("experiments/experiment.apx")))
            .hasMatch());
    QVERIFY(
        resultRegex
            .match(study.makeResultsFilePath(
                QDir(dir2.path())
                    .filePath(QSL(
                        "teststudy/experiments/experiments/experiment.apx"))))
            .hasMatch());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testStudyManager()
{
    TEST_EXCEPTIONS_TRY

    QString privateStudyName = QSL("privateTestStudy");
    QString publicStudyName = QSL("publicTestStudy");
    QTemporaryDir dir1;
    ManagedDirectory managedDir(dir1.path());
    managedDir.init();
    QVERIFY(managedDir.exists());
    managedDir.setAuthor(QSL("dummy"), QSL("dummy@dummy.com"));
    QFile newFile(dir1.path() + QL1S("/index.apf"));
    QVERIFY(newFile.open(QIODevice::ReadWrite));
    newFile.write(QL1S("someData\n").data());
    newFile.close();
    managedDir.add(newFile.fileName());
    QDir(dir1.path()).mkpath(QSL("experiments"));
    QFile newFile2(dir1.path() + QL1S("/experiments/experiment.apx"));
    QVERIFY(newFile2.open(QIODevice::ReadWrite));
    newFile2.write(QL1S("someData\n").data());
    newFile2.close();
    managedDir.add(newFile2.fileName());

    QTemporaryDir settingsDir;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       settingsDir.path());

    QTemporaryDir dir2;
    StudyManager manager;
    manager.setRootPath(dir2.path());
    QVERIFY(QFile::exists(QDir(dir2.path()).filePath(QSL("id_rsa"))));
    QVERIFY(QFile::exists(QDir(dir2.path()).filePath(QSL("id_rsa.pub"))));

    /* We rely on the comment having this format to make adding
     * a device to a gerrit instance for studies easier.
     * Account name and email are extracted from this comment.
     */
    QFile publicKeyFile(QDir(dir2.path()).filePath(QSL("id_rsa.pub")));
    publicKeyFile.open(QIODevice::ReadOnly);
    QString publicKeyData = QString::fromUtf8(publicKeyFile.readAll());
#ifdef Q_OS_ANDROID
    QCOMPARE(publicKeyData.split(' ').last(),
             QString::fromLatin1("%1@%2")
                 .arg(android::ApexAndroidBridge::getDeviceSerialNumber())
                 .arg(android::ApexAndroidBridge::getDeviceSerialNumber()));
#else
    QCOMPARE(publicKeyData.split(' ').last(),
             QString::fromLatin1("%1@%2")
                 .arg(ApexTools::getUser())
                 .arg(QHostInfo::localHostName()));
#endif

    QSignalSpy studiesUpdatedSpy(
        &manager, SIGNAL(studiesUpdated(QStringList, QString, QString)));
    QSignalSpy studyMessageUpdatedSpy(
        &manager, SIGNAL(studyStatusMessageUpdated(QString)));

    /* public study */
    manager.linkStudy(publicStudyName, dir1.path(), QSL("master"), QString(),
                      QString());
    QVERIFY(studiesUpdatedSpy.wait()); /* pull operation */
    QCOMPARE(manager.studies(), QStringList(publicStudyName));
    QVERIFY(manager.activeStudy());
    QVERIFY(manager.activeStudy()->isPublic());

    QSettings settings;
    settings.beginGroup(QSL("StudyManager"));
    QCOMPARE(settings.value(QSL("activeStudy")).toString(),
             manager.activeStudy()->name());
    int studyCount = settings.beginReadArray(QSL("studies"));
    QCOMPARE(studyCount, 1);
    for (int i = 0; i < studyCount; ++i) {
        settings.setArrayIndex(i);
        QCOMPARE(settings.value(QSL("name")).toString(),
                 manager.activeStudy()->name());
        QCOMPARE(settings.value(QSL("experimentsUrl")).toString(),
                 manager.activeStudy()->experimentsUrl());
        QCOMPARE(settings.value(QSL("experimentsBranch")).toString(),
                 manager.activeStudy()->experimentsBranch());
        QCOMPARE(settings.value(QSL("resultsUrl")).toString(),
                 manager.activeStudy()->resultsUrl());
        QCOMPARE(settings.value(QSL("resultsBranch")).toString(),
                 manager.activeStudy()->resultsBranch());
    }

    /* study already exists so studiesUpdated count won't be increased */
    manager.linkStudy(publicStudyName, dir1.path(), QSL("master"), QString(),
                      QString());

    studiesUpdatedSpy.clear();
    studyMessageUpdatedSpy.clear();
    manager.pauseActiveStudy();
    /* Public study doesn't do anything on pause */
    QCOMPARE(studyMessageUpdatedSpy.count(), 0);
    QCOMPARE(studiesUpdatedSpy.count(), 1);
    QCOMPARE(manager.studies(), QStringList(publicStudyName));
    QVERIFY(manager.activeStudy().isNull());

    studiesUpdatedSpy.clear();
    manager.setActiveStudy(publicStudyName);
    QCOMPARE(studyMessageUpdatedSpy.count(), 1);
    QVERIFY(studiesUpdatedSpy.wait()); /* pull operation */
    QCOMPARE(studiesUpdatedSpy.count(), 1);

    /* Activating again immediately returns */
    studyMessageUpdatedSpy.clear();
    manager.setActiveStudy(publicStudyName);
    QCOMPARE(studyMessageUpdatedSpy.count(), 0);

    QCOMPARE(manager.studies(), QStringList(publicStudyName));
    QVERIFY(manager.activeStudy());
    QVERIFY(manager.belongsToActiveStudy(
        dir2.path() + QL1S("/") + publicStudyName +
        QL1S("/experiments/experiments/experiment.apx")));

    QTemporaryFile existingFile;
    QVERIFY(existingFile.open());
    existingFile.close();
    QVERIFY(!manager.belongsToActiveStudy(existingFile.fileName()));

    QFile newFile3(dir1.path() + QL1S("/experiments/experiment-2.apx"));
    QVERIFY(newFile3.open(QIODevice::ReadWrite));
    newFile3.write(QL1S("someData\n").data());
    newFile3.close();
    managedDir.add(newFile3.fileName());
    studiesUpdatedSpy.clear();
    studyMessageUpdatedSpy.clear();
    manager.syncActiveStudy();
    QCOMPARE(studyMessageUpdatedSpy.count(), 1);
    QVERIFY(studiesUpdatedSpy.wait()); /* pull operation */
    QCOMPARE(studiesUpdatedSpy.count(), 1);
    QVERIFY(manager.belongsToActiveStudy(
        dir2.path() + QL1S("/") + publicStudyName +
        QL1S("/experiments/experiments/experiment-2.apx")));

    QVERIFY(manager
                .newExperiment(dir1.path() + QL1S("/") + privateStudyName +
                               QL1S("/experiments/experiments/experiment.apx"))
                .isEmpty());
    /* public study ignores afterExperiment, shouldn't throw */
    manager.afterExperiment(QString());
    studiesUpdatedSpy.clear();
    manager.deleteStudy(publicStudyName);
    QCOMPARE(studiesUpdatedSpy.count(), 1);
    QVERIFY(manager.studies().isEmpty());
    QVERIFY(manager.activeStudy().isNull());

    /* private study */
    manager.linkStudy(privateStudyName, dir1.path(), QSL("master"), dir1.path(),
                      QSL("results"));
    QVERIFY(studiesUpdatedSpy.wait()); /* pull operation */
    QCOMPARE(manager.studies(), QStringList(privateStudyName));
    QVERIFY(manager.activeStudy());
    QVERIFY(manager.activeStudy()->isPrivate());

    QString resultsFilePath =
        manager.newExperiment(dir2.path() + QL1S("/") + privateStudyName +
                              QL1S("/experiments/experiments/experiment.apx"));
    QRegularExpression resultRegex(
        dir2.path() + QL1S("/") + privateStudyName + QL1S("/results/") +
#ifdef Q_OS_ANDROID
        QRegularExpression::escape(
            android::ApexAndroidBridge::getDeviceSerialNumber()) +
#else
        QRegularExpression::escape(QString::fromLatin1("%1_%2")
                                       .arg(ApexTools::getUser())
                                       .arg(QHostInfo::localHostName())) +
#endif
        QL1S(("/experiments/experiment-\\d{4}\\d{2}\\d{2}T\\d{"
              "2}\\d{2}\\d{2}Z.apr")));
    QVERIFY(resultRegex.match(resultsFilePath).hasMatch());
    QFile resultsFile(resultsFilePath);
    QVERIFY(resultsFile.open(QIODevice::ReadWrite));
    resultsFile.write(QL1S("someData\n").data());
    resultsFile.close();

    QVERIFY(manager.belongsToActiveStudy(resultsFilePath));
    manager.afterExperiment(resultsFilePath);
    /* push to local non-bare repos is not supported by libgit2,
     * so check by opening the results repo directly.
     */
    ManagedDirectory resultsDirectory(
        QDir(dir2.path())
            .filePath(manager.activeStudy()->name() + QL1S("/results")));
    resultsDirectory.open();
    QVERIFY(resultsDirectory.lastCommitMessage().contains(
        QFileInfo(resultsFilePath).fileName()));
    resultsDirectory.close();

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testStudyFull()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryDir settingsDir;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       settingsDir.path());

    QTemporaryDir dir1;
    ManagedDirectory remoteDir(dir1.path());
    remoteDir.init();
    QVERIFY(remoteDir.exists());
    remoteDir.setAuthor(QSL("dummy"), QSL("dummy@dummy.com"));
    QFile newFile(dir1.path() + QL1S("/index.apf"));
    QVERIFY(newFile.open(QIODevice::ReadWrite));
    newFile.write(QL1S("someData\n").data());
    newFile.close();
    remoteDir.add(newFile.fileName());

    QTemporaryDir dir2;
    StudyManager manager;
    manager.setRootPath(dir2.path());
    QVERIFY(QFile::exists(QDir(dir2.path()).filePath(QSL("id_rsa"))));
    QVERIFY(QFile::exists(QDir(dir2.path()).filePath(QSL("id_rsa.pub"))));
    QVERIFY(!manager.activeStudy());
    QPointer<StudyDialog> dialog = nullptr;
    QTimer::singleShot(500, [&]() {
        Q_FOREACH (QWidget *widget, QApplication::topLevelWidgets())
            if (widget->objectName() == QSL("StudyDialog"))
                dialog = (StudyDialog *)widget;
        QVERIFY(dialog);
        QTimer::singleShot(10000, dialog, SLOT(reject()));

        /* private study */
        dialog->findChild<QTabWidget *>(QSL("tabWidget"))
            ->setCurrentWidget(dialog->findChild<QWidget *>(QSL("studiesTab")));
        dialog->findChild<QAbstractButton *>(QSL("newStudyButton"))->click();
        QSignalSpy experimentUrlSetSpy(dialog,
                                       SIGNAL(experimentsUrlSet(QString)));
        dialog->findChild<QGroupBox *>(QSL("linkStudyAdvancedForm"))->show();
        dialog->findChild<QCheckBox *>(QSL("uploadResultsCheckBox"))
            ->setCheckState(Qt::Checked);
        QTest::keyClicks(
            dialog->findChild<QLineEdit *>(QSL("experimentsUrlEdit")),
            dir1.path());
        dialog->findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->setFocus();
        dialog->findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->clearFocus();
        QCOMPARE(experimentUrlSetSpy.count(), 1);

        QString studyName = QDir(dir1.path()).dirName().split('.').first();
        QCOMPARE(studyName,
                 dialog->findChild<QLineEdit *>(QSL("studyNameEdit"))->text());
        QVERIFY(dialog->findChild<QAbstractButton *>(QSL("linkStudyButton"))
                    ->isEnabled());

        QSignalSpy linkStudySpy(
            dialog,
            SIGNAL(linkStudy(QString, QString, QString, QString, QString)));
        QSignalSpy studiesUpdatedSpy(
            &manager, SIGNAL(studiesUpdated(QStringList, QString, QString)));
        QSignalSpy studyMessageUpdatedSpy(
            &manager, SIGNAL(studyStatusMessageUpdated(QString)));

        dialog->findChild<QAbstractButton *>(QSL("linkStudyButton"))->click();
        QVERIFY(linkStudySpy.count() == 1);
        QVERIFY(studiesUpdatedSpy.wait());
        QVERIFY(manager.activeStudy());
        QCOMPARE(manager.studies(), QStringList(studyName));
        QCOMPARE(manager.activeStudy()->name(), studyName);
        QCOMPARE(manager.activeStudy()->indexExperiment(),
                 QDir(dir2.path())
                     .filePath(manager.activeStudy()->name() +
                               QL1S("/experiments/index.apf")));

        QFile newFile2(dir1.path() + QL1S("/experiment.apx"));
        QVERIFY(newFile2.open(QIODevice::ReadWrite));
        newFile2.write(QL1S("someData\n").data());
        newFile2.close();
        remoteDir.add(newFile2.fileName());

        /* Don't use syncStudy here as results will error because of local
         * non-bare repo.
         */
        QSignalSpy updateExperimentsSpy(manager.activeStudy().data(),
                                        SIGNAL(updateExperimentsDone()));
        manager.activeStudy()->updateExperiments();
        QVERIFY(updateExperimentsSpy.wait());
        QVERIFY(manager.activeStudy()->belongsToStudy(
            QDir(dir2.path())
                .filePath(manager.activeStudy()->name() +
                          QL1S("/experiments/experiment.apx"))));

        QFile resultsFile(manager.activeStudy()->makeResultsFilePath(
            QDir(dir2.path())
                .filePath(manager.activeStudy()->name() +
                          QL1S("/experiments/experiment.apx"))));
        QVERIFY(resultsFile.open(QIODevice::ReadWrite));
        resultsFile.write(QL1S("someData\n").data());
        resultsFile.close();
        QVERIFY(manager.belongsToActiveStudy(resultsFile.fileName()));
        manager.afterExperiment(resultsFile.fileName());
        /* push to local non-bare repos is not supported by libgit2,
         * so check by opening the results repo directly.
         */
        ManagedDirectory resultsDirectory(
            QDir(dir2.path())
                .filePath(manager.activeStudy()->name() + QL1S("/results")));
        resultsDirectory.open();
        QVERIFY(resultsDirectory.lastCommitMessage().contains(
            QFileInfo(resultsFile.fileName()).fileName()));
        resultsDirectory.close();

        /* public study */
        dialog->findChild<QAbstractButton *>(QSL("newStudyButton"))->click();
        QTest::keyClicks(
            dialog->findChild<QLineEdit *>(QSL("experimentsUrlEdit")),
            dir1.path());
        dialog->findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->setFocus();
        dialog->findChild<QLineEdit *>(QSL("experimentsUrlEdit"))->clearFocus();
        QCOMPARE(dialog->findChild<QLineEdit *>(QSL("studyNameEdit"))->text(),
                 studyName);
        QVERIFY(!dialog->findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());

        QString publicStudyName = studyName + QL1S("public");
        QTest::keyClicks(dialog->findChild<QLineEdit *>(QSL("studyNameEdit")),
                         QSL("public"));
        QVERIFY(!dialog->findChild<QAbstractButton *>(QSL("linkStudyButton"))
                     ->isEnabled());
        dialog->findChild<QCheckBox *>(QSL("uploadResultsCheckBox"))
            ->setCheckState(Qt::Unchecked);
        QVERIFY(dialog->findChild<QAbstractButton *>(QSL("linkStudyButton"))
                    ->isEnabled());
        dialog->findChild<QAbstractButton *>(QSL("linkStudyButton"))->click();
        QVERIFY(linkStudySpy.count() == 2);
        QVERIFY(studiesUpdatedSpy.wait());
        QVERIFY(manager.activeStudy());
        QVERIFY(manager.activeStudy()->isPublic());

        QCOMPARE(manager.studies(), QStringList() << studyName
                                                  << publicStudyName);
        QCOMPARE(manager.activeStudy()->name(), publicStudyName);
        QCOMPARE(
            manager.activeStudy()->indexExperiment(),
            QDir(dir2.path())
                .filePath(publicStudyName + QL1S("/experiments/index.apf")));

        /* exception making resultsfilepath with public study */
        QVERIFY_EXCEPTION_THROWN(
            manager.activeStudy()->makeResultsFilePath(
                QDir(dir2.path())
                    .filePath(publicStudyName +
                              QL1S("/experiments/experiment.apx"))),
            ApexException);

        dialog->findChild<QAbstractButton *>(QSL("pauseStudyButton"))->click();
        dialog->findChild<QComboBox *>(QSL("studiesComboBox"))
            ->setCurrentIndex(
                dialog->findChild<QComboBox *>(QSL("studiesComboBox"))
                    ->findText(publicStudyName));
        QVERIFY(QFile::copy(dir1.path() + QL1S("/experiment.apx"),
                            dir1.path() + QL1S("/experiment-2.apx")));
        remoteDir.add(dir1.path() + QL1S("/experiment-2.apx"));
        dialog->findChild<QAbstractButton *>(QSL("activateStudyButton"))
            ->click();
        QCOMPARE(studyMessageUpdatedSpy.count(), 1);
        QVERIFY(studiesUpdatedSpy.wait());
        QVERIFY(QFile::copy(dir1.path() + QL1S("/experiment.apx"),
                            dir1.path() + QL1S("/experiment-3.apx")));
        remoteDir.add(dir1.path() + QL1S("/experiment-3.apx"));
        studyMessageUpdatedSpy.clear();
        dialog->findChild<QAbstractButton *>(QSL("syncStudyButton"))->click();
        QCOMPARE(studyMessageUpdatedSpy.count(), 1);
        QVERIFY(studiesUpdatedSpy.wait());

/* gets stuck on dealing with QMessageBox */
#ifndef Q_OS_ANDROID
        /* deletion */
        QTimer::singleShot(1000, [&]() {
            Q_FOREACH (QObject *child, dialog->children()) {
                if (child->inherits("QMessageBox")) {
                    QMessageBox *mb = qobject_cast<QMessageBox *>(child);
                    mb->button(QMessageBox::Yes)->click();
                }
            }
        });
        QVERIFY(QFile::exists(QDir(dir2.path()).filePath(publicStudyName)));
        dialog->findChild<QAbstractButton *>(QSL("deleteStudyButton"))->click();
        QVERIFY(!QFile::exists(QDir(dir2.path()).filePath(publicStudyName)));

        QTimer::singleShot(1000, [&]() {
            Q_FOREACH (QObject *child, dialog->children()) {
                if (child->inherits("QMessageBox")) {
                    QMessageBox *mb = qobject_cast<QMessageBox *>(child);
                    mb->button(QMessageBox::Yes)->click();
                }
            }
        });
        QVERIFY(QFile::exists(QDir(dir2.path()).filePath(studyName)));
        dialog->findChild<QAbstractButton *>(QSL("deleteStudyButton"))->click();
        QVERIFY(!QFile::exists(QDir(dir2.path()).filePath(studyName)));
#endif

        QSignalSpy acceptedSpy(dialog, SIGNAL(accepted()));
        dialog->findChild<QAbstractButton *>(QSL("closeButton"))->click();
        QVERIFY(acceptedSpy.count() == 1);
    });

    manager.showConfigurationDialog();
    QVERIFY(dialog.isNull());

    TEST_EXCEPTIONS_CATCH
}

void ApexMainTest::testManagedDirectory()
{
    TEST_EXCEPTIONS_TRY

    /* pull */
    QTemporaryDir dir1;
    ManagedDirectory managedDir(dir1.path());
    managedDir.init();
    managedDir.setAuthor(QSL("dummy"), QSL("dummy@dummy.com"));
    QVERIFY(ManagedDirectory::exists(dir1.path()));
    QFile newFile(dir1.path() + QL1S("/newFile"));
    QVERIFY(newFile.open(QIODevice::ReadWrite));
    newFile.write(QL1S("someData\n").data());
    newFile.close();
    managedDir.add(newFile.fileName());
    QCOMPARE(managedDir.totalCommitCount(), 1);
    QTemporaryDir dir2;
    ManagedDirectory managedDir2(dir2.path());
    managedDir2.init();
    managedDir2.setRemote(dir1.path());
    QSignalSpy pullDoneSpy(&managedDir2, SIGNAL(pullDone()));
    managedDir2.pull();
    QVERIFY(pullDoneSpy.wait(500));
    QCOMPARE(managedDir2.totalCommitCount(), 1);
    QVERIFY(QFile::exists(dir2.path() + QL1S("/newFile")));

    /* close and reopen */
    managedDir2.close();
    managedDir2.open();
    QFile newFile2(dir1.path() + QL1S("/newFile2"));
    QVERIFY(newFile2.open(QIODevice::ReadWrite));
    newFile2.write(QL1S("someData\n").data());
    newFile2.close();
    managedDir.add(newFile2.fileName());
    managedDir2.pull();
    QVERIFY(pullDoneSpy.wait(500));
    QCOMPARE(managedDir2.totalCommitCount(), 2);
    QVERIFY(QFile::exists(dir2.path() + QL1S("/newFile")));
    QVERIFY(QFile::exists(dir2.path() + QL1S("/newFile2")));

    /* push & pull */
    QTemporaryDir dir3;
    ManagedDirectory managedDir3(dir3.path());
    managedDir3.init(true);
    QTemporaryDir dir4;
    ManagedDirectory managedDir4(dir4.path());
    QSignalSpy pushDoneSpy(&managedDir4, SIGNAL(pushDone()));
    managedDir4.init();
    QFile newFile3(dir4.path() + QL1S("/newFile3"));
    QVERIFY(newFile3.open(QIODevice::ReadWrite));
    newFile3.write(QL1S("someData\n").data());
    newFile3.close();
    managedDir4.setAuthor(QSL("dummy"), QSL("dummy@dummy.com"));
    managedDir4.add(newFile3.fileName());
    managedDir4.setRemote(dir3.path());
    QVERIFY(managedDir4.hasLocalRemote());
    QCOMPARE(managedDir4.totalCommitCount(), 1);
    managedDir4.push();
    QVERIFY(pushDoneSpy.wait(500));
    QTemporaryDir dir5;
    ManagedDirectory managedDir5(dir5.path());
    QSignalSpy pullDoneSpy5(&managedDir5, SIGNAL(pullDone()));
    managedDir5.init();
    managedDir5.setRemote(dir3.path());
    /* fetch & checkout instead of pull */
    managedDir5.fetch(Qt::BlockingQueuedConnection);
    managedDir5.checkout(Qt::BlockingQueuedConnection);
    QCOMPARE(managedDir5.totalCommitCount(), 1);
    QVERIFY(QFile::exists(dir5.path() + QL1S("/newFile3")));

    QTemporaryDir dir6;
    ManagedDirectory managedDir6(dir6.path());
    QVERIFY_EXCEPTION_THROWN(managedDir6.open(), ApexException);

    TEST_EXCEPTIONS_CATCH
}
