/******************************************************************************
 * Copyright (C) 2010  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * Based on Java port of getopt() by Aaron M. Renn <arenn@urbanophile.com>.   *
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

#include "optionparser.h"

#include <iostream>

namespace apex
{

class OptionParserPrivate
{
public:
    QStringList exchange(const QStringList &argv);
    QChar checkLongOption();

    enum OptionOrder {
        // Options following non-option arguments are not recognized as options
        RequireOrder,
        // Options are reordered to have all non-option arguments at the end
        Permute,
        // Returns options in order
        ReturnInOrder
    };

    // argument of an option, non-option argument
    QString optArg;
    // current option index
    unsigned optInd;
    // show error messages
    bool optErr;
    // invalid option value
    QChar optOpt;
    // next char to be scanned
    QString nextChar;
    // valid short options
    QString optString;
    // valid long options
    QList<LongOption> longOptions;
    // only long options are parsed
    bool longOnly;
    // index into longOptions array of found long option
    int longInd;
    // strict POSIX compliance
    bool posixlyCorrect;
    // whether or not all necessary processing was done for the current option
    bool longOptHandled;
    // index of the first non-option
    unsigned firstNonOpt;
    // index of the last non-option
    unsigned lastNonOpt;
    // argument list passed to the program
    QStringList argv;
    // argument permutation mode
    OptionOrder ordering;

    // immediately return -1 the next time we are called
    bool endParse;
};

// OptionParserPrivate ===============================================================

/**
  * Exchange the shorter segment with the far end of the longer segment.
  * That puts the shorter segment into the right place.
  * It leaves the longer segment in the right place overall,
  * but it consists of two parts that need to be swapped next.
  * This method is used by nextOption() for argument permutation.
  */
QStringList OptionParserPrivate::exchange(const QStringList &value)
{
    QStringList result = value;

    unsigned bottom = firstNonOpt;
    unsigned middle = lastNonOpt;
    unsigned top = optInd;
    QString tem;

    while (top > middle && middle > bottom) {
        if (top - middle > middle - bottom) {
            // Bottom segment is the short one.
            const unsigned len = middle - bottom;

            // Swap it with the top part of the top segment.
            for (unsigned i = 0; i < len; ++i) {
                tem = result[bottom + i];
                result[bottom + i] = result[top - (middle - bottom) + i];
                result[top - (middle - bottom) + i] = tem;
            }
            // Exclude the moved bottom segment from further swapping.
            top -= len;
        } else {
            // Top segment is the short one.
            const unsigned len = top - middle;
            // Swap it with the bottom part of the bottom segment.
            for (unsigned i = 0; i < len; ++i) {
                tem = result[bottom + i];
                result[bottom + i] = result[middle + i];
                result[middle + i] = tem;
            }
            // Exclude the moved top segment from further swapping.
            bottom += len;
        }
    }

    // Update records for the slots the non-options now occupy.
    firstNonOpt += (optInd - lastNonOpt);
    lastNonOpt = optInd;

    return result;
}

/** Check to see if an option is a valid long option.  Called by nextOption(). Put
 * in a separate method because this needs to be done twice.  (The C getOpt
 * authors just copy-pasted the code!).
 *
 * @param longInd A buffer in which to store the 'val' field of found LongOption
 *
 * @return Various things depending on circumstances
 */
