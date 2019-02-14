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

#include "manageddirectory.h"

#ifdef Q_OS_ANDROID
#include "../apexandroidnative.h"
#endif

#include "apextools/exceptions.h"

#include <git2.h>

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QSharedPointer>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QThread>

namespace apex
{

static void fillSSLCertificates()
{
#ifdef Q_OS_ANDROID
    QDir certsDir(
        apex::android::ApexAndroidBridge::getSSLCertificateDirectory());
    Q_FOREACH (const QString &obj,
               certsDir.entryList(QDir::NoDotAndDotDot | QDir::Files)) {
        if (git_libgit2_opts(
                GIT_OPT_SET_SSL_CERT_LOCATIONS,
                QFile::encodeName(certsDir.filePath(obj)).constData(),
                NULL) < 0)
            qCWarning(APEX_RS, "Unable to add certificate %s.",
                      qPrintable(obj));
    };

#endif
}

struct gitStrArray {
    ~gitStrArray()
    {
        git_strarray_free(&array);
    }

    git_strarray *operator&()
    {
        return &array;
    }

    git_strarray array;
};

class ManagedDirectoryWorker : public QObject
{
    Q_OBJECT

public:
    bool isClean();
    bool isOpen();
    bool hasLocalRemote();
    int commitsAheadOrigin();
    int totalCommitCount();
    QString lastCommitMessage();

    void init(const QDir &repoPath, const QDir &workdirPath, bool bare = false);
    void open(const QDir &repoPath);
    void close();

    void setRemote(const QString &url, const QString &branchName);
    void setAuthor(const QString &name, const QString &email);
    void setKeyPaths(const QString &publicKeyPath,
                     const QString &privateKeyPath);

    void add(const QString &path);
    void commit();

    static int acquireCredentialsCallback(git_cred **cred, const char *url,
                                          const char *username,
                                          unsigned allowedTypes, void *payload);

public Q_SLOTS:
    void push();
    void pull();
    void fetch();
    void checkout();
    void retryLastAction();

Q_SIGNALS:
    void pullDone();
    void pullFailed();
    void pushDone();
    void pushFailed();
    void retryLastActionDone();
    void progress(int received, int total);
    void error(const QString &error);

private:
    void doFetch();
    void doCheckout();

    static int progressCallback(const git_transfer_progress *stats,
                                void *userData);
    static int pushProgressCallback(unsigned current, unsigned total,
                                    size_t bytes, void *userData);

    void setConfigValue(const QString &key, const QString &value,
                        const QString &category = QL1S("userdata"));
    QString getConfigValue(const QString &key,
                           const QString &category = QL1S("userdata"));

    QSharedPointer<git_remote> findRemote();
    QSharedPointer<git_signature> signature();

    void handleResult(int result, const QString &message = QString());

    QMutex repoMutex;
    QSharedPointer<git_repository> repo;
    QDir repoPath;
    QDir workdirPath;

