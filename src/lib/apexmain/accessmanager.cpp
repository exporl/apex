#include "apextools/apexpaths.h"

#include "accessmanager.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QNetworkRequest>

using namespace apex;

AccessManager::AccessManager(QObject *parent) : QNetworkAccessManager(parent)
{
}

QUrl AccessManager::transformApexUrl(QUrl url) const
{
    QUrl newUrl;

    QStringList dirs;
    dirs << ApexPaths::GetScriptsPath();
    dirs << ApexPaths::GetScriptsPath() + "/external";
    dirs << ApexPaths::GetNonBinaryPluginPath();

    QFileInfo fi(url.path());

    newUrl.setPath(fi.filePath());
    newUrl.setScheme("file");

    if (!QFile::exists(fi.filePath())) {
        Q_FOREACH (const QString &prefix, dirs) {
            QString newpath(prefix + "/" + fi.filePath());
            if (QFile::exists(newpath)) {
                newUrl.setPath(newpath);
                break;
            }
        }
    }

    return newUrl;
}

QNetworkReply *AccessManager::createRequest(Operation op,
                                            const QNetworkRequest &req,
                                            QIODevice *outgoingData)
{
    QNetworkRequest newreq(req);
    // qCDebug(APEX_RS) << "createRequest " << req.url();

    // if scheme "apex" is used, try to find the file in some apex data paths
    if (req.url().scheme() == "apex") {
        newreq.setUrl(transformApexUrl(req.url()));
    }

    if (newreq.url().isLocalFile() &&
        !QFile::exists(newreq.url().toLocalFile()))
        qCWarning(APEX_RS, "%s",
                  qPrintable(QSL("%1: %2").arg(
                      "AccessManager",
                      tr("Resource not found: %1").arg(req.url().toString()))));

    qCDebug(APEX_RS) << "createRequest " << newreq.url();
    return QNetworkAccessManager::createRequest(op, newreq, outgoingData);
}

QUrl AccessManager::prepare(QUrl url) const
{
    QUrl correctUrl(url);
    QFileInfo fi(url.path());
    if (url.isRelative() || url.scheme() == "file") {
        if (fi.isRelative()) {
            QString correctPath =
                QUrl::fromUserInput(
                    QDir::toNativeSeparators(QDir::current().path() + "/" +
                                             fi.filePath()))
                    .path();

            correctUrl.setPath(correctPath);
            correctUrl.setScheme("file");
        } else {
            correctUrl = url;
        }
    } else if (url.scheme() == "apex") {
        correctUrl.setPath(fi.filePath());
        correctUrl.setScheme("apex");
    } else {
        correctUrl = url;
    }

    return correctUrl;
}