QChar OptionParserPrivate::checkLongOption()
{
    LongOption *pfound = NULL;
    bool ambig = false;
    bool exact = false;

    longOptHandled = true;
    ambig = false;
    exact = false;
    longInd = -1;

    int nameEnd = nextChar.indexOf(QLatin1Char('='));
    if (nameEnd == -1)
        nameEnd = nextChar.size();

    // Test all long options for either exact match or abbreviated matches
    for (unsigned i = 0; i < unsigned(longOptions.size()); ++i) {
        if (longOptions[i].name.startsWith(nextChar.left(nameEnd))) {
            if (longOptions[i].name == nextChar.left(nameEnd)) {
                // Exact match found
                pfound = &longOptions[i];
                longInd = i;
                exact = true;
                break;
            } else if (!pfound) {
                // First nonexact match found
                pfound = &longOptions[i];
                longInd = i;
            } else {
                // Second or later nonexact match found
                ambig = true;
            }
        }
    } // for

    // Print out an error if the option specified was ambiguous
    if (ambig && !exact) {
        if (optErr)
            std::cerr << qPrintable
                    (OptionParser::tr("%1: option '%2' is ambiguous")
                     .arg(argv.value(0), argv[optInd])) << std::endl;

        nextChar.clear();
        optOpt = 0;
        ++optInd;

        return QLatin1Char('?');
    }

    if (pfound) {
        ++optInd;

        if (nameEnd != nextChar.size()) {
            if (pfound->argumentType != LongOption::NoArgument) {
                if (nextChar.mid(nameEnd).size() > 1)
                    optArg = nextChar.mid(nameEnd + 1);
                else
                    optArg.clear();
            } else {
                if (optErr) {
                    // -- option
                    if (argv[optInd - 1].startsWith(QLatin1String("--")))
                        std::cerr << qPrintable
                                (OptionParser::tr("%1: option '--%2'"
                                        " doesn't allow an argument")
                                    .arg(argv.value(0), pfound->name)) << std::endl;
                    else
                        // +option or -option
                        std::cerr << qPrintable
                                (OptionParser::tr("%1: option '%2%3'"
                                        " doesn't allow an argument")
                                    .arg(argv.value(0), argv[optInd - 1].at(0),
                                        pfound->name)) << std::endl;
                }

                nextChar.clear();
                optOpt = pfound->val;

                return QLatin1Char('?');
            }
        } else if (pfound->argumentType == LongOption::RequiredArgument) {
            if (optInd < unsigned(argv.size())) {
                optArg = argv[optInd];
                ++optInd;
            } else {
                if (optErr)
                    std::cerr << qPrintable
                            (OptionParser::tr("%1: option '%2'"
                                              " requires an argument")
                             .arg(argv.value(0), argv[optInd - 1])) << std::endl;

                nextChar.clear();
                optOpt = pfound->val;
                if (optString[0] == QLatin1Char(':'))
                    return QLatin1Char(':');
                return QLatin1Char('?');
            }
        }

        nextChar.clear();

        if (pfound->flag) {
            *pfound->flag = pfound->val;
            return 0;
        }

        return pfound->val;
    }

    longOptHandled = false;

    return 0;
}

// OptionParser ======================================================================

OptionParser::OptionParser(const QStringList &argv, const QString &optString,
        const QList<LongOption> &longOptions, bool longOnly) :
    d(new OptionParserPrivate)
{
    d->optInd = 1;
    d->optErr = true;
    d->optOpt = QLatin1Char('?');
    d->optString = optString.isEmpty() ? QLatin1String(" ") : optString;
    d->longOptions = longOptions;
    d->longOnly = longOnly;
    d->longInd = -1;
    d->posixlyCorrect = !qgetenv("POSIXLY_CORRECT").isEmpty();
    d->longOptHandled = false;
    d->firstNonOpt = 1;
    d->lastNonOpt = 1;
    d->argv = argv;
    d->endParse = false;

    // Determine how to handle the ordering of options and non-options
    if (d->optString[0] == QLatin1Char('-')) {
        d->ordering = OptionParserPrivate::ReturnInOrder;
        d->optString = d->optString.mid(1);
    } else if (d->optString[0] == QLatin1Char('+')) {
        d->ordering = OptionParserPrivate::RequireOrder;
        d->optString = d->optString.mid(1);
    } else if (d->posixlyCorrect) {
        d->ordering = OptionParserPrivate::RequireOrder;
    } else {
        d->ordering = OptionParserPrivate::Permute; // The normal default case
    }

    d->optString = d->optString.isEmpty() ? QLatin1String(" ") : d->optString;
}

OptionParser::~OptionParser()
{
    delete d;
}

unsigned OptionParser::currentOptionIndex() const
{
    return d->optInd;
}

QStringList OptionParser::arguments() const
{
    return d->argv;
}

QString OptionParser::currentOptionArgument() const
{
    return d->optArg;
}

void OptionParser::setShowErrors(bool value)
{
    d->optErr = value;
}

QChar OptionParser::invalidOptionValue() const
{
    return d->optOpt;
}

int OptionParser::currentLongOptionIndex() const
{
    return d->longInd;
}

/**************************************************************************/

