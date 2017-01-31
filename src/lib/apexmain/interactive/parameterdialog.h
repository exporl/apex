/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef APEXPARAMETERDIALOG_H
#define APEXPARAMETERDIALOG_H

#include <QDialog>
#include <QList>

#include "xml/xercesinclude.h"

#include <memory>

namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
class DOMDocument;
};

namespace Ui
{
class ParameterDialog;
}

namespace apex
{

namespace data
{
class ParameterDialogResults;
}

class ParameterDialog : public QDialog
{
    Q_OBJECT
public:
    enum ValueType {
        IntValue,
        DoubleValue,
        StringValue,
        BoolValue,
        ComboValue
    };

    class InteractiveEntry
    {
    public:
        InteractiveEntry(const QString &xpath, const int type,
                const QString &description, const QString &defaultvalue) :
            xpath(xpath),
            type(type),
            description(description),
            defaultvalue(defaultvalue),
            result(QLatin1String("fromdialog")),
            succeeded(false)
        {
        }

        QString xpath;
        int type;
        QString description;
        QString defaultvalue;
        QString result;
        bool succeeded;
    };

public:
    ParameterDialog(xercesc::DOMDocument *document, QWidget *parent = NULL);
    virtual ~ParameterDialog();

    data::ParameterDialogResults* results() const;
    void apply();

    const QList<InteractiveEntry>& entries() const;

private:
    void processXml();
    void buildWidgets();
    bool dialogResult();

    QList<InteractiveEntry> mEntries;
    QList<QWidget*> widgets;
    bool doApply;
    std::auto_ptr<Ui::ParameterDialog> ui;

    xercesc::DOMDocument * const document;
};

}

#endif
