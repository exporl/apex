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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_MATRIXRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_MATRIXRUNDELEGATE_H_

#include "screenelementrundelegate.h"
#include <QWidget>

class QButtonGroup;

namespace apex
{
    class ExperimentRunDelegate;

namespace data
{
    class ScreenElement;
    class MatrixElement;
}

namespace rundelegates
{

/**
 * The MatrixRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a MatrixElement.
 */
class MatrixRunDelegate :
    public QWidget,
    public ScreenElementRunDelegate
{
    Q_OBJECT
public:
    MatrixRunDelegate(ExperimentRunDelegate* p_rd, QWidget* parent,
            const data::MatrixElement *e);

    const data::ScreenElement* getScreenElement() const;

    QWidget* getWidget();
    bool hasText() const;
    bool hasInterestingText() const;
    const QString getText() const;
    void clearText();
    void connectSlots(gui::ScreenRunDelegate *d);

public Q_SLOTS:
    void sendAnsweredSignal(int button);

Q_SIGNALS:
    void answered(ScreenElementRunDelegate*);

private:
    void makeMatrix();

    const data::MatrixElement *element;
    QVector<QButtonGroup*> m_groups;
};

}
}

#endif
