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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_MATRIXELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_MATRIXELEMENT_H_

#include "screenelement.h"

#include <QString>
#include <QVector>

#include "apextools/global.h"

namespace apex
{
namespace data
{
/**
 * The ButtonElement is an implementation of a \ref
 * ScreenElement representing a "button".
 */
class APEXDATA_EXPORT MatrixElement : public ScreenElement
{
public:
    struct MatrixButton {
        QString name;
        QString text;

        MatrixButton(const QString &n, const QString t) : name(n), text(t){};

        MatrixButton(){};

        bool operator==(const MatrixButton &other) const;
    };

    typedef QVector<QVector<MatrixButton>> tMatrixButtons;

    MatrixElement(const QString &id, ScreenElement *parent);
    ~MatrixElement();

    ElementTypeT elementType() const;
    void visit(ScreenElementVisitor *v);
    void visit(ScreenElementVisitor *v) const;

    /**
     * row and col are 1 based
     */
    void setButton(int row, int col, MatrixButton &button);
    const tMatrixButtons &buttons() const;

    void setAutoContinue(bool v);
    bool autoContinue() const;

    bool operator==(const MatrixElement &other) const;

private:
    tMatrixButtons m_buttons;
    bool m_autoContinue;
};
}
}
#endif
