#include "accessmanager.h"

#include <QDir>
#include <QFileInfo>
#include <QUrl>

QUrl apex::AccessManager::prepare(QUrl url)
{
    if (url.scheme().isEmpty() || url.scheme() == "file") {
        if (QDir::isRelativePath(url.path())) {
            QString relativePath = QDir::fromNativeSeparators(url.path());
            return QUrl::fromLocalFile(
                QDir::current().absoluteFilePath(relativePath));
        } else {
            return QUrl::fromLocalFile(url.path());
        }
    }

    return url;
}
