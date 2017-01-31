/******************************************************************************
 * Copyright (C) 2010  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * Based on Java port of getOpt() by Aaron M. Renn <arenn@urbanophile.com>.   *
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

#ifndef _RBA_SRC_LIB_PLUMBING_OPTPARSER_H_
#define _RBA_SRC_LIB_PLUMBING_OPTPARSER_H_

#include "global.h"

#include <QCoreApplication>
#include <QList>
#include <QString>
#include <QStringList>

namespace apex
{

class APEXTOOLS_EXPORT LongOption
{
public:
    enum ArgumentType {
        NoArgument,
        RequiredArgument,
        OptionalArgument
    };

    QString name;
    ArgumentType argumentType;
    // value stored in "val" stored here when this long option is encountered. If
    // this is NULL, the value stored in "val" is treated as the name of an
    // equivalent short option.
    QString *flag;
    // value to store in "flag" if flag is not NULL, otherwise the equivalent
    // short option character for this long option.
    QChar val;

public:
    /* Create a new LongOption object with the given parameter values.
    *
    * @param name The long option String.
    * @param argumentType Indicates whether the option has no argument
    * (NoArgument), a required argument (RequiredArgument) or an optional
    * argument (OptionalArgument).
    * @param flag If non-NULL, this is a location to store the value of "val" when
    * this option is encountered, otherwise "val" is treated as the equivalent
    * short option character.
    * @param val The value to return for this long option, or the equivalent single
    * letter option to emulate if flag is NULL.
    */
    LongOption(const QString &name, ArgumentType argumentType, QString *flag,
            QChar val) :
        name(name),
        argumentType(argumentType),
        flag(flag),
        val(val)
    {
    }
};

class OptionParserPrivate;

class APEXTOOLS_EXPORT OptionParser
{
    Q_DECLARE_TR_FUNCTIONS(OptionParser)
public:
    /**
     * Construct a Getopt instance with given input data that is capable of
     * parsing long options and short options.  Contrary to what you might
     * think, the flag 'longOnly' does not determine whether or not we scan for
     * only long arguments.  Instead, a value of true here allows long arguments
     * to start with a '-' instead of '--' unless there is a conflict with a
     * short option name.
     *
     * @param argv The String array passed as the command line to the program
     * @param optString A String containing a description of the valid short
     * args for this program
     * @param longOptions An array of LongOption objects that describes the valid
     * long args for this program
     * @param longOnly true if long options that do not conflict with short
     * options can start with a '-' as well as '--'
     */
    OptionParser(const QStringList &argv, const QString &optString,
            const QList<LongOption> &longOptions = QList<LongOption>(),
            bool longOnly = false);
    virtual ~OptionParser();

    unsigned currentOptionIndex() const;
    // reshuffled arguments
    QStringList arguments() const;
    /** When `nextOption' finds an option that takes an argument, the argument value
    * is returned here. Also, when `ordering' is ReturnInOrder, each non-option
    * ARGV-element is returned here. */
    QString currentOptionArgument() const;
    void setShowErrors(bool value);
    /** When nextOption() encounters an invalid option, it stores the value of
     * that option in optOpt which can be retrieved with this method. */
    QChar invalidOptionValue() const;
    /** Returns the index into the array of long options (NOT argv) representing
     * the long option that was found. */
    int currentLongOptionIndex() const;
    /** Returns a char that is the current option that has been
     * parsed from the command line.  If the option takes an argument it can be
     * obtained from currentOptionArgument() (empty otherwise). If an invalid
     * option is found, an error message is printed and a '?' is returned.  The
     * name of the invalid option character can be retrieved by calling the
     * invalidOptionValue() method.  When there are no more options to be
     * scanned, this method returns -1. The index of first non-option element in
     * arguments() can be retrieved with the currentOptionIndex() method.
     *
     * @return Various things as described above
     */
    QChar nextOption();

private:
    OptionParserPrivate * const d;

};

} // namespace rba

#endif
