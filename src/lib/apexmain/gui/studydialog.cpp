/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#include "studydialog.h"
#include "ui_studydialog.h"

#ifdef Q_OS_ANDROID
#include "../apexandroidnative.h"
#endif
#include "../mainconfigfileparser.h"

#include "apextools/apextools.h"
#include "apextools/global.h"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QProgressBar>
#include <QStandardPaths>

namespace apex
{
class StudyDialogPrivate : public QObject
{
    Q_OBJECT
public:
    StudyDialogPrivate(StudyDialog *dialog);

public Q_SLOTS:
    void tabWidgetCurrentChanged();

    void sharePublicKeyClicked();
    void exportPublicKeyToFile();

    void selectedStudyChanged(int index);
    void activateStudyClicked();
    void startStudyClicked();
    void pauseStudyClicked();
    void syncStudyClicked();
    void deleteStudyClicked();

    void newStudyClicked();
    void experimentsUrlEditingFinished();
    void validateStudyForm();
    bool validateWithMessage(bool valid, const QString &errorMessage);
    void linkStudyClicked();

Q_SIGNALS:
    void sharePublicKey(const QString &key);

    void activateStudy(const QString &study);
    void startActiveStudy();
    void pauseActiveStudy();
    void syncActiveStudy();
    void deleteStudy(const QString &study);

    void experimentsUrlSet(const QString &url);
    void linkStudy(QString name, QString experimentsUrl,
                   QString experimentsBranch, QString resultsUrl,
                   QString resultsBranch);

public:
    StudyDialog *p;
    Ui::StudyDialog ui;
    QString rootPath;
    QSharedPointer<QFileSystemModel> experimentsFSModel;
};

StudyDialogPrivate::StudyDialogPrivate(StudyDialog *dialog) : p(dialog)
{
    ui.setupUi(dialog);

    connect(ui.closeButton, SIGNAL(clicked()), p, SLOT(accept()));
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this,
            SLOT(tabWidgetCurrentChanged()));
    /* public key text */
    ui.tabWidget->setCurrentWidget(ui.secureShellKeysTab);
    ui.publicKeyTextEdit->setReadOnly(true);

    /* share public key */
    connect(ui.sharePublicKeyButton, SIGNAL(clicked()), this,
            SLOT(sharePublicKeyClicked()));
    connect(this, SIGNAL(sharePublicKey(QString)), p,
            SIGNAL(sharePublicKey(QString)));
    connect(ui.exportPublicKeyToFileButton, SIGNAL(clicked()), this,
            SLOT(exportPublicKeyToFile()));

    /* studies stacked widget */
    ui.studiesStackedWidget->setCurrentWidget(ui.studyInfoPage);

    /* link study page */
    connect(ui.experimentsUrlEdit, SIGNAL(editingFinished()), this,
            SLOT(experimentsUrlEditingFinished()));
    connect(ui.experimentsUrlEdit, SIGNAL(editingFinished()), this,
            SLOT(validateStudyForm()));
    connect(ui.resultsUrlEdit, SIGNAL(editingFinished()), this,
            SLOT(validateStudyForm()));
    connect(ui.experimentsBranchComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(validateStudyForm()));
    connect(ui.resultsBranchEdit, SIGNAL(editingFinished()), this,
            SLOT(validateStudyForm()));
    connect(ui.studyNameEdit, SIGNAL(editingFinished()), this,
            SLOT(validateStudyForm()));
    connect(ui.uploadResultsCheckBox, SIGNAL(stateChanged(int)), this,
            SLOT(validateStudyForm()));
    connect(ui.linkStudyButton, SIGNAL(clicked()), this,
            SLOT(linkStudyClicked()));
    connect(this,
            SIGNAL(linkStudy(QString, QString, QString, QString, QString)), p,
            SIGNAL(linkStudy(QString, QString, QString, QString, QString)));
    connect(this, SIGNAL(experimentsUrlSet(QString)), p,
            SIGNAL(experimentsUrlSet(QString)));

