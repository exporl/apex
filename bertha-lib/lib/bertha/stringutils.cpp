/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
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

#include "stringutils.h"
#include <QMetaEnum>

namespace bertha
{

QString stringListToString(const QStringList value)
{
    QString result;

    for (unsigned i = 0; i < unsigned(value.size()); ++i) {
        if (i > 0)
            result += QLatin1Char(',');
        result += QString::fromLatin1("\"");
        for (unsigned j = 0; j < unsigned(value[i].size()); ++j) {
            char c = value[i].at(j).toLatin1();
            switch (c) {
            case '\a':
                result += QString::fromLatin1("\\a");
                break;
            case '\b':
                result += QString::fromLatin1("\\b");
                break;
            case '\f':
                result += QString::fromLatin1("\\f");
                break;
            case '\n':
                result += QString::fromLatin1("\\n");
                break;
            case '\r':
                result += QString::fromLatin1("\\r");
                break;
            case '\t':
                result += QString::fromLatin1("\\t");
                break;
            case '\v':
                result += QString::fromLatin1("\\v");
                break;
            case '"':
                result += QString::fromLatin1("\\\"");
                break;
            case '\\':
                result += QString::fromLatin1("\\\\");
                break;
            case '\'':
                result += QString::fromLatin1("\\\'");
                break;
            default:
                result += QChar::fromLatin1(c);
            }
        }
        result += QString::fromLatin1("\"");
    }
    return result;
}

QStringList stringToStringList(const QString value)
{
    static const char escapeCodes[][2] = {
        {'a', '\a'}, {'b', '\b'},  {'f', '\f'}, {'n', '\n'},
        {'r', '\r'}, {'t', '\t'},  {'v', '\v'}, {'"', '"'},
        {'?', '?'},  {'\'', '\''}, {'\\', '\\'}};
    static const int numEscapeCodes =
        sizeof(escapeCodes) / sizeof(escapeCodes[0]);

    int from = 0;
    int to = value.length();

    QByteArray str = value.toLatin1();
    QString stringResult;
    QStringList stringListResult;
    stringListResult.clear();

    bool inQuotedString = false;
    bool currentValueIsQuoted = false;
    int i = from;
    char ch;

    while (i < to) {
        switch (str.at(i)) {
        case '\\':
            ++i;
            if (i >= to)
                break;

            ch = str.at(i++);
            for (int j = 0; j < numEscapeCodes; ++j) {
                if (ch == escapeCodes[j][0]) {
                    stringResult += QLatin1Char(escapeCodes[j][1]);
                    break;
                }
            }
            if (ch == '\n' || ch == '\r') {
                if (i < to) {
                    char ch2 = str.at(i);
                    // \n, \r, \r\n, and \n\r are legitimate line terminators
                    if ((ch2 == '\n' || ch2 == '\r') && ch2 != ch)
                        ++i;
                }
            } else {
                // the character is skipped
            }
            break;
        case '"':
            ++i;
            currentValueIsQuoted = true;
            inQuotedString = !inQuotedString;
            if (!inQuotedString)
                // skip spaces
                while (i < to && ((ch = str.at(i)) == ' ' || ch == '\t'))
                    ++i;
            break;
        case ',':
            if (!inQuotedString) {
                if (!currentValueIsQuoted) {
                    int l = stringResult.size() - 1;
                    while (l >= 0 && (stringResult.at(l) == QLatin1Char(' ') ||
                                      stringResult.at(l) == QLatin1Char('\t')))
                        stringResult.truncate(l--);
                }

                stringResult.squeeze();
                stringListResult.append(stringResult);
                stringResult.clear();
                currentValueIsQuoted = false;
                ++i;
                // skip spaces
                while (i < to && ((ch = str.at(i)) == ' ' || ch == '\t'))
                    ++i;
            } else {
                ++i;
                stringResult += QLatin1Char(',');
            }
            break;
        default: {
            int j = i + 1;
            while (j < to) {
                ch = str.at(j);
                if (ch == '\\' || ch == '"' || ch == ',')
                    break;
                ++j;
            }

            int n = stringResult.size();
            stringResult.resize(n + (j - i));
            QChar *resultData = stringResult.data() + n;
            for (int k = i; k < j; ++k)
                *resultData++ = QLatin1Char(str.at(k));

            i = j;
        }
        }
    }

    if (!currentValueIsQuoted) {
        int l = stringResult.size() - 1;
        while (l >= 0 && (stringResult.at(l) == QLatin1Char(' ') ||
                          stringResult.at(l) == QLatin1Char('\t')))
            stringResult.truncate(l--);
    }
    stringListResult.append(stringResult);
    return stringListResult;
}

QVector<double> stringToVector(const QString value)
{
    unsigned l = value.length();
    unsigned pos = 0;
    QByteArray str = value.toLatin1();

    QVector<double> result;
    QString number;

    while (l > pos && str.at(pos) == ' ')
        ++pos;

    while (l > 0 && str.at(l - 1) == ' ')
        --l;

    if (!(l > 0) || str.at(pos) != '[' || str.at(l - 1) != ']')
        return result;

    for (++pos; pos < l; ++pos) {
        char c = str.at(pos);
        if (c == ']') {
            if (number.size() > 0) {
                result.append(number.toDouble());
                number.clear();
            }
        } else if (c == ' ') {
            result.append(number.toDouble());
            number.clear();
            while (str.at(pos + 1) == ' ')
                ++pos;
        } else {
            number.append(QChar::fromLatin1(c));
        }
    }

    return result;
}

QString vectorToString(const QVector<double> value)
{
    QString str;
    str.append(QLatin1Char('['));
    for (int i = 0; i < value.size() - 1; ++i) {
        str.append(QString::number(value[i]));
        str.append(QLatin1Char(' '));
    }
    str.append(QString::number(value[value.size() - 1]));
    str.append(QLatin1Char(']'));

    return str;
}

QVector<QVector<double>> stringToDoubleVector(const QString value)
{
    unsigned l = value.length();
    unsigned pos = 0;

    QByteArray str = value.toLatin1();

    QVector<QVector<double>> result;
    QVector<double> vecItem;
    QString number;

    while (l > pos && str.at(pos) == ' ')
        ++pos;

    while (l > 0 && str.at(l - 1) == ' ')
        --l;

    if (!(l > 0) || str.at(pos) != '[' || str.at(l - 1) != ']')
        return result;

    for (++pos; pos < l; ++pos) {
        char c = str.at(pos);
        if (c == ']') {
            if (number.size() > 0) {
                if (number.size() > 0) {
                    vecItem.append(number.toDouble());
                    number.clear();
                }
                result.append(vecItem);
                vecItem.clear();
            }
        } else if (c == '[') {
            vecItem.clear();
        } else if (c == ';') {
            if (vecItem.size() > 0) {
                if (number.size() > 0) {
                    vecItem.append(number.toDouble());
                    number.clear();
                }
                result.append(vecItem);
                vecItem.clear();
            }
        } else if (c == ' ') {
            if (number.size() > 0) {
                vecItem.append(number.toDouble());
                number.clear();
            }
        } else {
            number.append(QLatin1Char(c));
        }
    }

    return result;
}

QString doubleVectorToString(const QVector<QVector<double>> value)
{
    QString str = QString::fromLatin1("[");
    for (unsigned i = 0; i < unsigned(value.size()); ++i)
        str.append(vectorToString(value[i]));
    str.append(QString::fromLatin1("]"));
    return str;
}

int stringToEnumValue(const QString value, const QMetaEnum enumerator)
{
    int result = enumerator.keyToValue(value.toLatin1().constData());
    if (result != -1)
        return result;
    bool ok = true;
    result = value.toInt(&ok);
    if (ok)
        return result;
    else
        return 0;
}

QString enumValueToKey(const QMetaProperty &property, int value)
{
    Q_ASSERT_X(property.isEnumType(), "enumValueToKey",
               "Not an enum type: " + QByteArray(property.typeName()));
    QMetaEnum metaEnum = property.enumerator();
    return QString::fromLatin1(metaEnum.valueToKey(value));
}

} // namespace bertha
