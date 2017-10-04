#include "bertha/xmlparser.h"

#include "common/exception.h"
#include "common/paths.h"

#include "conditionsparser.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMap>

using namespace cmn;

ConditionsParser::ConditionsParser()
{
}

QMap<QString, Condition> ConditionsParser::parse(const QString &file)
{
    QMap<QString, Condition> ret;

    bertha::XmlParser parser(Paths::searchFile(
        QLatin1String("schema/ptcfast.xsd"), Paths::dataDirectories()));

    QDomDocument doc = parser.parse(file);
    QDomElement rootElement = doc.documentElement();
    Q_ASSERT(rootElement.tagName() == QLatin1String("ptcfast"));

    QDomElement prefixElement = rootElement.firstChildElement();
    Q_ASSERT(prefixElement.tagName() == QLatin1String("prefix"));
    QString prefix = createPrefix(file, prefixElement.text());

    QDomElement current;
    for (current = prefixElement.nextSiblingElement(); !current.isNull();
         current = current.nextSiblingElement()) {
        QPair<QString, Condition> condition = parseCondition(current, prefix);

        if (ret.contains(condition.first)) {
            QString error = QObject::tr("Multiple conditions with name %1");
            throw Exception(error.arg(condition.first));
        }

        ret[condition.first] = condition.second;
    }

    Q_ASSERT(!ret.isEmpty());
    return ret;
}

QPair<QString, Condition>
ConditionsParser::parseCondition(const QDomElement &element,
                                 const QString &prefix)
{
    Q_ASSERT(element.tagName() == QLatin1String("condition"));

    QString name = element.attribute(QLatin1String("name"));
    Q_ASSERT(!name.isEmpty());

    Condition condition;
    QDomElement stimulus;

    for (stimulus = element.firstChildElement(); !stimulus.isNull();
         stimulus = stimulus.nextSiblingElement()) {
        QString tag = stimulus.tagName();
        QString value = QFileInfo(prefix, stimulus.text()).absoluteFilePath();

        if (tag == QLatin1String("tone"))
            condition.tone = value;
        else if (tag == QLatin1String("noise"))
            condition.noise = value;
        else if (tag == QLatin1String("masker"))
            condition.masker = value;
        else
            qFatal("Unknown tag %s", qPrintable(tag));
    }

    Q_ASSERT(!condition.tone.isEmpty() && !condition.noise.isEmpty() &&
             !condition.masker.isEmpty());

    checkFiles(condition);

    return qMakePair(name, condition);
}

QString ConditionsParser::createPrefix(const QString &dataFile,
                                       const QString &relativePrefix)
{
    if (QDir::isAbsolutePath(relativePrefix))
        return relativePrefix;

    QFileInfo info(dataFile);
    return QDir(info.absolutePath() + QLatin1Char('/') + relativePrefix)
        .absolutePath();
}

void ConditionsParser::checkFiles(const Condition &condition)
{
    checkFile(condition.tone);
    checkFile(condition.noise);
    checkFile(condition.masker);
}

void ConditionsParser::checkFile(const QString &file)
{
    QFileInfo info(file);
    QString error;

    if (!info.exists())
        error = QObject::tr("File %1 does not exist");
    else if (!info.isReadable())
        error = QObject::tr("File %1 is not readable");

    if (!error.isEmpty())
        throw Exception(error.arg(file));
}
