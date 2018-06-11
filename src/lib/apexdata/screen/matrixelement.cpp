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

#include "matrixelement.h"

#include "screenelementvisitor.h"

#include <QObject>

namespace apex
{
namespace data
{

MatrixElement::MatrixElement(const QString &id, ScreenElement *parent)
    : ScreenElement(id, parent)
{
}

MatrixElement::~MatrixElement()
{
}

void MatrixElement::visit(ScreenElementVisitor *v)
{
    v->visitMatrix(this);
}

void MatrixElement::visit(ScreenElementVisitor *v) const
{
    v->visitMatrix(this);
}

ScreenElement::ElementTypeT MatrixElement::elementType() const
{
    return Matrix;
}

bool MatrixElement::MatrixButton::
operator==(const MatrixElement::MatrixButton &other) const
{
    return name == other.name && text == other.text;
}

bool MatrixElement::operator==(const MatrixElement &other) const
{
    return ScreenElement::operator==(other) &&
           m_autoContinue == other.m_autoContinue &&
           m_buttons == other.m_buttons;
}
}
}

void apex::data::MatrixElement::setButton(int row, int col,
                                          MatrixButton &button)
{
    qCDebug(APEX_RS, "MatrixElement::setButton at (%d,%d) to %s/%s", row, col,
            qPrintable(button.name), qPrintable(button.text));

    // FIXME: it's rather slow to resize for each new element
    if (col > m_buttons.size()) {
        /*QVector<MatrixButton> newlist;
        m_buttons.insert(row, newlist );*/
        m_buttons.resize(col);
    }

    if (row > m_buttons[col - 1].size())
        m_buttons[col - 1].resize(row);

    m_buttons[col - 1][row - 1] = button;
}

const apex::data::MatrixElement::tMatrixButtons &
apex::data::MatrixElement::buttons() const
{
    return m_buttons;
}

bool apex::data::MatrixElement::autoContinue() const
{
    return m_autoContinue;
}

void apex::data::MatrixElement::setAutoContinue(bool v)
{
    m_autoContinue = v;
}
