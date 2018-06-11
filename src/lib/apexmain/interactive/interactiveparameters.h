/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_INTERACTIVE_INTERACTIVEPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXMAIN_INTERACTIVE_INTERACTIVEPARAMETERS_H_

#include "apexdata/interactive/parameterdialogresults.h"

#include "apextools/global.h"

#include "apextools/xml/xmltools.h"

namespace apex
{
class XPathProcessor;

class APEX_EXPORT InteractiveParameters
{
    Q_DECLARE_TR_FUNCTIONS(InteractiveParameters)

public:
    InteractiveParameters(const QDomDocument &document);

    enum ValueType {
        IntValue,
        DoubleValue,
        StringValue,
        BoolValue,
        ComboValue
    };

    struct Entry {
        Entry(const QString &xpath, const ValueType type,
              const QString &description, const QString &defaultvalue,
              const QRegExp &constraint)
            : xpath(xpath),
              type(type),
              description(description),
              defaultvalue(defaultvalue),
              result(QLatin1String("?")),
              succeeded(false),
              constraint(constraint)
        {
        }

        QString xpath;
        ValueType type;
        QString description;
        QString defaultvalue;
        QString result;
        bool succeeded;
        QRegExp constraint;
    };

    struct Callback {
        virtual void warning(const QString &message) = 0;
    };

    bool apply(const QStringList &entryResults, Callback *callback = 0);
    data::ParameterDialogResults
    applyExpressions(const QMap<QString, QString> &expressions);
    data::ParameterDialogResults *results() const;
    QList<Entry> entries() const;
    QDomDocument document() const;

private:
    QList<Entry> mEntries;

    QDomDocument document_;

    QString tempFileName() const;
    XPathProcessor *getXPathProcessor(QString *filename);
    void finishXPathProcessor(XPathProcessor *xpathProcessor,
                              const QString &filename);
};
}

#endif // _EXPORL_SRC_LIB_APEXMAIN_INTERACTIVE_INTERACTIVEPARAMETERS_H_