    QString publicKey;
    QString privateKey;
};

bool ManagedDirectoryWorker::isClean()
{
    QMutexLocker locker(&repoMutex);
    return getConfigValue(QSL("repostate")) == QL1S("clean");
}

bool ManagedDirectoryWorker::isOpen()
{
    QMutexLocker locker(&repoMutex);
    return !repo.isNull();
}

bool ManagedDirectoryWorker::hasLocalRemote()
{
    QMutexLocker locker(&repoMutex);
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    QSharedPointer<git_remote> remote = findRemote();
    return QFile::exists(QString::fromUtf8(git_remote_url(remote.data())));
}

int ManagedDirectoryWorker::commitsAheadOrigin()
{
    QMutexLocker locker(&repoMutex);
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    int result = git_repository_head_unborn(repo.data());
    if (result < 0)
        throw ApexStringException(tr("Unable to determine if HEAD is unborn"));
    if (result)
        return 0;

    git_oid local, upstream;
    handleResult(git_reference_name_to_id(&local, repo.data(), "HEAD"),
                 tr("Unable to lookup head oid"));

    QSharedPointer<git_remote> remote = findRemote();
    if (QFile::exists(QString::fromUtf8(git_remote_url(remote.data()))))
        throw ApexStringException(
            tr("Unable to determine commits ahead for local remote"));

    /* Only one fetchspec is defined at any given time */
    const git_refspec *fetchSpec = nullptr;
    for (size_t i = 0; i < git_remote_refspec_count(remote.data()); ++i) {
        const git_refspec *spec = git_remote_get_refspec(remote.data(), i);
        if (git_refspec_direction(spec) == GIT_DIRECTION_FETCH)
            fetchSpec = spec;
    }
    if (!fetchSpec)
        throw ApexStringException(tr("Unable to find fetch refspec"));

    result = git_reference_name_to_id(&upstream, repo.data(),
                                      git_refspec_dst(fetchSpec));
    /* If no remote is found, assume remote ref hasn't been created yet */
    if (result == GIT_ENOTFOUND) {
        locker.unlock();
        return totalCommitCount();
    } else if (result < 0) {
        throw ApexStringException(tr("Unable to lookup upstream oid"));
    }

    size_t ahead = 0;
    size_t behind = 0;
    handleResult(
        git_graph_ahead_behind(&ahead, &behind, repo.data(), &local, &upstream),
        tr("Unable to determine commits ahead/behind remote"));
    return (int)ahead;
}

int ManagedDirectoryWorker::totalCommitCount()
{
    QMutexLocker locker(&repoMutex);
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    git_revwalk *revWalkPtr = nullptr;
    handleResult(git_revwalk_new(&revWalkPtr, repo.data()),
                 tr("Unable to start revwalk"));
    QSharedPointer<git_revwalk> revWalk(revWalkPtr, git_revwalk_free);
    handleResult(git_revwalk_push_head(revWalk.data()),
                 tr("Unable to start revwalk at HEAD"));
    handleResult(git_revwalk_hide_glob(revWalk.data(), "tags/*"),
                 tr("Unable to hide tags from revwalk"));

    git_oid oid;
    int count = 0;
    while (git_revwalk_next(&oid, revWalk.data()) == 0)
        count++;
    return count;
}

QString ManagedDirectoryWorker::lastCommitMessage()
{
    QMutexLocker locker(&repoMutex);
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    git_oid commitId;
    handleResult(git_reference_name_to_id(&commitId, repo.data(), "HEAD"),
                 tr("Unable to lookup head oid"));
    git_commit *commitPtr = nullptr;
    handleResult(git_commit_lookup(&commitPtr, repo.data(), &commitId),
                 tr("Unable to lookup commit"));
    QSharedPointer<git_commit> commit(commitPtr, git_commit_free);

    return QString::fromLatin1("Commit: %1\n"
                               "Date: %2\n\n"
                               "Message: %3")
        .arg(QString::fromUtf8(git_oid_tostr_s(&commitId)))
        .arg(QDateTime::fromMSecsSinceEpoch(1000 *
                                            git_commit_time(commit.data()))
                 .toString())
        .arg(QString::fromUtf8(git_commit_message(commit.data())));
}

void ManagedDirectoryWorker::init(const QDir &repoPath, const QDir &workdirPath,
                                  bool bare)
{
    QMutexLocker locker(&repoMutex);

    this->repoPath = repoPath;
    this->workdirPath = workdirPath;

    git_repository_init_options initopts;
    handleResult(git_repository_init_init_options(
                     &initopts, GIT_REPOSITORY_INIT_OPTIONS_VERSION),
                 tr("Unable to initialize init options"));

    QByteArray workdirPathByteArray = QFile::encodeName(workdirPath.path());
    initopts.workdir_path = workdirPathByteArray.data();

    initopts.flags |= GIT_REPOSITORY_INIT_MKPATH;
    if (bare) {
        initopts.flags |= GIT_REPOSITORY_INIT_BARE;
    }

    git_repository *repoPtr = nullptr;
    handleResult(git_repository_init_ext(
                     &repoPtr, QFile::encodeName(repoPath.path()), &initopts),
                 tr("Unable to initialize repository"));
    repo = QSharedPointer<git_repository>(repoPtr, git_repository_free);

    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryWorker::open(const QDir &repoPath)
{
    QMutexLocker locker(&repoMutex);
    qCWarning(APEX_RS, "apex: opening!");
    this->repoPath = repoPath;
    git_repository *repoPtr = nullptr;
    handleResult(
        git_repository_open(&repoPtr, QFile::encodeName(repoPath.path())),
        tr("Unable to open repository %1, does it exists?")
            .arg(repoPath.path()));
    repo = QSharedPointer<git_repository>(repoPtr, git_repository_free);
    if (getConfigValue(QSL("repostate")) != QL1S("clean"))
        qCWarning(APEX_RS) << "Warning, repo in dirty state";
}

void ManagedDirectoryWorker::close()
{
    QMutexLocker locker(&repoMutex);
    repo.clear();
}

void ManagedDirectoryWorker::setRemote(const QString &url,
                                       const QString &branchName)
{
    QMutexLocker locker(&repoMutex);
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    /* delete all other remotes, this directory should only have one remote */
    gitStrArray remotes;
    handleResult(git_remote_list(&remotes, repo.data()),
                 tr("Unable to list remotes"));

    for (unsigned i = 0; i < remotes.array.count; ++i)
        handleResult(git_remote_delete(repo.data(), remotes.array.strings[i]),
                     tr("Error deleting remote"));

    QString name = QSL("origin");
    git_remote *remotePtr = nullptr;
    QString fetchSpec = QString::fromLatin1("+refs/heads/%1:refs/remotes/%2/%1")
                            .arg(branchName)
                            .arg(name);
    handleResult(git_remote_create_with_fetchspec(&remotePtr, repo.data(),
                                                  name.toUtf8(), url.toUtf8(),
                                                  fetchSpec.toUtf8()),
                 tr("error creating remote"));
    QSharedPointer<git_remote> remote(remotePtr, git_remote_free);

    QString pushSpec =
        QString::fromLatin1("+refs/heads/%1:refs/heads/%1").arg(branchName);
    handleResult(git_remote_add_push(repo.data(),
                                     git_remote_name(remote.data()),
                                     pushSpec.toUtf8()),
                 tr("Unable to add push refspec"));

    handleResult(
        git_repository_set_head(
            repo.data(),
            QString::fromLatin1("refs/heads/%1").arg(branchName).toUtf8()),
        tr("Unable to set repository head"));
}

void ManagedDirectoryWorker::setAuthor(const QString &name,
                                       const QString &email)
{
    QMutexLocker locker(&repoMutex);
    setConfigValue(QSL("name"), name, QSL("user"));
    setConfigValue(QSL("email"), email, QSL("user"));
}

void ManagedDirectoryWorker::setKeyPaths(const QString &publicKeyPath,
                                         const QString &privateKeyPath)
{
    QMutexLocker locker(&repoMutex);
    publicKey = publicKeyPath;
    privateKey = privateKeyPath;
}

void ManagedDirectoryWorker::add(const QString &path)
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    if (!workdirPath.exists(path))
        throw ApexStringException(
            tr("Unable to add nonexistent file %1 to directory").arg(path));

    QMutexLocker locker(&repoMutex);
    setConfigValue(QSL("repostate"), QSL("dirty-add"));
    setConfigValue(QSL("addpath"), path);

    git_index *indexPtr = nullptr;
    handleResult(git_repository_index(&indexPtr, repo.data()),
                 tr("Unable to open repo index"));
    QSharedPointer<git_index> index(indexPtr, git_index_free);
    handleResult(git_index_add_bypath(
                     index.data(),
                     QFile::encodeName(workdirPath.relativeFilePath(path))),
                 tr("Unable to add file to index"));

    handleResult(git_index_write(index.data()),
                 tr("Unable to write index to disk"));
    setConfigValue(QSL("addpath"), QString());
    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryWorker::commit()
{
    QMutexLocker locker(&repoMutex);
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    setConfigValue(QSL("repostate"), QSL("dirty-commit"));

    QString commitMessage = QSL("Added files: \n");

    git_oid commitId, treeId;
    git_index *indexPtr = nullptr;
    handleResult(git_repository_index(&indexPtr, repo.data()),
                 tr("Unable to open repo index"));
    QSharedPointer<git_index> index(indexPtr, git_index_free);
    handleResult(git_index_write_tree(&treeId, index.data()),
                 tr("Unable to write tree"));
    git_tree *treePtr = nullptr;
    handleResult(git_tree_lookup(&treePtr, repo.data(), &treeId),
                 tr("Unable to lookup tree"));
    QSharedPointer<git_tree> tree(treePtr, git_tree_free);

    git_status_options statusOptions;
    handleResult(
        git_status_init_options(&statusOptions, GIT_STATUS_OPTIONS_VERSION),
        tr("Unable to init status options"));
    git_status_list *statusListPtr = nullptr;
    handleResult(
        git_status_list_new(&statusListPtr, repo.data(), &statusOptions),
        tr("Unable to make status list"));
    QSharedPointer<git_status_list> statusList(statusListPtr,
                                               git_status_list_free);
    for (size_t i = 0; i < git_status_list_entrycount(statusList.data()); i++) {
        const git_status_entry *entry =
            git_status_byindex(statusList.data(), i);
        if (entry->status == GIT_STATUS_INDEX_NEW)
            commitMessage.append(QString::fromLatin1("    %1\n")
                                     .arg(QString::fromUtf8(
                                         entry->head_to_index->new_file.path)));
    }

    QSharedPointer<git_signature> author = signature();
    if (git_repository_is_empty(repo.data()) == 1 ||
        git_repository_head_unborn(repo.data()) == 1) {
        handleResult(git_commit_create_v(&commitId, repo.data(), "HEAD",
                                         author.data(), author.data(), NULL,
                                         commitMessage.toUtf8(), tree.data(),
                                         0),
                     tr("Unable to create initial commit"));
    } else {
        git_oid parentId;
        handleResult(git_reference_name_to_id(&parentId, repo.data(), "HEAD"),
                     tr("Unable to lookup head oid"));
        git_commit *parentCommitPtr = nullptr;
        handleResult(
            git_commit_lookup(&parentCommitPtr, repo.data(), &parentId),
            tr("Unable to lookup commit"));
        QSharedPointer<git_commit> parentCommit(parentCommitPtr,
                                                git_commit_free);

        handleResult(git_commit_create_v(&commitId, repo.data(), "HEAD",
                                         author.data(), author.data(), NULL,
                                         commitMessage.toUtf8(), tree.data(), 1,
                                         parentCommit.data()),
                     tr("Unable to commit commit"));
    }
    setConfigValue(QSL("repostate"), QSL("clean"));
}

int ManagedDirectoryWorker::acquireCredentialsCallback(
    git_cred **cred, const char *url, const char *username,
    unsigned int allowedTypes, void *userData)
{
    Q_UNUSED(url);
    if (allowedTypes & GIT_CREDTYPE_SSH_KEY) {
        return git_cred_ssh_key_new(
            cred, username,
            ((ManagedDirectoryWorker *)userData)->publicKey.toUtf8(),
            ((ManagedDirectoryWorker *)userData)->privateKey.toUtf8(), NULL);
    } else if (allowedTypes & GIT_CREDTYPE_USERNAME) {
        qCWarning(APEX_RS, "GIT CREDTYPE USERNAME not supported");
    } else if (allowedTypes & GIT_CREDTYPE_USERPASS_PLAINTEXT) {
        qCDebug(APEX_RS, "GIT CREDTYPE USERNAME PASS PLAINTEXT not supported");
    } else if (allowedTypes & GIT_CREDTYPE_DEFAULT) {
        qCDebug(APEX_RS, "GIT CREDTYPE DEFAULT not supported");
    }
    return -1;
}

void ManagedDirectoryWorker::push()
{
    QMutexLocker locker(&repoMutex);
    try {
        if (!repo)
            throw ApexStringException(tr("No repository opened"));

        setConfigValue(QSL("default"), QSL("simple"), QSL("push"));
        git_push_options pushOptions;
        handleResult(
            git_push_init_options(&pushOptions, GIT_PUSH_OPTIONS_VERSION),
            tr("Unable to init push options"));
        pushOptions.callbacks.credentials =
            ManagedDirectoryWorker::acquireCredentialsCallback;
        pushOptions.callbacks.push_transfer_progress =
            ManagedDirectoryWorker::pushProgressCallback;
        pushOptions.callbacks.payload = (void *)this;

        /* Upload instead of push to create non-existent refs */
        QSharedPointer<git_remote> remote = findRemote();
        handleResult(git_remote_upload(remote.data(), NULL, &pushOptions),
                     tr("Unable to push"));

        git_remote_callbacks callbacks;
        handleResult(
            git_remote_init_callbacks(&callbacks, GIT_REMOTE_CALLBACKS_VERSION),
            tr("Unable to init remote callbacks for updating tips"));
        callbacks.credentials =
            ManagedDirectoryWorker::acquireCredentialsCallback;
        callbacks.payload = (void *)this;

        handleResult(git_remote_update_tips(remote.data(), &callbacks, 0,
                                            GIT_REMOTE_DOWNLOAD_TAGS_NONE,
                                            NULL),
                     tr("Unable to update tips"));

        Q_EMIT pushDone();
    } catch (ApexStringException &e) {
        Q_EMIT error(QString::fromLocal8Bit(e.what()));
        Q_EMIT pushFailed();
    }
}

void ManagedDirectoryWorker::pull()
{
    QMutexLocker locker(&repoMutex);

    try {
        if (!repo)
            throw ApexStringException(tr("No repository opened"));

        doFetch();
        doCheckout();
        Q_EMIT pullDone();
    } catch (ApexStringException &e) {
        Q_EMIT error(QString::fromLocal8Bit(e.what()));
        Q_EMIT pullFailed();
    }
}

void ManagedDirectoryWorker::fetch()
{
    QMutexLocker locker(&repoMutex);

    try {
        if (!repo)
            throw ApexStringException(tr("No repository opened"));
        doFetch();
    } catch (ApexStringException &e) {
        Q_EMIT error(QString::fromLocal8Bit(e.what()));
    }
}

void ManagedDirectoryWorker::checkout()
{
    QMutexLocker locker(&repoMutex);

    try {
        if (!repo)
            throw ApexStringException(tr("No repository opened"));
        doCheckout();
    } catch (ApexStringException &e) {
        Q_EMIT error(QString::fromLocal8Bit(e.what()));
    }
}

void ManagedDirectoryWorker::retryLastAction()
{
    QMutexLocker locker(&repoMutex);

    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    QString repoState = getConfigValue(QSL("repostate"));
    if (repoState.endsWith(QL1S("-add"))) {
        add(getConfigValue(QSL("addpath")));
        commit();
    } else if (repoState.endsWith(QL1S("-commit"))) {
        commit();
    } else if (repoState.endsWith(QL1S("-fetch")) ||
               repoState.endsWith(QL1S("-checkout"))) {
        pull();
    }
}

void ManagedDirectoryWorker::doFetch()
{
    setConfigValue(QSL("repostate"), QSL("dirty-fetch"));
    git_fetch_options fetchOptions;
    handleResult(
        git_fetch_init_options(&fetchOptions, GIT_FETCH_OPTIONS_VERSION),
        tr("Unable to init fetch options"));
    fetchOptions.callbacks.transfer_progress =
        ManagedDirectoryWorker::progressCallback;
    fetchOptions.callbacks.credentials =
        ManagedDirectoryWorker::acquireCredentialsCallback;
    fetchOptions.callbacks.payload = (void *)this;

    QSharedPointer<git_remote> remote(findRemote());
    handleResult(
        git_remote_fetch(remote.data(), nullptr, &fetchOptions, nullptr),
        tr("Unable to fetch remote"));
    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryWorker::doCheckout()
{
    setConfigValue(QSL("repostate"), QSL("dirty-checkout"));
    git_object *remoteObj = nullptr;
    handleResult(git_revparse_single(&remoteObj, repo.data(),
                                     QSL("FETCH_HEAD").toUtf8()),
                 tr("Unable to find fetch head"));
    QSharedPointer<git_object> remoteHead(remoteObj, git_object_free);
    git_checkout_options checkoutOptions;
    handleResult(git_checkout_init_options(&checkoutOptions,
                                           GIT_CHECKOUT_OPTIONS_VERSION),
                 tr("Unable to init checkout options"));
    checkoutOptions.checkout_strategy = GIT_CHECKOUT_FORCE;
    handleResult(git_reset(repo.data(), remoteHead.data(), GIT_RESET_HARD,
                           &checkoutOptions),
                 tr("unable to reset head to fetch head"));
    setConfigValue(QSL("repostate"), QSL("clean"));
}

int ManagedDirectoryWorker::progressCallback(const git_transfer_progress *stats,
                                             void *userData)
{
    ((ManagedDirectoryWorker *)userData)
        ->progress(stats->received_objects, stats->total_objects);
    return 0;
}

int ManagedDirectoryWorker::pushProgressCallback(unsigned current,
                                                 unsigned total, size_t bytes,
                                                 void *userData)
{
    Q_UNUSED(bytes);
    ((ManagedDirectoryWorker *)userData)->progress(current, total);
    return 0;
}

void ManagedDirectoryWorker::setConfigValue(const QString &key,
                                            const QString &value,
                                            const QString &category)
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    git_config *configPtr = nullptr;
    handleResult(git_repository_config(&configPtr, repo.data()),
                 tr("Unable to open git config"));
    QSharedPointer<git_config> config(configPtr, git_config_free);
    if (!value.isEmpty())
        handleResult(
            git_config_set_string(
                config.data(),
                QFile::encodeName(
                    QString::fromLatin1("%1.%2").arg(category).arg(key)),
                QFile::encodeName(value)),
            tr("Unable to set key in git config"));
    else
        handleResult(
            git_config_delete_entry(
                config.data(),
                QFile::encodeName(
                    QString::fromLatin1("%1.%2").arg(category).arg(key))),
            tr("Unable to delete entry after passing empty value to "
               "setConfigValue"));
}

QString ManagedDirectoryWorker::getConfigValue(const QString &key,
                                               const QString &category)
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    git_config *configPtr = nullptr;
    handleResult(git_repository_config(&configPtr, repo.data()),
                 tr("Unable to open git config"));
    QSharedPointer<git_config> config(configPtr, git_config_free);
    git_config *snapshotPtr = nullptr;
    handleResult(git_config_snapshot(&snapshotPtr, config.data()),
                 tr("Unable to make snapshot of git config"));
    QSharedPointer<git_config> configSnapshot(snapshotPtr, git_config_free);

    const char *value = nullptr;
    int result = git_config_get_string(
        &value, configSnapshot.data(),
        QFile::encodeName(QString::fromLatin1("%1.%2").arg(category).arg(key)));
    if (result == 0)
        return QString::fromUtf8(value);

    if (result != GIT_ENOTFOUND)
        handleResult(result, tr("Unable to get git config value"));
    return QString();
}

QSharedPointer<git_remote> ManagedDirectoryWorker::findRemote()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    gitStrArray remotes;
    handleResult(git_remote_list(&remotes, repo.data()));

