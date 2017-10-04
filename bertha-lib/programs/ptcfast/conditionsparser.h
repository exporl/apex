#ifndef CONDITIONSPARSER_H
#define CONDITIONSPARSER_H

#include <QPair>

class QDomElement;

struct Condition {
    QString tone;
    QString noise;
    QString masker;
};

class ConditionsParser
{
public:
    ConditionsParser();

    /**
     * Parses the given condition file and returns a map containing
     * (Name -> Condition) pairs.
     */
    QMap<QString, Condition> parse(const QString &file);

private:
    QPair<QString, Condition> parseCondition(const QDomElement &element,
                                             const QString &prefix);

    QString createPrefix(const QString &dataFile,
                         const QString &relativePrefix);

    void checkFiles(const Condition &condition);
    void checkFile(const QString &file);
};

#endif
