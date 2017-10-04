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
#include <QDir>
#include <QFile>
#include <QSharedPointer>

namespace apex
{

class ManagedDirectoryPrivate : public QSharedData
{
    Q_DECLARE_TR_FUNCTIONS(ManagedDirectoryPrivate)
public:
    void fillSSLCertificates();

    bool isClean();
    void init();
    void open();

    void setRemote(const QString &name, const QString &url,
                   const QString &branchName);
    void setAuthor(const QString &name, const QString &email);

    void add(const QString &path);
    void amend();
    void push();
    void fetch();
    void checkout();

    void retryLastAction();
    void reClone();

    QSharedPointer<git_repository> repo;
    QDir directory;
    QString publicKey;
    QString privateKey;

private:
    void setConfigValue(const QString &key, const QString &value,
                        const QString &category = QL1S("userdata"));
    QString getConfigValue(const QString &key,
                           const QString &category = QL1S("userdata"));

    QSharedPointer<git_remote> findRemote();
    QSharedPointer<git_signature> signature();
    void handleResult(int result, const QString &message = QString());
};

/* ManagedDirectoryPrivate=================================================== */

void ManagedDirectoryPrivate::fillSSLCertificates()
{
#ifdef Q_OS_ANDROID
    QDir certsDir(
        apex::android::ApexAndroidMethods::getSSLCertificateDirectory());
    Q_FOREACH (const QString &obj,
               certsDir.entryList(QDir::NoDotAndDotDot | QDir::Files)) {
        handleResult(git_libgit2_opts(
                         GIT_OPT_SET_SSL_CERT_LOCATIONS,
                         QFile::encodeName(certsDir.filePath(obj)).constData(),
                         NULL),
                     tr("Unable to set ssl file location"));
    };
#endif
}

bool ManagedDirectoryPrivate::isClean()
{
    return getConfigValue(QSL("repostate")) != QL1S("clean");
}

void ManagedDirectoryPrivate::init()
{
    git_repository *repoPtr = nullptr;
    handleResult(git_repository_init(
                     &repoPtr, QFile::encodeName(directory.path()), false),
                 tr("Unable to initialize repository"));
    repo = QSharedPointer<git_repository>(repoPtr, git_repository_free);
    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryPrivate::open()
{
    git_repository *repoPtr = nullptr;
    handleResult(
        git_repository_open(&repoPtr, QFile::encodeName(directory.path())),
        tr("Unable to open repository %1, does it exists?")
            .arg(directory.path()));
    repo = QSharedPointer<git_repository>(repoPtr, git_repository_free);
    if (getConfigValue(QSL("repostate")) != QL1S("clean"))
        qCWarning(APEX_RS) << "Warning, repo in dirty state";
}

void ManagedDirectoryPrivate::setRemote(const QString &name, const QString &url,
                                        const QString &branchName)
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    /* delete all other remotes, this directory should only have one remote */
    git_strarray remotes = {nullptr, 0};
    handleResult(git_remote_list(&remotes, repo.data()),
                 tr("Unable to list remotes"));

    for (unsigned i = 0; i < remotes.count; ++i)
        handleResult(git_remote_delete(repo.data(), remotes.strings[i]),
                     tr("Error deleting remote"));

    git_remote *remotePtr = nullptr;
    QString refspec = QString::fromLatin1("+refs/heads/%1:refs/remotes/%2/%1")
                          .arg(branchName)
                          .arg(name);
    handleResult(git_remote_create_with_fetchspec(&remotePtr, repo.data(),
                                                  name.toUtf8(), url.toUtf8(),
                                                  refspec.toUtf8()),
                 tr("error creating remote"));
    QSharedPointer<git_remote> remote(remotePtr, git_remote_free);

    handleResult(git_remote_list(&remotes, repo.data()),
                 tr("Unable to list remotes"));
}

void ManagedDirectoryPrivate::setAuthor(const QString &name,
                                        const QString &email)
{
    setConfigValue(QSL("name"), name, QSL("user"));
    setConfigValue(QSL("email"), email, QSL("user"));
}

void ManagedDirectoryPrivate::add(const QString &path)
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    if (!directory.exists(path)) // QFile::exists(path))
        throw ApexStringException(
            tr("Unable to add nonexistent file %1 to directory").arg(path));

    setConfigValue(QSL("repostate"), QSL("dirty-add"));
    setConfigValue(QSL("addpath"), path);

    git_index *indexPtr = nullptr;
    handleResult(git_repository_index(&indexPtr, repo.data()),
                 tr("Unable to open repo index"));
    QSharedPointer<git_index> index(indexPtr, git_index_free);
    handleResult(
        git_index_add_bypath(
            index.data(), QFile::encodeName(directory.relativeFilePath(path))),
        tr("Unable to add file to index"));
    setConfigValue(QSL("addpath"), QString());
    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryPrivate::amend()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    setConfigValue(QSL("repostate"), QSL("dirty-amend"));

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

