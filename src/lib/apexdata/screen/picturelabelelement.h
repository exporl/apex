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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_PICTURELABELELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_PICTURELABELELEMENT_H_

#include "screenelement.h"

#include <QString>

#include "apextools/global.h"

namespace apex
{
namespace data
{
/**
 * The PictureLabelElement is an implementation of a \ref
 * ScreenElement representing a "picture label".
 */
class APEXDATA_EXPORT PictureLabelElement : public ScreenElement
{
    QString picturePath;
    QString pictureDisabledPath;

public:
    PictureLabelElement(const QString &id, ScreenElement *parent);
    ~PictureLabelElement();

    ElementTypeT elementType() const;
    void visit(ScreenElementVisitor *v);
    void visit(ScreenElementVisitor *v) const;

    void setPicture(const QString &p);
    QString getPicture() const;

    void setPictureDisabled(const QString &p);
    QString getPictureDisabled() const;

    bool operator==(const PictureLabelElement &other) const;
};
}
}

#endif
