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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_FLASHPLAYERELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_FLASHPLAYERELEMENT_H_

#include "screenelement.h"

#include <QString>

#include "apextools/global.h"

namespace apex
{
namespace data
{

/**
 * The FlashPlayerElement is an implementation of a \ref
 * ScreenElement representing a "flash player".
 */
class APEXDATA_EXPORT FlashPlayerElement : public ScreenElement
{
    QString defaultPath;
    QString highlightPath;
    QString positivePath;
    QString negativePath;
    bool overrideFeedback;

public:
    FlashPlayerElement(const QString &id, ScreenElement *parent);
    ~FlashPlayerElement();
    void visit(ScreenElementVisitor *v);
    void visit(ScreenElementVisitor *v) const;
    ElementTypeT elementType() const;

    QString getDefault() const;
    QString getHighlight() const;
    QString getPositive() const;
    QString getNegative() const;
    bool getOverrideFeedback() const;
    void setDefault(const QString &p);
    void setPositive(const QString &p);
    void setNegative(const QString &p);
    void setHighlight(const QString &p);
    void setOverrideFeedback(bool o);

    bool operator==(const FlashPlayerElement &other) const;
};
}
}

#endif
