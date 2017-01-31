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

#ifndef _RBA_SRC_LIB_PLUMBING_OPTIONCONTEXT_H_
#define _RBA_SRC_LIB_PLUMBING_OPTIONCONTEXT_H_

#include "global.h"

#include <QCoreApplication>
#include <QSet>
#include <QSharedPointer>
#include <QStringList>
#include <QVariant>

namespace apex
{

class APEXTOOLS_EXPORT AbstractOptionValue
{
public:
    virtual ~AbstractOptionValue()
    {
    }

    virtual void setValue(const QVariant &value) = 0;
};

template<typename T, typename U>
class MapOptionValue : public AbstractOptionValue
{
public:
    MapOptionValue(QMap<T, U> *data) :
        data(data)
    {
    }

    virtual void setValue(const QVariant &value)
    {
        const QString stringValue = value.toString();
        const T k(QVariant(stringValue.section(QLatin1Char('='), 0, 0)).value<T>());
        const U v(QVariant(stringValue.section(QLatin1Char('='), 1)).value<U>());
        data->insert(k, v);
    }

private:
    QMap<T, U> *data;
};

template<typename T, typename U>
MapOptionValue<T, U> mapOptionValue(QMap<T, U> *data)
{
    return MapOptionValue<T, U>(data);
}

template<typename T>
class ListOptionValue : public AbstractOptionValue
{
public:
    ListOptionValue(QList<T> *data) :
        data(data)
    {
    }

    virtual void setValue(const QVariant &value)
    {
        data->append(value.value<T>());
    }

private:
    QList<T> *data;
};

template<typename T>
ListOptionValue<T> listOptionValue(QList<T> *data)
{
    return ListOptionValue<T>(data);
}

template<typename T>
class OptionValue : public AbstractOptionValue
{
public:
    OptionValue(T *data) :
        data(data)
    {
    }

    virtual void setValue(const QVariant &value)
    {
        *data = value.value<T>();
    }

private:
    T *data;
};

class APEXTOOLS_EXPORT OptionEntry
{
public:
    enum Flag {
        // option doesn't appear in --help output
        HiddenOption  = 0x01,
        // option appears in the main section
        MainOption    = 0x02,
        // reversed non-argument option
        ReverseOption = 0x04
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    enum ArgumentType {
        NoArgument,
        RequiredArgument
    };

    QString longName;
    QChar shortName;
    Flags flags;
    ArgumentType argumentType;
    QSharedPointer<AbstractOptionValue> value;
    QString description;
    QString argDescription;

public:
    OptionEntry()
    {
    }

    /** An OptionEntry defines a single option. To have an effect, they must be
     * added to an OptionGroup.
     *
     * @param longName The long name of an option can be used to specify it in
     * a commandline as --long_name. Every option must have a long name. To
     * resolve conflicts if multiple option groups contain the same long name,
     * it is also possible to specify the option as --groupname-long_name.
     * @param shortName If an option has a short name, it can be specified
     * -short_name in a commandline.
     * @param flags flags
     * @param arg type of the option
     * @param argValue object to store the data
     * @param description description for the option in --help output
     * @param argDescription placeholder to use for the extra argument parsed
     * by the option in --help output
     */
    template<class T>
    OptionEntry(const QString &longName, const QChar &shortName,
        Flags flags, ArgumentType argumentType, const T &value,
        const QString &description, const QString &argDescription = QString()) :
        longName(longName),
        shortName(shortName),
        flags(flags),
        argumentType(argumentType),
        value(new T(value)),
        description(description),
        argDescription(argDescription)
    {
    }

    template<typename T>
    OptionEntry(const QString &longName, const QChar &shortName,
        Flags flags, ArgumentType argumentType, T *value,
        const QString &description, const QString &argDescription = QString()) :
        longName(longName),
        shortName(shortName),
        flags(flags),
        argumentType(argumentType),
        value(new OptionValue<T>(value)),
        description(description),
        argDescription(argDescription)
    {
    }
};

class APEXTOOLS_EXPORT OptionGroup
{
public:
    QString name;
    QString description;
    QString helpDescription;
    QString longDescription;
    QList<OptionEntry> entries;

    OptionGroup()
    {
    }

    /** Creates a new OptionGroup.
     *
     * @param name the name for the option group, this is used to provide help
     * for the options in this group with --help-name
     * @param description a description for this group to be shown in --help.
     * @param helpDescription a description for the --help-name option.
     * @param longDescription a string which is shown after all the other
     * options if these options are actually displayed.
     * @param entries the commandline options belonging to this group.
     */
    OptionGroup(const QString &name, const QString &description,
        const QString &helpDescription, const QString &longDescription,
        const QList<OptionEntry> &entries) :
        name(name),
        description(description),
        helpDescription(helpDescription),
        longDescription(longDescription),
        entries(entries)
    {
    }
};

class APEXTOOLS_EXPORT OptionContext
{
    Q_DECLARE_TR_FUNCTIONS(OptionContext)
private:
    const static int totalLength = 80;
    const static int optionLength = 30;
    QString description;
    bool helpEnabled;

    OptionGroup mainGroup;
    QList<OptionGroup> groups;

public:
    OptionContext(const QString &description,
        const OptionGroup &mainGroup,
        const QList<OptionGroup> &groups = QList<OptionGroup>());
    void addGroup(const OptionGroup &group);
    void setMainGroup(const OptionGroup &group);
    QStringList parse(const QStringList &args);

    void setHelpEnabled(bool value);
    bool isHelpEnabled() const;

    // TODO: manually print usage for option context

private:
    void printAllHelp(const QString &program) const;
    void printShortHelp(const QString &program) const;
    void printSpecialHelp(const QString &program, const OptionGroup &group) const;

    void printHelpUsage(const QString &program) const;
    void printHelpHeading(const QString &header) const;
    void printHelpHelp() const;
    void printHelpEntry(const OptionEntry &entry) const;
    void printHelpEntry(const QString &longName, const QChar &shortName,
            const QString &description, const QString &argDescription) const;
    QString printHelpGroup(const OptionGroup &group) const;
    void printHelpDescription(const QString &description) const;

    QString formatDescription(const QString &description, unsigned leftIndent) const;
};

} // namespace rba

#endif

