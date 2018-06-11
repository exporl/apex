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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_EMPTYELEMENTEDITORDELEGATE_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_EMPTYELEMENTEDITORDELEGATE_H_

#include <QLabel>

#include "labeleditordelegatebase.h"

namespace apex
{
namespace data
{
class EmptyElement;
}

namespace editor
{
using namespace apex::data;

/**
 * This class is an ScreenElementEditorDelegate representing a
 * EmptyElement.
 */
class EmptyElementEditorDelegate : public LabelEditorDelegateBase
{
    EmptyElement *element;

public:
    EmptyElementEditorDelegate(EmptyElement *e, QWidget *parent,
                               ScreenWidget *w);
    ~EmptyElementEditorDelegate();
    ScreenElement *getScreenElement();

    const QString getText() const;

    int getPropertyCount();
    QString getPropertyName(int nr);
    QVariant getPropertyData(int nr, int role);
    PropertyType getPropertyType(int nr);
    bool setProperty(int nr, const QVariant &v);
};
}
}
#endif
