/******************************************************************************
 * Copyright (C) 2010  Michael Hofmann <mh21@piware.de>                       *
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

#include "exceptions.h"
#include "optioncontext.h"
#include "optionparser.h"

#include <cstdlib>
#include <iostream>

namespace apex
{

// OptionContext ===============================================================

OptionContext::OptionContext(const QString &description,
    const OptionGroup &mainGroup, const QList<OptionGroup> &groups) :
    description(description),
    helpEnabled(true),
    mainGroup(mainGroup),
    groups(groups)
{
}

void OptionContext::addGroup(const OptionGroup &group)
{
    groups += group;
}

void OptionContext::setMainGroup(const OptionGroup &group)
{
    mainGroup = group;
}

void OptionContext::setHelpEnabled(bool value)
{
    helpEnabled = value;
}

bool OptionContext::isHelpEnabled() const
{
    return helpEnabled;
}

QStringList OptionContext::parse(const QStringList &args)
{
    QMap<unsigned, OptionGroup> helpMap;
    QMap<unsigned, OptionEntry> entryMap;
    QMap<QChar, OptionEntry> shortMap;
    QList<LongOption> longOpts;
    if (helpEnabled) {
        longOpts += LongOption(QLatin1String("help"), LongOption::NoArgument, NULL, QLatin1Char('h'));
        longOpts += LongOption(QLatin1String("help-all"), LongOption::NoArgument, NULL, 2);
        Q_FOREACH (const OptionGroup &group, groups) {
            helpMap[longOpts.size()] = group;
            longOpts += LongOption(QLatin1String("help-") + group.name,
                        LongOption::NoArgument, NULL, 0);
        }
    }
    QString shortOpts(helpEnabled ? QLatin1String(":h") : QLatin1String(":"));
    QList<OptionGroup> allGroups(groups);
    allGroups += mainGroup;
    Q_FOREACH (const OptionGroup &group, allGroups) {
        Q_FOREACH (const OptionEntry &entry, group.entries) {
            // Short option
            if (entry.shortName != 0) {
                shortOpts += entry.shortName;
                if (entry.argumentType != OptionEntry::NoArgument)
                    shortOpts += QLatin1Char(':');
                shortMap[entry.shortName] = entry;
            }
            // Long options
            if (!entry.longName.isEmpty()) {
                entryMap[longOpts.size()] = entry;
                longOpts += LongOption(entry.longName,
                    entry.argumentType == OptionEntry::NoArgument
                        ? LongOption::NoArgument : LongOption::RequiredArgument,
                    NULL, 0);
                entryMap[longOpts.size()] = entry;
                longOpts += LongOption(
                    group.name + QLatin1Char('-') + entry.longName,
                    entry.argumentType == OptionEntry::NoArgument
                        ? LongOption::NoArgument : LongOption::RequiredArgument,
                    NULL, 0);
            }
        }
    }

    OptionParser getopt(args, shortOpts, longOpts);
    getopt.setShowErrors(false);
    QChar c;
    while ((c = getopt.nextOption ()) != -1) {
        Q_ASSERT(c != 1);
        if (c == QLatin1Char(':'))
            throw ApexStringException(tr("%1: option '%2' requires an argument")
                    .arg(args.value(0), QLatin1String("--") +
                        shortMap[getopt.invalidOptionValue()].longName));
        if (c == QLatin1Char('?'))
            throw ApexStringException(tr("%1: unrecognized option '%2'")
                    .arg(args.value(0), getopt.invalidOptionValue() == 0 ?
                        args[getopt.currentOptionIndex() - 1] :
                        QString(getopt.invalidOptionValue())));
        if (c == QLatin1Char('h')) {
            printShortHelp(args.value(0));
            exit(0);
        }
        if (c == 2) {
            printAllHelp(args.value(0));
            exit(0);
        }
        const int longOptIndex = getopt.currentLongOptionIndex();
        if (helpMap.contains(longOptIndex)) {
            printSpecialHelp(args.value(0), helpMap[longOptIndex]);
            exit(0);
        }
        OptionEntry entry = longOptIndex < 0 ? shortMap[c] : entryMap[longOptIndex];
        if (entry.argumentType == OptionEntry::NoArgument)
            entry.value->setValue(!entry.flags.testFlag(OptionEntry::ReverseOption));
        else
            entry.value->setValue(getopt.currentOptionArgument());
    }

    return getopt.arguments().mid(getopt.currentOptionIndex());
}

void OptionContext::printAllHelp(const QString &program) const
{
    printHelpUsage(program);
    printHelpHelp();
    QString result;
    QList<OptionGroup> allGroups(groups);
    allGroups += mainGroup;
    Q_FOREACH (const OptionGroup &group, allGroups)
        result += printHelpGroup(group);
    printHelpDescription(result);
}

void OptionContext::printShortHelp(const QString &program) const
{
    printHelpUsage(program);
    printHelpHelp();
    const QString description = printHelpGroup(mainGroup);
    printHelpDescription(description);
}

void OptionContext::printSpecialHelp(const QString &program, const OptionGroup &group) const
{
    printHelpUsage(program);
    const QString description = printHelpGroup(group);
    printHelpDescription(description);
}

void OptionContext::printHelpUsage(const QString &program) const
{
    std::cout << qPrintable(tr("Usage:")) << std::endl;
    std::cout << "  " << qPrintable(program)
        << " " << qPrintable(description) << "\n" << std::endl;
}

void OptionContext::printHelpHelp() const
{
    if (!helpEnabled)
        return;

    printHelpHeading(tr("Help Options"));
    printHelpEntry(QLatin1String("help"), QLatin1Char('h'),
            tr("Show help options"), QString());
    if (!groups.isEmpty()) {
        printHelpEntry(QLatin1String("help-all"), 0,
                tr("Show all help options"), QString());
        Q_FOREACH (const OptionGroup &group, groups)
            printHelpEntry(QLatin1String("help-") + group.name, 0,
                    group.helpDescription, QString());
    }
    std::cout << std::endl;
}

void OptionContext::printHelpHeading(const QString &header) const
{
    std::cout << qPrintable(header) << ":" << std::endl;
}

QString OptionContext::formatDescription(const QString &description,
        unsigned leftIndent) const
{
    if (description.isEmpty())
        return QString();

    QRegExp indentExpression(QString::fromLatin1("^\\s*"));
    QRegExp linePart;

    QString result;
    QString descriptionText = description;
    bool newLine = true;
    unsigned indent;
    while (!descriptionText.isEmpty()) {
        if (newLine) {
            indentExpression.indexIn(descriptionText);
            indent = indentExpression.matchedLength() + leftIndent;
            descriptionText = descriptionText.mid(indentExpression.matchedLength());
            linePart.setPattern(QString::fromLatin1("^[^\n]{0,%1}[\n\\s]|[^\n]{0,%1}").arg(totalLength - indent));
            newLine = false;
        }
        if (!result.isEmpty())
            result += QString(indent, QLatin1Char(' '));
        linePart.indexIn(descriptionText);
        result += descriptionText.left(linePart.matchedLength());
        descriptionText = descriptionText.mid(linePart.matchedLength());
        if (result.right(1) == QLatin1String("\n"))
            newLine = true;
        else
            result += QLatin1Char('\n');
    }

    return result.left(result.size() - 1);
}

void OptionContext::printHelpEntry(const OptionEntry &entry) const
{
    if (!entry.flags.testFlag(OptionEntry::HiddenOption))
        printHelpEntry(entry.longName, entry.shortName,
                entry.description, entry.argDescription);
}

void OptionContext::printHelpEntry(const QString &longName, const QChar &shortName,
        const QString &description, const QString &argDescription) const
{
    QString result;
    result += QLatin1String("  ");
    if (shortName == 0)
        result += QLatin1String("    ");
    else
        result += QLatin1Char('-') + shortName +
            (longName.isEmpty() ? QLatin1String(" ") : QLatin1String(", "));
    if (!longName.isEmpty())
        result += QLatin1String("--") + longName + QLatin1Char(' ');
    if (!argDescription.isEmpty())
        result += argDescription + QLatin1Char(' ');
    result = result.leftJustified(optionLength);
    if (result.size() > optionLength)
        result += QLatin1Char('\n') + QString(optionLength, QLatin1Char(' '));

    result += formatDescription(description, optionLength);

    std::cout << qPrintable(result) << std::endl;
}

QString OptionContext::printHelpGroup(const OptionGroup &group) const
{
    printHelpHeading(group.description);
    Q_FOREACH (const OptionEntry &entry, group.entries)
        printHelpEntry(entry);
    std::cout << std::endl;
    if (group.longDescription.isEmpty())
        return QString();
    return group.longDescription + QLatin1Char('\n');
}

void OptionContext::printHelpDescription(const QString &description) const
{
    if (description.isEmpty())
        return;
    std::cout << qPrintable(formatDescription(description, 0)) << std::endl;
}

} // namespace apex