    for (unsigned i = 0; i < git_index_entrycount(index.data()); i++) {
        const git_index_entry *entry = git_index_get_byindex(index.data(), i);
        commitMessage.append(QString::fromLatin1("    %1\n")
                                 .arg(QString::fromUtf8(entry->path)));
    }

    if (git_repository_is_empty(repo.data()) == 1 ||
        git_repository_head_unborn(repo.data()) == 1) {
        QSharedPointer<git_signature> author = signature();
        handleResult(
            git_commit_create(&commitId, repo.data(), "refs/heads/master",
                              author.data(), author.data(), NULL,
                              commitMessage.toUtf8(), tree.data(), 0, NULL),
            tr("Unable to create initial commit"));
    } else {
        handleResult(git_reference_name_to_id(&commitId, repo.data(), "HEAD"),
                     tr("Unable to lookup head oid"));
        git_commit *commitPtr = nullptr;
        handleResult(git_commit_lookup(&commitPtr, repo.data(), &commitId),
                     tr("Unable to lookup commit"));
        QSharedPointer<git_commit> commit(commitPtr, git_commit_free);

        handleResult(git_commit_amend(&commitId, commit.data(), "HEAD", NULL,
                                      NULL, NULL, commitMessage.toUtf8(),
                                      tree.data()),
                     tr("Unable to amend commit"));
    }
    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryPrivate::push()
{
    throw ApexStringException(tr("Action not supported"));
}

void ManagedDirectoryPrivate::fetch()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));
    setConfigValue(QSL("repostate"), QSL("dirty-fetch"));

    QSharedPointer<git_remote> remote = findRemote();
    git_fetch_options fetchOptions;
    handleResult(
        git_fetch_init_options(&fetchOptions, GIT_FETCH_OPTIONS_VERSION),
        tr("Unable to init fetch options"));

    handleResult(
        git_remote_fetch(remote.data(), nullptr, &fetchOptions, nullptr),
        tr("Unable to fetch remote"));

    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryPrivate::checkout()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    setConfigValue(QSL("repostate"), QSL("dirty-checkout"));

    // git_repository_set_head(repo.data(), "FETCH_HEAD");
    git_object *remoteObj = nullptr;
    handleResult(git_revparse_single(&remoteObj, repo.data(),
                                     QFile::encodeName("FETCH_HEAD")),
                 tr("Unable to find fetch head"));
    QSharedPointer<git_object> remoteHead(remoteObj, git_object_free);
    git_checkout_options checkoutOptions;
    handleResult(git_checkout_init_options(&checkoutOptions,
                                           GIT_CHECKOUT_OPTIONS_VERSION),
                 tr("Unable to init checkout options"));
    checkoutOptions.checkout_strategy = GIT_CHECKOUT_FORCE;
    handleResult(
        git_checkout_tree(repo.data(), remoteHead.data(), &checkoutOptions),
        tr("Unable to checkout fetch head"));

    handleResult(git_repository_set_head(repo.data(), "FETCH_HEAD"),
                 tr("Unable to set HEAD to FETCH_HEAD"));

    // git_checkout_head(repo.data(), &checkoutOptions);
    setConfigValue(QSL("repostate"), QSL("clean"));
}

void ManagedDirectoryPrivate::retryLastAction()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    QString repoState = getConfigValue(QSL("repostate"));
    if (repoState.endsWith(QL1S("-add"))) {
        add(getConfigValue(QSL("addpath")));
        amend();
    } else if (repoState.endsWith(QL1S("-amend"))) {
        amend();
    } else if (repoState.endsWith(QL1S("-fetch"))) {
        fetch();
    } else if (repoState.endsWith(QL1S("-checkout"))) {
        checkout();
    }
}

void ManagedDirectoryPrivate::reClone()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    setConfigValue(QSL("repostate"), QSL("dirty-reclone"));

    QDir rootDirectory(directory);
    if (!rootDirectory.cdUp())
        throw ApexStringException(tr("Unable to change directory upward"));

    QString currentDirectory = directory.dirName();
    QString cloneDirectory(currentDirectory + QL1S("-clone"));
    QString oldDirectory(currentDirectory + QL1S("-old"));
    if (!rootDirectory.mkdir(cloneDirectory))
        throw ApexStringException(
            tr("Unable to create temporary clone directory"));

    git_clone_options cloneOptions;
    git_clone_init_options(&cloneOptions, GIT_CLONE_OPTIONS_VERSION);
    git_repository *clonedRepoPtr = nullptr;
    handleResult(
        git_clone(&clonedRepoPtr,
                  QFile::encodeName(rootDirectory.filePath(currentDirectory)),
                  QFile::encodeName(rootDirectory.filePath(cloneDirectory)),
                  &cloneOptions),
        tr("Cloning failed"));
    QSharedPointer<git_repository> clonedRepo(clonedRepoPtr,
                                              git_repository_free);
    git_config *configPtr = nullptr;
    handleResult(git_repository_config(&configPtr, clonedRepo.data()),
                 tr("Unable to open git config"));
    QSharedPointer<git_config> config(configPtr, git_config_free);
    handleResult(
        git_config_set_string(config.data(), "userdata.repostate", "clean"),
        tr("Unable to set key in git config"));
    setConfigValue(QSL("repostate"), QSL("clean"));
    clonedRepo.clear();

    /* close our repository */
    repo.clear();
    /*rename*/
    rootDirectory.rename(currentDirectory, oldDirectory);
    rootDirectory.rename(cloneDirectory, currentDirectory);
    QDir(rootDirectory.filePath(oldDirectory)).removeRecursively();
    /* Open the new repo */
    open();
}