    /* study info page */
    connect(ui.studiesComboBox, SIGNAL(activated(int)), this,
            SLOT(selectedStudyChanged(int)));
    connect(ui.activateStudyButton, SIGNAL(clicked()), this,
            SLOT(activateStudyClicked()));
    connect(this, SIGNAL(activateStudy(QString)), p,
            SIGNAL(activateStudy(QString)));
    connect(ui.startStudyButton, SIGNAL(clicked()), this,
            SLOT(startStudyClicked()));
    connect(this, SIGNAL(startActiveStudy()), p, SIGNAL(startActiveStudy()));
    connect(ui.pauseStudyButton, SIGNAL(clicked()), this,
            SLOT(pauseStudyClicked()));
    connect(this, SIGNAL(pauseActiveStudy()), p, SIGNAL(pauseActiveStudy()));
    connect(ui.syncStudyButton, SIGNAL(clicked()), this,
            SLOT(syncStudyClicked()));
    connect(this, SIGNAL(syncActiveStudy()), p, SIGNAL(syncActiveStudy()));
    connect(ui.deleteStudyButton, SIGNAL(clicked()), this,
            SLOT(deleteStudyClicked()));
    connect(this, SIGNAL(deleteStudy(QString)), p,
            SIGNAL(deleteStudy(QString)));
    connect(ui.newStudyButton, SIGNAL(clicked()), this,
            SLOT(newStudyClicked()));
}
void StudyDialogPrivate::tabWidgetCurrentChanged()
{

    if (ui.tabWidget->currentWidget() == ui.secureShellKeysTab)
        ui.buttonsStackedWidget->setCurrentWidget(ui.securityButtons);
    else
        ui.buttonsStackedWidget->setCurrentWidget(ui.studiesButtons);
}
void StudyDialogPrivate::sharePublicKeyClicked()
{
    QString url = MainConfigFileParser::Get().data().studyShareUrl();
    if (url.isEmpty())
#ifdef Q_OS_ANDROID
        Q_EMIT sharePublicKey(ui.publicKeyTextEdit->toPlainText());
#else
        return;
#endif
    else
        Q_EMIT sharePublicKey(
            url + QL1S("?publicKey=") +
            QUrl::toPercentEncoding(ui.publicKeyTextEdit->toPlainText()));
}

void StudyDialogPrivate::exportPublicKeyToFile()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(
        QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .first());
#ifdef Q_OS_ANDROID
    dialog.selectFile(QL1S("apex-") +
                      android::ApexAndroidBridge::getDeviceSerialNumber() +
                      QL1S(".pub"));
#else
    dialog.selectFile(QL1S("apex-") + ApexTools::getUser() + QL1S(".pub"));
#endif
    ApexTools::expandWidgetToWindow(&dialog);
    if (dialog.exec() == QDialog::Accepted) {
        QFile file(dialog.selectedFiles().first());
        file.open(QIODevice::WriteOnly);
        file.write(ui.publicKeyTextEdit->toPlainText().toUtf8());
    }
}

void StudyDialogPrivate::activateStudyClicked()
{
    ui.studiesButtons->setEnabled(false);
    ui.activeStudyStatusMessageGroupBox->show();
    Q_EMIT activateStudy(ui.studiesComboBox->currentText());
}

void StudyDialogPrivate::startStudyClicked()
{
    if (!ui.studiesComboBox->currentData().toBool())
        qCWarning(APEX_RS,
                  "Trying to start non-active study %s, this shouldn't happen.",
                  qPrintable(ui.studiesComboBox->currentText()));
    Q_EMIT startActiveStudy();
}

void StudyDialogPrivate::pauseStudyClicked()
{
    if (!ui.studiesComboBox->currentData().toBool())
        qCWarning(APEX_RS,
                  "Trying to pause non-active study %s, this shouldn't happen.",
                  qPrintable(ui.studiesComboBox->currentText()));
    ui.studiesButtons->setEnabled(false);
    Q_EMIT pauseActiveStudy();
}

