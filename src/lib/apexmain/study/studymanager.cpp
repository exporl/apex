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

#include "studymanager.h"

#include "apexcontrol.h"
#include "gui/studydialog.h"
#include "manageddirectory.h"

#ifdef Q_OS_ANDROID
#include "../apexandroidnative.h"
#endif

#include "apextools/settingsfactory.h"

#include "apextools/apextools.h"
#include "apextools/desktoptools.h"
#include "apextools/exceptions.h"

#include <QDir>
#include <QHostInfo>
#include <QMessageBox>
#include <QPointer>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

namespace apex
{

Q_GLOBAL_STATIC(StudyManager, studyManager)

class StudyManagerPrivate : public QObject
{
    Q_OBJECT
public:
    StudyManagerPrivate(const QString &rootPath,
                        const QString &resultsWorkdirRootPath,
                        const QString &keyPath);

    void setActiveStudy(const QString &studyName);
    void writeStudies();
    void connectActiveStudySignals();
    void disconnectActiveStudySignals();

public Q_SLOTS:
    void updateActiveStudyExperimentsDone();
    void updateActiveStudyExperimentsFailed();
    void updateActiveStudyResultsDone();
    void updateActiveStudyResultsFailed();

Q_SIGNALS:
    void studiesUpdated(const QStringList &studies, const QString &activeStudy,
                        const QString &activeStudyStatusMessage);

public:
    enum State { IDLE, LINKING, PAUSING, SYNCING };