    if (remotes.array.count != 1)
        qCWarning(APEX_RS,
                  "Number of remotes in Managed Directory %s is not 1, but %zu",
                  qPrintable(repoPath.path()), remotes.array.count);

    git_remote *remote = nullptr;
    handleResult(
        git_remote_lookup(&remote, repo.data(), remotes.array.strings[0]),
        tr("Error during remote lookup"));
    return QSharedPointer<git_remote>(remote, git_remote_free);
}

QSharedPointer<git_signature> ManagedDirectoryWorker::signature()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    git_signature *signaturePtr = nullptr;
    handleResult(git_signature_default(&signaturePtr, repo.data()),
                 tr("Unable to create new signature"));
    return QSharedPointer<git_signature>(signaturePtr, git_signature_free);
}

void ManagedDirectoryWorker::handleResult(int result, const QString &message)
{
    const git_error *error = giterr_last();
    if (error != nullptr)
        qCWarning(APEX_RS, "Managed directory [%s] (message: %s | result: %i | "
                           "error: %s | klass: %i)",
                  qPrintable(repoPath.path()), qPrintable(message), result,
                  error->message, error->klass);
    giterr_clear();

    if (result < 0)
        throw ApexStringException(message);
}

/* ManagedDirectoryPrivate=================================================== */