void StudyDialogPrivate::syncStudyClicked()
{
    if (!ui.studiesComboBox->currentData().toBool())
        qCWarning(APEX_RS,
                  "Trying to sync non-active study %s, this shouldn't happen.",
                  qPrintable(ui.studiesComboBox->currentText()));
    ui.studiesButtons->setEnabled(false);
    Q_EMIT syncActiveStudy();
}

void StudyDialogPrivate::deleteStudyClicked()
{
    if (QMessageBox::Yes ==
        QMessageBox::question(
            p, tr("Delete study %1?").arg(ui.studiesComboBox->currentText()),
            tr("Are you sure?"))) {
        experimentsFSModel.reset(new QFileSystemModel);
        experimentsFSModel->setRootPath(rootPath);
        ui.experimentsTreeView->setModel(experimentsFSModel.data());
        ui.experimentsTreeView->setRootIndex(
            experimentsFSModel->index(rootPath));
        ui.experimentsTreeView->setEnabled(false);
        Q_EMIT deleteStudy(ui.studiesComboBox->currentText());
    }
}

void StudyDialogPrivate::selectedStudyChanged(int index)
{
    if (index == -1) {
        ui.experimentsTreeView->setEnabled(false);
        ui.activeStudyStatusMessageGroupBox->hide();
        ui.activateStudyButton->hide();
        ui.startStudyButton->hide();
        ui.pauseStudyButton->hide();
        ui.syncStudyButton->hide();
        ui.deleteStudyButton->hide();
        return;
    }

    ui.tabWidget->setCurrentWidget(ui.studiesTab);
    ui.studiesStackedWidget->setCurrentWidget(ui.studyInfoPage);
    ui.deleteStudyButton->show();
    ui.newStudyButton->show();
    if (!ui.studiesComboBox->currentData().toBool()) {
        ui.activateStudyButton->show();
        ui.startStudyButton->hide();
        ui.pauseStudyButton->hide();
        ui.syncStudyButton->hide();
        ui.activeStudyStatusMessageGroupBox->hide();
    } else {
        ui.activateStudyButton->hide();
        ui.activeStudyStatusMessageGroupBox->show();
        ui.startStudyButton->show();
        ui.pauseStudyButton->show();
        ui.syncStudyButton->show();
    }

    experimentsFSModel.reset(new QFileSystemModel);
    QString path = QDir(rootPath).filePath(ui.studiesComboBox->currentText() +
                                           QL1S("/experiments"));
    experimentsFSModel->setRootPath(path);
    ui.experimentsTreeView->setModel(experimentsFSModel.data());
    ui.experimentsTreeView->setRootIndex(experimentsFSModel->index(path));
    for (int i = 1; i < experimentsFSModel->columnCount(); ++i)
        ui.experimentsTreeView->hideColumn(i);
    ui.experimentsTreeView->setEnabled(true);
}

void StudyDialogPrivate::newStudyClicked()
{
    ui.studiesStackedWidget->setCurrentWidget(ui.linkStudyPage);
    ui.studyNameEdit->setEnabled(true);
    ui.experimentsUrlEdit->setEnabled(true);
    ui.resultsUrlEdit->setEnabled(true);
    ui.resultsBranchEdit->setEnabled(true);
    ui.uploadResultsCheckBox->setEnabled(true);

    ui.newStudyButton->hide();
    ui.studyNameEdit->clear();
    ui.experimentsUrlEdit->clear();
    ui.experimentsBranchComboBox->clear();
    ui.resultsUrlEdit->clear();
    ui.resultsBranchEdit->clear();
    ui.linkStudyTextBrowser->clear();

    ui.experimentsBranchComboBox->setEnabled(false);
    ui.linkStudyAdvancedForm->hide();
    ui.linkStudyStackedWidget->setCurrentWidget(ui.linkStudyButtonPage);
}

