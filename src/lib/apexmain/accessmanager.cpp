#include "accessmanager.h"

#include <QDir>

QString apex::AccessManager::toLocalFile(const QUrl &url,
                                         const QString &apexBaseDir)
{
    if (url.scheme() == "apex") {
        return QDir(apexBaseDir).absoluteFilePath(url.path());
    }

    return url.path();
}
