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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_TEXTEDITRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_TEXTEDITRUNDELEGATE_H_

#include "screenelementrundelegate.h"

#include <QLineEdit>

namespace apex
{
namespace data
{
class ScreenElement;
class TextEditElement;
}

namespace rundelegates
{
using data::TextEditElement;
using data::ScreenElement;

/**
 * The TextEditRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a TextEditElement.
 */
class TextEditRunDelegate : public QLineEdit, public ScreenElementRunDelegate
{
    Q_OBJECT

    const TextEditElement *element;

public:
    TextEditRunDelegate(ExperimentRunDelegate *p_exprd, QWidget *parent,
                        const TextEditElement *e, const QFont &defaultFont);

    const ScreenElement *getScreenElement() const;

    QWidget *getWidget();
    bool hasText() const;
    bool hasInterestingText() const;
    const QString getText() const;
    void setText(const QString &t);
    void clearText();
    void connectSlots(gui::ScreenRunDelegate *d);
signals:
    void answered(ScreenElementRunDelegate *);
public slots:
    void sendAnsweredSignal();
};
}
}

#endif