void StudyDialogPrivate::experimentsUrlEditingFinished()
{
    if (ui.studyNameEdit->text().isEmpty())
        ui.studyNameEdit->setText(
            ui.experimentsUrlEdit->text().split('/').last().split('.').first());
    if (ui.resultsUrlEdit->text().isEmpty())
        ui.resultsUrlEdit->setText(ui.experimentsUrlEdit->text());
    ui.linkStudyTextBrowser->append(QSL("Fetching branches."));
    if (!ui.experimentsUrlEdit->text().isEmpty())
        Q_EMIT experimentsUrlSet(ui.experimentsUrlEdit->text());
}

void StudyDialogPrivate::validateStudyForm()
{
    ui.linkStudyTextBrowser->clear();

    bool valid = true;
    valid &= validateWithMessage(!ui.studyNameEdit->text().isEmpty(),
                                 QSL("Study name is empty."));
    valid &= validateWithMessage(!ui.experimentsUrlEdit->text().isEmpty(),
                                 QSL("Experiment url is empty."));

    valid &=
        validateWithMessage(ui.experimentsBranchComboBox->currentIndex() != -1,
                            QSL("No experiment branch selected."));
    valid &= validateWithMessage(
        ui.studiesComboBox->findText(ui.studyNameEdit->text()) == -1,
        QSL("Study with this name already exists"));

    if (ui.uploadResultsCheckBox->isChecked()) {
        ui.linkStudyAdvancedForm->show();
        valid &= validateWithMessage(!ui.resultsUrlEdit->text().isEmpty(),
                                     QSL("Results url is empty."));
        valid &= validateWithMessage(!ui.resultsBranchEdit->text().isEmpty(),
                                     QSL("Results branch is empty."));
        valid &= validateWithMessage(
            ui.resultsBranchEdit->text() !=
                ui.experimentsBranchComboBox->currentText(),
            QSL("Results branch equals experiments branch."));

        if (!ui.resultsBranchEdit->text().isEmpty() &&
            ui.resultsBranchEdit->text() ==
                ui.experimentsBranchComboBox->currentText()) {
            ui.linkStudyTextBrowser->append(
                QSL("Results branch same as experiment branch."));
        }

        if (!ui.resultsBranchEdit->text().isEmpty() &&
            ui.experimentsUrlEdit->text() == ui.resultsUrlEdit->text() &&
            ui.experimentsBranchComboBox->findText(
                ui.resultsBranchEdit->text()) != -1) {
            ui.linkStudyTextBrowser->append(
                QString::fromLatin1(
                    "<font color=\"purple\">Results branch %1 already exists! "
                    "Future results will be mixed with existing ones!</font>")
                    .arg(ui.resultsBranchEdit->text()));
        }
    } else {
        ui.linkStudyAdvancedForm->hide();
    }

    if (valid)
        ui.linkStudyTextBrowser->append(
            QSL("<font color=\"green\">Valid configuration.</font>"));
    else
        ui.linkStudyAdvancedForm->show();

    ui.linkStudyButton->setEnabled(valid);
}

bool StudyDialogPrivate::validateWithMessage(bool valid,
                                             const QString &errorMessage)
{
    if (!valid)
        ui.linkStudyTextBrowser->append(QL1S("<font color=\"red\">") +
                                        errorMessage + QL1S("</font>"));
    return valid;
}

void StudyDialogPrivate::linkStudyClicked()
{
    ui.linkStudyButton->setEnabled(false);
    ui.studyNameEdit->setEnabled(false);
    ui.experimentsUrlEdit->setEnabled(false);
    ui.resultsUrlEdit->setEnabled(false);
    ui.experimentsBranchComboBox->setEnabled(false);
    ui.resultsBranchEdit->setEnabled(false);
    ui.uploadResultsCheckBox->setEnabled(false);
    ui.closeButton->setEnabled(false);
    ui.linkStudyProgressBar->setMaximum(0);
    ui.linkStudyProgressBar->setMinimum(0);
    ui.linkStudyStackedWidget->setCurrentWidget(ui.linkStudyProgressPage);
    Q_EMIT linkStudy(
        ui.studyNameEdit->text(), ui.experimentsUrlEdit->text(),
        ui.experimentsBranchComboBox->currentText(),
        ui.uploadResultsCheckBox->isChecked() ? ui.resultsUrlEdit->text()
                                              : QString(),
        ui.uploadResultsCheckBox->isChecked() ? ui.resultsBranchEdit->text()
                                              : QString());
}

