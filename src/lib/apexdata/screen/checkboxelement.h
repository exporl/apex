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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_CHECKBOXELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_CHECKBOXELEMENT_H_

#include "screenelement.h"
#include <QString>
#include <Qt>

#include "apextools/global.h"

namespace apex
{
namespace data
{
/**
Impements checkboxes in Apex3
**/
class APEXDATA_EXPORT CheckBoxElement : public ScreenElement
{
    QString text;

public:
    CheckBoxElement(const QString &id, ScreenElement *parent);
    ~CheckBoxElement();

    ElementTypeT elementType() const;
    void visit(ScreenElementVisitor *v);
    void visit(ScreenElementVisitor *v) const;
    void setText(const QString &t);
    QString getText() const;
    Qt::CheckState getChecked() const;

    const QString &GetParameter() const
    {
        return parameter;
    };

    bool operator==(const CheckBoxElement &other) const;

private:
    QString parameter;
    Qt::CheckState isChecked;
};
}
}

#endif