    QString rootPath;
    QString resultsWorkdirRootPath;
    QString keyPath;
    StudyManagerPrivate::State currentState;
    QSharedPointer<Study> activeStudy;
    QMap<QString, QSharedPointer<Study>> studies;
    QScopedPointer<StudyDialog> studyDialog;
};

StudyManagerPrivate::StudyManagerPrivate(const QString &rootPath,
                                         const QString &resultsWorkdirRootPath,
                                         const QString &keyPath)
    : rootPath(rootPath),
      resultsWorkdirRootPath(resultsWorkdirRootPath),
      keyPath(keyPath),
      currentState(StudyManagerPrivate::IDLE)
{
    QDir keyDirectory(keyPath);
    keyDirectory.mkpath(QSL("."));
    if (!keyDirectory.exists(keyDirectory.filePath(QSL("id_rsa")))) {
        try {
#ifdef Q_OS_ANDROID
            QString serial =
                android::ApexAndroidBridge::getDeviceSerialNumber();
            ApexTools::generateKeyPair(
                keyDirectory.filePath(QSL("id_rsa")),
                QString::fromLatin1("%1@%2").arg(serial).arg(serial));

#else
            ApexTools::generateKeyPair(keyDirectory.filePath(QSL("id_rsa")),
                                       QString::fromLatin1("%1@%2")
                                           .arg(ApexTools::getUser())
                                           .arg(QHostInfo::localHostName()));
#endif
        } catch (std::exception &e) {
            qCWarning(APEX_RS, "Unable to generate keypair %s.", e.what());
        }
    }

    QScopedPointer<QSettings> settings(SettingsFactory().createSettings());
    settings->beginGroup(QSL("StudyManager"));
    int studyCount = settings->beginReadArray(QSL("studies"));
    for (int i = 0; i < studyCount; ++i) {
        settings->setArrayIndex(i);
        const QString name = settings->value(QSL("name")).toString();
        studies.insert(
            name, QSharedPointer<Study>(new Study(
                      name, settings->value(QSL("experimentsUrl")).toString(),
                      settings->value(QSL("experimentsBranch")).toString(),
                      settings->value(QSL("resultsUrl")).toString(),
                      settings->value(QSL("resultsBranch")).toString(),
                      rootPath, resultsWorkdirRootPath, keyPath)));
    }
    settings->endArray();
    QString current(settings->value(QSL("activeStudy")).toString());
    if (!current.isEmpty())
        setActiveStudy(current);
    settings->endGroup();
}

void StudyManagerPrivate::setActiveStudy(const QString &studyName)
{
    if (!studyName.isEmpty() && !studies.contains(studyName))
        qCCritical(APEX_RS,
                   "Trying to set study %s as active, but study does not exist",
                   qPrintable(studyName));

    if (activeStudy)
        disconnectActiveStudySignals();
    if (studyName.isEmpty())
        activeStudy.clear();
    else
        activeStudy = studies.value(studyName);
    writeStudies();
    connectActiveStudySignals();
}

void StudyManagerPrivate::writeStudies()
{
    QScopedPointer<QSettings> settings(SettingsFactory().createSettings());
    settings->beginGroup(QSL("StudyManager"));
    settings->remove(QSL(""));
    settings->setValue(QSL("activeStudy"),
                       activeStudy ? activeStudy->name() : QString());
    settings->beginWriteArray(QSL("studies"));
    int index = 0;
    Q_FOREACH (const QString &key, studies.keys()) {
        settings->setArrayIndex(index++);
        settings->setValue(QSL("name"), studies.value(key)->name());
        settings->setValue(QSL("experimentsUrl"),
                           studies.value(key)->experimentsUrl());
        settings->setValue(QSL("experimentsBranch"),
                           studies.value(key)->experimentsBranch());
        settings->setValue(QSL("resultsUrl"), studies.value(key)->resultsUrl());
        settings->setValue(QSL("resultsBranch"),
                           studies.value(key)->resultsBranch());
    }
    settings->endArray();
    settings->endGroup();
}

void StudyManagerPrivate::connectActiveStudySignals()
{
    if (!activeStudy)
        return;

    connect(activeStudy.data(), SIGNAL(updateExperimentsDone()), this,
            SLOT(updateActiveStudyExperimentsDone()));
    connect(activeStudy.data(), SIGNAL(updateExperimentsFailed()), this,
            SLOT(updateActiveStudyExperimentsFailed()));
    if (activeStudy->isPrivate()) {
        connect(activeStudy.data(), SIGNAL(updateResultsDone()), this,
                SLOT(updateActiveStudyResultsDone()));
        connect(activeStudy.data(), SIGNAL(updateResultsFailed()), this,
                SLOT(updateActiveStudyResultsFailed()));
    }
}

void StudyManagerPrivate::disconnectActiveStudySignals()
{
    disconnect(this, SLOT(updateActiveStudyExperimentsDone()));
    disconnect(this, SLOT(updateActiveStudyExperimentsFailed()));
    if (activeStudy->isPrivate()) {
        disconnect(this, SLOT(updateActiveStudyResultsDone()));
        disconnect(this, SLOT(updateActiveStudyResultsFailed()));
    }
}

void StudyManagerPrivate::updateActiveStudyExperimentsDone()
{
#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::sendToast(tr("Experiments update successful"));
#endif
    switch (currentState) {
    case StudyManagerPrivate::LINKING: {
        QPointer<Study> study(qobject_cast<Study *>(sender()));
        if (study) {
            setActiveStudy(study->name());
            writeStudies();
            Q_EMIT studiesUpdated(studies.keys(), activeStudy->name(),
                                  activeStudy->statusMessage());
        }
        currentState = StudyManagerPrivate::IDLE;
        break;
    }
    case StudyManagerPrivate::SYNCING: {
        if (activeStudy)
            Q_EMIT studiesUpdated(studies.keys(), activeStudy->name(),
                                  activeStudy->statusMessage());
        currentState = StudyManagerPrivate::IDLE;
        break;
    }
    case StudyManagerPrivate::PAUSING:
        break;
    case StudyManagerPrivate::IDLE:
        break;
    default:
        break;
    }
}

void StudyManagerPrivate::updateActiveStudyExperimentsFailed()
{
#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::sendToast(
        tr("Experiments update failed, connected to WiFi?"));
#endif
    switch (currentState) {
    case StudyManagerPrivate::LINKING: {
        QPointer<Study> study(qobject_cast<Study *>(sender()));
        if (study) {
            /* Copy study name because QMap::remove takes const& and after
             * removing study continues looking for other values with that
             * (now destructed) key.
             */
            QString studyName = study->name();
            studies.remove(studyName);
            if (!QDir(QDir(rootPath).filePath(studyName)).removeRecursively())
                qCWarning(APEX_RS, "Unable to remove study %s on disk.",
                          qPrintable(studyName));
        }
        if (studyDialog)
            studyDialog->linkStudyFailed();
        currentState = StudyManagerPrivate::IDLE;
        break;
    }
    case StudyManagerPrivate::SYNCING: {
        if (studyDialog)
            QMessageBox::warning(nullptr, QSL("Syncing active study failed"),
                                 QL1S("Unable to sync study. Press "
                                      "sync again to retry."));
        if (activeStudy)
            Q_EMIT studiesUpdated(studies.keys(), activeStudy->name(),
                                  activeStudy->statusMessage());

        currentState = StudyManagerPrivate::IDLE;
        break;
    }
    case StudyManagerPrivate::PAUSING:
        break;
    case StudyManagerPrivate::IDLE:
        break;
    default:
        break;
    }
}