QChar OptionParser::nextOption()
{
    d->optArg.clear();

    if (d->endParse == true)
        return -1;

    if (d->nextChar.isEmpty()) {
        // If we have just processed some options following some non-options,
        //  exchange them so that the options come first.
        if (d->lastNonOpt > d->optInd)
            d->lastNonOpt = d->optInd;
        if (d->firstNonOpt > d->optInd)
            d->firstNonOpt = d->optInd;

        if (d->ordering == OptionParserPrivate::Permute) {
            // If we have just processed some options following some non-options,
            // exchange them so that the options come first.
            if ((d->firstNonOpt != d->lastNonOpt) && (d->lastNonOpt != d->optInd))
                d->argv = d->exchange(d->argv);
            else if (d->lastNonOpt != d->optInd)
                d->firstNonOpt = d->optInd;

            // Skip any additional non-options
            // and extend the range of non-options previously skipped.
            while ((d->optInd < unsigned(d->argv.size())) &&
                    (d->argv[d->optInd].isEmpty() ||
                     d->argv[d->optInd][0] != QLatin1Char('-') ||
                     d->argv[d->optInd] == QLatin1String("-")))
                d->optInd++;

            d->lastNonOpt = d->optInd;
        }

        // The special ARGV-element `--' means premature end of options.
        // Skip it like a NULL option,
        // then exchange with previous non-options as if it were an option,
        // then skip everything else like a non-option.
        if ((d->optInd != unsigned(d->argv.size())) && d->argv[d->optInd] == QLatin1String("--")) {
            d->optInd++;

            if ((d->firstNonOpt != d->lastNonOpt) && (d->lastNonOpt != d->optInd))
                d->argv = d->exchange(d->argv);
            else if (d->firstNonOpt == d->lastNonOpt)
                d->firstNonOpt = d->optInd;

            d->lastNonOpt = d->argv.size();

            d->optInd = d->argv.size();
        }

        // If we have done all the ARGV-elements, stop the scan
        // and back over any non-options that we skipped and permuted.
        if (d->optInd == unsigned(d->argv.size())) {
            // Set the next-arg-index to point at the non-options
            // that we previously skipped, so the caller will digest them.
            if (d->firstNonOpt != d->lastNonOpt)
                d->optInd = d->firstNonOpt;

            return -1;
        }

        // If we have come to a non-option and did not permute it,
        // either stop the scan or describe it to the caller and pass it by.
        if (d->argv[d->optInd].isEmpty() || (d->argv[d->optInd][0] != QLatin1Char('-')) ||
                d->argv[d->optInd] == QLatin1String("-")) {
            if (d->ordering == OptionParserPrivate::RequireOrder)
                return -1;

            d->optArg = d->argv[d->optInd++];
            return 1;
        }

        // We have found another option-ARGV-element.
        // Skip the initial punctuation.
        if (d->argv[d->optInd].startsWith(QLatin1String("--")))
            d->nextChar = d->argv[d->optInd].mid(2);
        else
            d->nextChar = d->argv[d->optInd].mid(1);
    }

    // Decode the current option-ARGV-element.

    /* Check whether the ARGV-element is a long option.

       If longOnly and the ARGV-element has the form "-f", where f is
       a valid short option, don't consider it an abbreviated form of
       a long option that starts with f.  Otherwise there would be no
       way to give the -f short option.

       On the other hand, if there's a long option "fubar" and
       the ARGV-element is "-fu", do consider that an abbreviation of
       the long option, just like "--fu", and not "-f" with arg "u".

       This distinction seems to be the most useful approach.  */
    if (!d->longOptions.isEmpty() && (d->argv[d->optInd].startsWith(QLatin1String("--"))
                || (d->longOnly && ((d->argv[d->optInd].size() > 2) ||
                        (d->optString.indexOf(d->argv[d->optInd][1]) == -1))))) {
        QChar c = d->checkLongOption();

        if (d->longOptHandled)
            return c;

        // Can't find it as a long option.  If this is not longOnly,
        // or the option starts with '--' or is not a valid short
        // option, then it's an error.
        // Otherwise interpret it as a short option.
        if (!d->longOnly || d->argv[d->optInd].startsWith(QLatin1String("--"))
                || (d->optString.indexOf(d->nextChar[0]) == -1)) {
            if (d->optErr) {
                if (d->argv[d->optInd].startsWith(QLatin1String("--")))
                    std::cerr << qPrintable
                            (OptionParser::tr("%1: unrecognized option '--%2'")
                             .arg(d->argv.value(0), d->nextChar)) << std::endl;
                else
                    std::cerr << qPrintable
                            (OptionParser::tr("%1: unrecognized option '%2%3'")
                             .arg(d->argv.value(0), d->argv[d->optInd].at(0),
                                 d->nextChar)) << std::endl;
            }

            d->nextChar.clear();
            ++d->optInd;
            d->optOpt = 0;

            return QLatin1Char('?');
        }
    } // if (longopts)

    // Look at and handle the next short option-character */
    QChar c = d->nextChar[0]; //**** Do we need to check for empty str?
    d->nextChar = d->nextChar.mid(1);

    QString temp;
    if (d->optString.indexOf(c) != -1)
        temp = d->optString.mid(d->optString.indexOf(c));

    if (d->nextChar.isEmpty())
        ++d->optInd;

    if (temp.isEmpty() || (c == QLatin1Char(':'))) {
        if (d->optErr) {
            if (d->posixlyCorrect)
                // 1003.2 specifies the format of this message
                std::cerr << qPrintable
                        (OptionParser::tr("%1: illegal option -- %2")
                         .arg(d->argv.value(0), c)) << std::endl;
            else
                std::cerr << qPrintable
                        (OptionParser::tr("%1: invalid option -- %2")
                         .arg(d->argv.value(0), c)) << std::endl;
        }

        d->optOpt = c;

        return QLatin1Char('?');
    }

    // Convenience. Treat POSIX -W foo same as long option --foo
    if ((temp[0] == QLatin1Char('W')) && (temp.size() > 1) && (temp[1] == QLatin1Char(';'))) {
        if (!d->nextChar.isEmpty())
            d->optArg = d->nextChar;
        // No further cars in this d->argv element and no more d->argv elements
        else if (d->optInd == unsigned(d->argv.size())) {
            if (d->optErr)
                // 1003.2 specifies the format of this message.
                std::cerr << qPrintable
                        (OptionParser::tr("%1: option requires an argument -- %2")
                         .arg(d->argv.value(0), c)) << std::endl;

            d->optOpt = c;
            if (d->optString[0] == QLatin1Char(':'))
                return QLatin1Char(':');
            return QLatin1Char('?');
        } else {
            // We already incremented `optInd' once;
            // increment it again when taking next ARGV-elt as argument.
            d->nextChar = d->argv[d->optInd];
            d->optArg  = d->argv[d->optInd];
        }

        c = d->checkLongOption();

        if (d->longOptHandled)
            return c;
        // Let the application handle it
        d->nextChar.clear();
        ++d->optInd;
        return QLatin1Char('W');
    }

    if ((temp.size() > 1) && (temp[1] == QLatin1Char(':'))) {
        if ((temp.size() > 2) && (temp[2] == QLatin1Char(':'))) {
            // This is an option that accepts and argument optionally
            if (!d->nextChar.isEmpty()) {
                d->optArg = d->nextChar;
                ++d->optInd;
            } else {
                d->optArg.clear();
            }

            d->nextChar.clear();
        } else {
            if (!d->nextChar.isEmpty()) {
                d->optArg = d->nextChar;
                ++d->optInd;
            } else if (d->optInd == unsigned(d->argv.size())) {
                if (d->optErr)
                    // 1003.2 specifies the format of this message
                    std::cerr << qPrintable
                            (OptionParser::tr("%1: option requires an argument -- %2")
                             .arg(d->argv.value(0), c)) << std::endl;

                d->optOpt = c;

                if (d->optString[0] == QLatin1Char(':'))
                    return QLatin1Char(':');
                return QLatin1Char('?');
            } else {
                d->optArg = d->argv[d->optInd];
                ++d->optInd;

                // Ok, here's an obscure Posix case.  If we have o:, and
                // we get -o -- foo, then we're supposed to skip the --,
                // end parsing of options, and make foo an operand to -o.
                // Only do this in Posix mode.
                if (d->posixlyCorrect && d->optArg == QLatin1String("--")) {
                    // If end of d->argv, error out
                    if (d->optInd == unsigned(d->argv.size())) {
                        if (d->optErr)
                            // 1003.2 specifies the format of this message
                            std::cerr << qPrintable
                                    (OptionParser::tr("%1: option requires an argument -- %2")
                                     .arg(d->argv.value(0), c)) << std::endl;

                        d->optOpt = c;

                        if (d->optString[0] == QLatin1Char(':'))
                            return QLatin1Char(':');
                        return QLatin1Char('?');
                    }

                    // Set new optArg and set to end
                    // Don't permute as we do on -- up above since we
                    // know we aren't in permute mode because of Posix.
                    d->optArg = d->argv[d->optInd];
                    ++d->optInd;
                    d->firstNonOpt = d->optInd;
                    d->lastNonOpt = d->argv.size();
                    d->endParse = true;
                }
            }

            d->nextChar.clear();
        }
    }

    return c;
}

} // namespace apex