void ManagedDirectoryPrivate::setConfigValue(const QString &key,
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

QString ManagedDirectoryPrivate::getConfigValue(const QString &key,
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

QSharedPointer<git_remote> ManagedDirectoryPrivate::findRemote()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    git_strarray remotes = {nullptr, 0};
    handleResult(git_remote_list(&remotes, repo.data()));

    if (remotes.count != 1)
        qCWarning(APEX_RS,
                  "Number of remotes in Managed Directory %s is not 1, but %zu",
                  qPrintable(directory.path()), remotes.count);

    git_remote *remote = nullptr;
    handleResult(git_remote_lookup(&remote, repo.data(), remotes.strings[0]),
                 tr("Error during remote lookup"));
    return QSharedPointer<git_remote>(remote, git_remote_free);
}

QSharedPointer<git_signature> ManagedDirectoryPrivate::signature()
{
    if (!repo)
        throw ApexStringException(tr("No repository opened"));

    git_signature *signaturePtr = nullptr;
    handleResult(git_signature_default(&signaturePtr, repo.data()),
                 tr("Unable to create new signature"));
    return QSharedPointer<git_signature>(signaturePtr, git_signature_free);
}

void ManagedDirectoryPrivate::handleResult(int result, const QString &message)
{
    const git_error *error = giterr_last();
    if (error != nullptr)
        qCDebug(APEX_RS, "Dir %s: %s", qPrintable(directory.dirName()),
                (error->message));
    giterr_clear();

    if (result < 0)
        throw ApexStringException(message);
}

/* ManagedDirectory========================================================== */

ManagedDirectory::ManagedDirectory()
{
    git_libgit2_init();
    d->fillSSLCertificates();
}

ManagedDirectory::ManagedDirectory(const QString &path)
    : d(new ManagedDirectoryPrivate)
{
    git_libgit2_init();
    d->fillSSLCertificates();
    d->directory = QDir(path);
}

ManagedDirectory::~ManagedDirectory()
{
    d->repo.clear();
    git_libgit2_shutdown();
}

QString ManagedDirectory::path() const
{
    return d->directory.path();
}

QStringList ManagedDirectory::objects() const
{
    return d->directory.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
}

bool ManagedDirectory::isClean()
{
    return d->isClean();
}

void ManagedDirectory::setPath(const QString &path)
{
    if (d->repo) {
        qCWarning(APEX_RS, "Changing path while ManagedDirectory is opened or "
                           "initialized %s, closing repo.",
                  qPrintable(path));
        d->repo.clear();
    }
    d->directory = QDir(path);
}

void ManagedDirectory::setKeyPaths(const QString &publicKey,
                                   const QString &privateKey)
{
    d->publicKey = publicKey;
    d->privateKey = privateKey;
}

void ManagedDirectory::setRemote(const QString &name, const QString &url,
                                 const QString &branchName)
{
    d->setRemote(name, url, branchName);
}

void ManagedDirectory::setAuthor(const QString &name, const QString &email)
{
    d->setAuthor(name, email);
}

void ManagedDirectory::open()
{
    d->open();
}

void ManagedDirectory::init()
{
    d->init();
}

void ManagedDirectory::add(const QString &path)
{
    d->add(path);
    d->amend();
}

void ManagedDirectory::add(const QStringList &paths)
{
    Q_FOREACH (const QString &path, paths)
        d->add(path);
    d->amend();
}

void ManagedDirectory::push()
{
    d->push();
}

void ManagedDirectory::pull()
{
    d->fetch();
    d->checkout();
}

void ManagedDirectory::retryLastAction()
{
    d->retryLastAction();
}

void ManagedDirectory::reClone()
{
    d->reClone();
}

bool ManagedDirectory::exists(const QString &path)
{
    return git_repository_open_ext(nullptr, QFile::encodeName(path),
                                   GIT_REPOSITORY_OPEN_NO_SEARCH, nullptr) == 0;
}
}
