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

#include "apexdata/screen/matrixelement.h"

#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "screen/screenrundelegate.h"

#include "matrixrundelegate.h"

#include <QButtonGroup>
#include <QFont>
#include <QFontMetrics>
#include <QPushButton>

namespace apex
{

namespace rundelegates
{

const ScreenElement *MatrixRunDelegate::getScreenElement() const
{
    return element;
}

QWidget *MatrixRunDelegate::getWidget()
{
    return this;
}

bool MatrixRunDelegate::hasText() const
{
    return true;
}

bool MatrixRunDelegate::hasInterestingText() const
{
    return true;
}

const QString MatrixRunDelegate::getText() const
{
    // create string containing the user response
    QStringList result;

    for (unsigned i = 0, size = m_groups.size(); i < size; ++i) {
        const int id = m_groups[i]->checkedId();
        result << "<answer>" +
                      (id < 0 ? QString() : element->buttons()[i][id].name) +
                      "</answer>";
    }

    return result.join("\n");
}

void MatrixRunDelegate::connectSlots(gui::ScreenRunDelegate *d)
{
    connect(this, SIGNAL(answered(ScreenElementRunDelegate *)), d,
            SIGNAL(answered(ScreenElementRunDelegate *)));
}

void MatrixRunDelegate::sendAnsweredSignal(int button)
{
    Q_UNUSED(button);

    // qCDebug(APEX_RS, "MatrixRunDelegate::sendAnsweredSignal");
    // qCDebug(APEX_RS, "Number of buttongroups: %d", m_groups.size());

    if (element->autoContinue() == false)
        return;

    // check whether each buttongroup has an answer
    bool ok = true;
    for (unsigned i = 0, size = m_groups.size(); i < size; ++i) {
        // qCDebug(APEX_RS, "buttongroup %d: checkedid=%d",
        // i, m_groups[i]->checkedId());
        if (m_groups[i]->checkedId() == -1) {
            ok = false;
            break;
        }
    }

    if (!ok)
        return;

    qCDebug(APEX_RS, "MatrixRunDelegate: Emitting answered");
    Q_EMIT answered(this);
}

MatrixRunDelegate::MatrixRunDelegate(ExperimentRunDelegate *p_rd,
                                     QWidget *parent,
                                     const data::MatrixElement *e)
    : QWidget(parent), ScreenElementRunDelegate(p_rd, e), element(e)
{
    setObjectName(element->getID());

    makeMatrix();

    if (!e->getFGColor().isEmpty()) {
        QPalette palette;
        palette.setColor(QPalette::Active, QPalette::Button,
                         QColor(e->getFGColor()));
        setPalette(palette);
    }
}

void MatrixRunDelegate::makeMatrix()
{
    QGridLayout *layout = new QGridLayout(this);

    const data::MatrixElement::tMatrixButtons &buttons = element->buttons();

    // create one qbuttongroup per column
    m_groups.resize(buttons.size());

    qCDebug(APEX_RS, "Matrix size: (%d,%d)", buttons.size(), buttons[0].size());

    for (unsigned col = 0, size = buttons.size(); col < size; ++col) {
        QButtonGroup *bg = new QButtonGroup;
        m_groups[col] = bg;
        bg->setExclusive(true);

        connect(bg, SIGNAL(buttonClicked(int)), this,
                SLOT(sendAnsweredSignal(int)));

        for (unsigned row = 0, rowSize = buttons[col].size(); row < rowSize;
             ++row) {
            QPushButton *b = new QPushButton(buttons[col][row].text, this);
            b->setCheckable(true);
            // col_row as that makes the most sense for matrix tests
            b->setObjectName(QSL("%1_%2_%3")
                                 .arg(element->getID())
                                 .arg(col + 1)
                                 .arg(row + 1));
            b->setStyleSheet(element->getStyle());
            setCommonProperties(b);
            QFont font(b->font());
            if (element->getFontSize() != -1)
                font.setPointSize(element->getFontSize());
            b->setFont(font);

            layout->addWidget(b, row, col);
            bg->addButton(b, row);
        }
    }
}

void MatrixRunDelegate::clearText()
{
    for (unsigned i = 0, size = m_groups.size(); i < size; ++i) {
        if (QAbstractButton *button = m_groups[i]->checkedButton()) {
            m_groups[i]->setExclusive(false);
            button->setChecked(false);
            m_groups[i]->setExclusive(true);
        }
    }
}
}
}