void StudyManagerPrivate::updateActiveStudyResultsDone()
{
#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::sendToast(tr("Results upload successful"));
#endif
    switch (currentState) {
    case StudyManagerPrivate::SYNCING:
        break;
    case StudyManagerPrivate::LINKING:
        break;
    case StudyManagerPrivate::PAUSING:
        activeStudy.clear();
        writeStudies();
        Q_EMIT studiesUpdated(studies.keys(), QString(), QString());
        currentState = StudyManagerPrivate::IDLE;
        break;
    case StudyManagerPrivate::IDLE:
    default:
        break;
    }
}

void StudyManagerPrivate::updateActiveStudyResultsFailed()
{
#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::sendToast(
        tr("Results upload failed, connected to WiFi?"));
#endif
    switch (currentState) {
    case StudyManagerPrivate::LINKING:
        break;
    case StudyManagerPrivate::SYNCING:
        break;
    case StudyManagerPrivate::PAUSING: {
        currentState = StudyManagerPrivate::IDLE;
        if (studyDialog)
            QMessageBox::warning(
                nullptr, QSL("Results update failed"),
                QL1S("Unable to update results before pausing study. Activate "
                     "and pause again to retry."));
        break;
    }
    case StudyManagerPrivate::IDLE:
    default:
        break;
    }
}

/* StudyManagerPrivate================================================== */

StudyManager *StudyManager::instance()
{
    return studyManager;
}

StudyManager::StudyManager()
    : d(new StudyManagerPrivate(ApexPaths::GetStudyRootPath(),
                                ApexPaths::GetStudyRootResultsWorkdirPath(),
                                ApexPaths::GetStudyManagerDirectory()))
{
    connect(d.data(), SIGNAL(studiesUpdated(QStringList, QString, QString)),
            this, SIGNAL(studiesUpdated(QStringList, QString, QString)));
}

StudyManager::~StudyManager()
{
}

void StudyManager::setRootPath(const QString &path)
{
    d.reset(new StudyManagerPrivate(path, path, d->keyPath));
    connect(d.data(), SIGNAL(studiesUpdated(QStringList, QString, QString)),
            this, SIGNAL(studiesUpdated(QStringList, QString, QString)));
}

void StudyManager::showConfigurationDialog()
{
    QDir rootDirectory(d->rootPath);
    QDir keyDirectory(d->keyPath);
    QFile file(keyDirectory.filePath(QSL("id_rsa.pub")));
    if (!file.open(QIODevice::ReadOnly))
        qCWarning(APEX_RS, "Unable to open public key file");

    d->studyDialog.reset(new StudyDialog);
    d->studyDialog->setPublicKey(QString::fromUtf8(file.readAll()));
    connect(d->studyDialog.data(),
            SIGNAL(linkStudy(QString, QString, QString, QString, QString)),
            this, SLOT(linkStudy(QString, QString, QString, QString, QString)));
    connect(d->studyDialog.data(), SIGNAL(activateStudy(QString)), this,
            SLOT(setActiveStudy(QString)));
    connect(d->studyDialog.data(), SIGNAL(startActiveStudy()), this,
            SLOT(startActiveStudy()));
    connect(d->studyDialog.data(), SIGNAL(pauseActiveStudy()), this,
            SLOT(pauseActiveStudy()));
    connect(d->studyDialog.data(), SIGNAL(syncActiveStudy()), this,
            SLOT(syncActiveStudy()));
    connect(d->studyDialog.data(), SIGNAL(deleteStudy(QString)), this,
            SLOT(deleteStudy(QString)));
    connect(d->studyDialog.data(), SIGNAL(sharePublicKey(QString)), this,
            SLOT(sharePublicKey(QString)));
    connect(this, SIGNAL(studiesUpdated(QStringList, QString, QString)),
            d->studyDialog.data(),
            SLOT(updateStudies(QStringList, QString, QString)));
    connect(this, SIGNAL(studyStatusMessageUpdated(QString)),
            d->studyDialog.data(), SLOT(updateStudyStatusMessage(QString)));
    connect(d->studyDialog.data(), SIGNAL(experimentsUrlSet(QString)), this,
            SLOT(fetchRemoteBranches(QString)));
    d->studyDialog->setRootPath(rootDirectory.path());
    if (d->activeStudy)
        d->studyDialog->updateStudies(d->studies.keys(), d->activeStudy->name(),
                                      d->activeStudy->statusMessage());
    else
        d->studyDialog->updateStudies(d->studies.keys(), QString(), QString());
    d->studyDialog->exec();
    d->studyDialog.reset();
}