/* StudyDialogPrivate============================================ */

StudyDialog::StudyDialog(QWidget *parent)
    : QDialog(parent), d(new StudyDialogPrivate(this))
{
    ApexTools::expandWidgetToWindow(this);
}

StudyDialog::~StudyDialog()
{
}

void StudyDialog::setPublicKey(const QString &key)
{
    d->ui.publicKeyTextEdit->setPlainText(key);
}

void StudyDialog::linkStudySetExperimentsBranches(const QStringList &branches)
{
    d->ui.experimentsBranchComboBox->clear();
    if (!branches.isEmpty()) {
        d->ui.experimentsBranchComboBox->addItems(branches);
        int index = d->ui.experimentsBranchComboBox->findText(QSL("master"));
        if (index != -1)
            d->ui.experimentsBranchComboBox->setCurrentIndex(index);
        d->ui.experimentsBranchComboBox->setEnabled(true);
    }
}

void StudyDialog::linkStudySetResultsBranch(const QString &branch)
{
    d->ui.resultsBranchEdit->setText(branch);
}

void StudyDialog::linkStudyStatusUpdate(const QString &statusUpdate)
{
    d->ui.linkStudyTextBrowser->append(statusUpdate);
}

void StudyDialog::linkStudyProgress(int received, int total)
{
    d->ui.linkStudyProgressBar->setMaximum(total);
    d->ui.linkStudyProgressBar->setMinimum(0);
    d->ui.linkStudyProgressBar->setValue(received);
}

void StudyDialog::linkStudyFailed()
{
    linkStudyStatusUpdate(QSL("Link study failed. Click link to retry."));
    d->ui.linkStudyButton->setEnabled(true);
    d->ui.studyNameEdit->setEnabled(true);
    d->ui.experimentsUrlEdit->setEnabled(true);
    d->ui.resultsUrlEdit->setEnabled(true);
    d->ui.experimentsBranchComboBox->setEnabled(true);
    d->ui.resultsBranchEdit->setEnabled(true);
    d->ui.closeButton->setEnabled(true);
    d->ui.linkStudyStackedWidget->setCurrentWidget(d->ui.linkStudyButtonPage);
}

void StudyDialog::updateStudies(const QStringList &studies,
                                const QString &activeStudy,
                                const QString &statusMessage)
{
    d->ui.studiesComboBox->clear();
    Q_FOREACH (const QString &study, studies) {
        if (study == activeStudy)
            d->ui.studiesComboBox->addItem(study, true);
        else
            d->ui.studiesComboBox->addItem(study, false);
    }
    d->ui.studiesStackedWidget->setCurrentWidget(d->ui.studyInfoPage);
    int index = d->ui.studiesComboBox->findData(true);
    if (index != -1)
        d->ui.studiesComboBox->setCurrentIndex(index);
    d->selectedStudyChanged(
        studies.isEmpty() ? -1 : d->ui.studiesComboBox->currentIndex());
    d->ui.activeStudyStatusTextBrowser->setText(statusMessage);
    d->ui.closeButton->setEnabled(true);
    d->ui.studiesButtons->setEnabled(true);
}

void StudyDialog::updateStudyStatusMessage(const QString &statusMessage)
{
    d->ui.activeStudyStatusTextBrowser->setText(statusMessage);
}

void StudyDialog::setRootPath(const QString &path)
{
    d->rootPath = path;
}
}

#include "studydialog.moc"
