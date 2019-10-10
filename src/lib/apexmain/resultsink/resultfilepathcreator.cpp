#include "resultfilepathcreator.h"
#include "apexmain/resultsink/apexresultsink.h"
#include "apexmain/study/studymanager.h"

#include <QDir>
#include <QFileInfo>
#include <QString>

namespace apex
{

const QString ResultfilePathCreator::createDefaultAbsolutePath(
    const QString &experimentfilePath, const QString &subject) const
{
    QFileInfo experimentFileInfo(experimentfilePath);

    QString path = QSL("%1/%2-%3.%4")
                       .arg(experimentFileInfo.path())
                       .arg(experimentFileInfo.completeBaseName())
                       .arg(subject.isEmpty() ? "results" : QString(subject))
                       .arg(ApexResultSink::resultfileExtension);

    return addTimestamp(path);
}

const QString ResultfilePathCreator::createAbsolutePathForFlow(
    const QDir &flowBaseDir, const QString &experimentfilePath,
    const QString &resultfilePath) const
{
    if (QDir::isAbsolutePath(resultfilePath))
        return resultfilePath;

    if (StudyManager::instance()->belongsToActiveStudy(experimentfilePath))
        return addTimestamp(StudyManager::instance()->makeResultfilePath(
            experimentfilePath, resultfilePath));

    if (resultfilePath.isEmpty())
        return QString();

    return addTimestamp(flowBaseDir.absoluteFilePath(resultfilePath));
}

const QString ResultfilePathCreator::addTimestamp(const QString &path) const
{
    QFileInfo templateFileInfo(path);

    return QString("%1/%2-%3.%4")
        .arg(templateFileInfo.path())
        .arg(templateFileInfo.completeBaseName())
        .arg(QDateTime::currentDateTime().toString("yyyyMMddTHHmmss"))
        .arg(templateFileInfo.suffix());
}
}