const QSharedPointer<Study> &StudyManager::activeStudy() const
{
    return d->activeStudy;
}

QStringList StudyManager::studies() const
{
    return d->studies.keys();
}

bool StudyManager::belongsToActiveStudy(const QString &path) const
{
    return d->activeStudy && d->activeStudy->belongsToStudy(path);
}

QString StudyManager::newExperiment(const QString &experimentFilePath) const
{
    if (!d->activeStudy)
        return QString();

    try {
        if (d->activeStudy->isPrivate() &&
            belongsToActiveStudy(experimentFilePath))
            return d->activeStudy->makeResultsFilePath(experimentFilePath);
        else
            return QString();
    } catch (std::exception &e) {
        qCWarning(APEX_RS,
                  "Unable to make resultsfilepath for experiment %s: %s",
                  qPrintable(experimentFilePath), e.what());
    }
    return QString();
}

void StudyManager::afterExperiment(const QString &resultsFilePath)
{
    if (!d->activeStudy)
        return;

    try {
        if (d->activeStudy->isPrivate() &&
            belongsToActiveStudy(resultsFilePath))
            d->activeStudy->addResult(resultsFilePath);
    } catch (std::exception &e) {
        qCWarning(APEX_RS, "Unable to add result to active study %s: %s",
                  qPrintable(d->activeStudy->name()), e.what());
    }
}

void StudyManager::setActiveStudy(const QString &studyName)
{
    if (d->activeStudy && studyName == d->activeStudy->name())
        return;
    d->setActiveStudy(studyName);
    syncActiveStudy();
}

void StudyManager::linkStudy(const QString &name, const QString &experimentsUrl,
                             const QString &experimentsBranch,
                             const QString &resultsUrl,
                             const QString &resultsBranch)
{
    if (d->studyDialog)
        d->studyDialog->linkStudyStatusUpdate(QSL("Attempting to link study"));
    if (d->studies.contains(name)) {
        if (d->studyDialog) {
            d->studyDialog->linkStudyStatusUpdate(
                QString::fromLatin1("Study with name %1 already exists")
                    .arg(name));
            d->studyDialog->linkStudyFailed();
        }
        return;
    }

    d->currentState = StudyManagerPrivate::LINKING;

    QSharedPointer<Study> newStudy(new Study(
        name, experimentsUrl, experimentsBranch, resultsUrl, resultsBranch,
        d->rootPath, d->resultsWorkdirRootPath, d->keyPath));
    d->studies.insert(name, newStudy);
    connect(newStudy.data(), SIGNAL(updateExperimentsDone()), d.data(),
            SLOT(updateActiveStudyExperimentsDone()));
    connect(newStudy.data(), SIGNAL(updateExperimentsFailed()), d.data(),
            SLOT(updateActiveStudyExperimentsFailed()));

    if (d->studyDialog) {
        d->studyDialog->linkStudyStatusUpdate(QSL("Pulling experiments."));
        connect(newStudy.data(), SIGNAL(updateExperimentsProgress(int, int)),
                d->studyDialog.data(), SLOT(linkStudyProgress(int, int)));
    }
    try {
        newStudy->updateExperiments();
        if (newStudy->isPrivate())
            newStudy->pullResults();
    } catch (std::exception &e) {
        qCWarning(APEX_RS, "Unable to synchronize experiments/results when "
                           "linking new study: %s.",
                  e.what());
        d->currentState = StudyManagerPrivate::IDLE;
        d->studies.remove(name);
        if (d->studyDialog)
            d->studyDialog->linkStudyFailed();
    }
}

void StudyManager::startActiveStudy()
{
    QMetaObject::invokeMethod(&ApexControl::Get(), "startPluginRunner",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QSL("study")));
    d->studyDialog.reset();
}