class ManagedDirectoryPrivate : public QObject
{
    Q_OBJECT
public:
    QThread workerThread;
    ManagedDirectoryWorker worker;
    QDir repoPath;
    QDir workdirPath;
};

/* ManagedDirectory========================================================== */

ManagedDirectory::ManagedDirectory(QObject *parent)
    : QObject(parent), d(new ManagedDirectoryPrivate)
{
    setup();
}

ManagedDirectory::ManagedDirectory(const QString &path, QObject *parent)
    : QObject(parent), d(new ManagedDirectoryPrivate)
{
    setup();
    d->repoPath = QDir(path);
    d->workdirPath = QDir(path);
}

void ManagedDirectory::setup()
{
    git_libgit2_init();
    fillSSLCertificates();
    d->worker.moveToThread(&d->workerThread);
    d->workerThread.start();
    connect(&d->worker, SIGNAL(pullDone()), this, SIGNAL(pullDone()));
    connect(&d->worker, SIGNAL(pullFailed()), this, SIGNAL(pullFailed()));
    connect(&d->worker, SIGNAL(pushDone()), this, SIGNAL(pushDone()));
    connect(&d->worker, SIGNAL(pushFailed()), this, SIGNAL(pushFailed()));
    connect(&d->worker, SIGNAL(retryLastActionDone()), this,
            SIGNAL(retryLastActionDone()));
    connect(&d->worker, SIGNAL(progress(int, int)), this,
            SIGNAL(progress(int, int)));
    connect(&d->worker, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
}

ManagedDirectory::~ManagedDirectory()
{
    d->worker.close();
    d->workerThread.quit();
    d->workerThread.wait();
    git_libgit2_shutdown();
}

bool ManagedDirectory::exists()
{
    return git_repository_open_ext(nullptr,
                                   QFile::encodeName(d->repoPath.path()),
                                   GIT_REPOSITORY_OPEN_NO_SEARCH, nullptr) == 0;
}

bool ManagedDirectory::isOpen()
{
    return d->worker.isOpen();
}

QString ManagedDirectory::lastCommitMessage()
{
    return d->worker.lastCommitMessage();
}

bool ManagedDirectory::isClean()
{
    return d->worker.isClean();
}

bool ManagedDirectory::hasLocalRemote()
{
    return d->worker.hasLocalRemote();
}

int ManagedDirectory::commitsAheadOrigin()
{
    return d->worker.commitsAheadOrigin();
}

int ManagedDirectory::totalCommitCount()
{
    return d->worker.totalCommitCount();
}

void ManagedDirectory::setPath(const QString &repoPath,
                               const QString &workdirPath)
{
    if (d->worker.isOpen()) {
        d->worker.close();
        qCWarning(APEX_RS, "Changing path while ManagedDirectory is opened or"
                           "initialized %s, closing repo.",
                  qPrintable(workdirPath));
    }
    d->repoPath = QDir(repoPath);
    d->workdirPath = QDir(workdirPath);
}

void ManagedDirectory::setKeyPaths(const QString &publicKeyPath,
                                   const QString &privateKeyPath)
{
    d->worker.setKeyPaths(publicKeyPath, privateKeyPath);
}

void ManagedDirectory::setRemote(const QString &url, const QString &branchName)
{
    d->worker.setRemote(url, branchName);
}

void ManagedDirectory::setAuthor(const QString &name, const QString &email)
{
    d->worker.setAuthor(name, email);
}

void ManagedDirectory::init(bool bare)
{
    d->worker.init(d->repoPath, d->workdirPath, bare);
}

void ManagedDirectory::open()
{
    d->worker.open(d->repoPath);
}

void ManagedDirectory::close()
{
    d->worker.close();
}

void ManagedDirectory::add(const QString &path)
{
    QString relativePath = path;
    if (QFileInfo(relativePath).isAbsolute())
        relativePath.remove(d->workdirPath.canonicalPath() + QL1S("/"));

    d->worker.add(relativePath);
    d->worker.commit();
}

void ManagedDirectory::add(const QStringList &paths)
{
    Q_FOREACH (const QString &path, paths) {
        QString relativePath = path;
        if (QFileInfo(relativePath).isAbsolute())
            relativePath.remove(d->workdirPath.canonicalPath() + QL1S("/"));
        d->worker.add(relativePath);
    }
    d->worker.commit();
}

void ManagedDirectory::push(Qt::ConnectionType connectionType)
{
    QMetaObject::invokeMethod(&d->worker, "push", connectionType);
}

void ManagedDirectory::pull(Qt::ConnectionType connectionType)
{
    QMetaObject::invokeMethod(&d->worker, "pull", connectionType);
}

void ManagedDirectory::fetch(Qt::ConnectionType connectionType)
{
    QMetaObject::invokeMethod(&d->worker, "fetch", connectionType);
}

void ManagedDirectory::checkout(Qt::ConnectionType connectionType)
{
    QMetaObject::invokeMethod(&d->worker, "checkout", connectionType);
}

void ManagedDirectory::retryLastAction()
{
    QMetaObject::invokeMethod(&d->worker, "retryLastAction",
                              Qt::QueuedConnection);
}

bool ManagedDirectory::exists(const QString &path)
{
    return git_repository_open_ext(nullptr, QFile::encodeName(path),
                                   GIT_REPOSITORY_OPEN_NO_SEARCH, nullptr) == 0;
}

QStringList ManagedDirectory::lsRemote(const QString &url,
                                       const QString &publicKeyPath,
                                       const QString &privateKeyPath)
{
    git_libgit2_init();
    fillSSLCertificates();
    QTemporaryDir tempDir;
    QStringList refheads;
    int result;

    git_repository *repoPtr = nullptr;
    result =
        git_repository_init(&repoPtr, QFile::encodeName(tempDir.path()), false);
    if (result)
        throw ApexStringException(
            tr("Unable to init repository in temporary directory."));
    QSharedPointer<git_repository> repo(repoPtr, git_repository_free);

    git_remote *remotePtr = nullptr;
    result = git_remote_create_anonymous(&remotePtr, repo.data(), url.toUtf8());
    if (result)
        throw ApexStringException(tr("Unable to create anonymous remote."));
    QSharedPointer<git_remote> remote(remotePtr, git_remote_free);

    ManagedDirectoryWorker worker;
    worker.setKeyPaths(publicKeyPath, privateKeyPath);

    git_remote_callbacks callbacks;
    result =
        git_remote_init_callbacks(&callbacks, GIT_REMOTE_CALLBACKS_VERSION);
    callbacks.credentials = ManagedDirectoryWorker::acquireCredentialsCallback;
    callbacks.payload = &worker;

    if (result)
        throw ApexStringException(
            tr("Unable to init remote callbacks, used for credentials."));

    git_remote_head **heads;
    size_t size;
#if (LIBGIT2_SOVERSION >= 25)
    result = git_remote_connect(remote.data(), GIT_DIRECTION_FETCH, &callbacks,
                                NULL, NULL);
#else
    result = git_remote_connect(remote.data(), GIT_DIRECTION_FETCH, &callbacks,
                                NULL);
#endif

    if (result)
        throw ApexStringException(tr("Unable to connect to anonymous remote."));
    result =
        git_remote_ls((const git_remote_head ***)&heads, &size, remote.data());
    if (result)
        throw ApexStringException(tr("Unable to list remote."));

    for (int i = 0; i < (int)size; ++i) {
        QString ref = QString::fromUtf8(heads[i]->name);
        if (ref.startsWith(QSL("refs/heads/")))
            refheads.append(ref.split("/").back());
    }
    git_libgit2_shutdown();
    return refheads;
}
}

#include "manageddirectory.moc"