void StudyManager::pauseActiveStudy()
{
    if (!d->activeStudy) {
        qCWarning(APEX_RS, "Pause active study failed: no study is active.");
        Q_EMIT studiesUpdated(d->studies.keys(), QString(), QString());
        return;
    }
    if (d->currentState != StudyManagerPrivate::IDLE) {
        qCWarning(APEX_RS,
                  "Pause active study failed: other operation in progress.");
        return;
    }
    if (d->activeStudy->isPublic()) {
        d->setActiveStudy(QString());
        Q_EMIT studiesUpdated(d->studies.keys(), QString(), QString());
        return;
    }

    d->currentState = StudyManagerPrivate::PAUSING;
    Q_EMIT studyStatusMessageUpdated(
        QString::fromLatin1(
            "Attempting to send results before pausing study %1.")
            .arg(d->activeStudy->name()));
    try {
        d->activeStudy->updateResults();
    } catch (std::exception &e) {
        if (d->studyDialog)
            QMessageBox::warning(
                nullptr, QSL("Results update failed"),
                QL1S("Unable to update results before pausing study. Activate "
                     "and pause again to retry."));
        d->currentState = StudyManagerPrivate::IDLE;
        Q_EMIT studiesUpdated(d->studies.keys(), d->activeStudy->name(),
                              d->activeStudy->statusMessage());
    }
}

void StudyManager::syncActiveStudy()
{
    if (!d->activeStudy) {
        Q_EMIT studiesUpdated(d->studies.keys(), QString(), QString());
        return;
    }
    if (d->currentState != StudyManagerPrivate::IDLE) {
        Q_EMIT studiesUpdated(d->studies.keys(), QString(), QString());
        return;
    }

    d->currentState = StudyManagerPrivate::SYNCING;

    Q_EMIT studyStatusMessageUpdated(
        QString::fromLatin1("Attempting to sync study %1.")
            .arg(d->activeStudy->name()));
    try {
        d->activeStudy->updateExperiments();
        if (d->activeStudy->isPrivate())
            d->activeStudy->updateResults();
    } catch (std::exception &e) {
        if (d->studyDialog)
            QMessageBox::warning(
                nullptr, QSL("Syncing active study failed"),
                QL1S("Unable to sync study. Press sync again to retry."));
        d->currentState = StudyManagerPrivate::IDLE;
        Q_EMIT studiesUpdated(d->studies.keys(), d->activeStudy->name(),
                              d->activeStudy->statusMessage());
    }
}

void StudyManager::deleteStudy(const QString &studyName)
{
    if (d->currentState != StudyManagerPrivate::IDLE) {
        Q_EMIT studiesUpdated(d->studies.keys(), d->activeStudy->name(),
                              d->activeStudy->statusMessage());
        return;
    }

    d->setActiveStudy(QString());
    d->studies.remove(studyName);
    d->writeStudies();

    bool success =
        QDir(QDir(d->rootPath).filePath(studyName)).removeRecursively();
    if (!success)
        qCWarning(APEX_RS, "Unable to remove study %s on disk.",
                  qPrintable(studyName));
    Q_EMIT studiesUpdated(d->studies.keys(), QString(), QString());
}

void StudyManager::sharePublicKey(const QString &key)
{
#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::shareText(key);
#else
    DesktopTools::openUrl(key);
#endif
}

void StudyManager::fetchRemoteBranches(const QString &remote)
{
    try {
        QDir rootDirectory(d->rootPath);
        QDir keyDirectory(d->keyPath);
        d->studyDialog->linkStudySetExperimentsBranches(
            ManagedDirectory::lsRemote(remote,
                                       keyDirectory.filePath(QSL("id_rsa.pub")),
                                       keyDirectory.filePath(QSL("id_rsa"))));
        d->studyDialog->linkStudySetResultsBranch(
#ifdef Q_OS_ANDROID
            QString::fromLatin1("results-%1_%2")
                .arg(android::ApexAndroidBridge::getDeviceSerialNumber())
                .arg(android::ApexAndroidBridge::getDeviceSerialNumber()));
#else
            QString::fromLatin1("results-%1_%2")
                .arg(ApexTools::getUser())
                .arg(QHostInfo::localHostName()));
#endif

    } catch (std::exception &e) {
        d->studyDialog->linkStudyStatusUpdate(QString::fromLatin1(e.what()));
        d->studyDialog->linkStudySetExperimentsBranches(QStringList());
    }
}
} // namespace apex

#include "studymanager.moc"
