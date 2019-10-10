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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLRUNDELEGATE_H_

#include "htmlapi.h"
#include "screenelementrundelegate.h"

#include <QResizeEvent>
#include <QScopedPointer>

// TODO: move code from rtresultsink into apexwebpage and reuse here

namespace apex
{
class ExperimentRunDelegate;
namespace data
{
class ScreenElement;
class HtmlElement;
}

namespace rundelegates
{
using data::HtmlElement;
using data::ScreenElement;

class HtmlRunDelegatePrivate;

/**
 * The HtmlRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a HtmlElement.
 */
class HtmlRunDelegate : public QObject, public ScreenElementRunDelegate
{
    Q_OBJECT

public:
    HtmlRunDelegate(ExperimentRunDelegate *p_rd, QWidget *parent,
                    const HtmlElement *e);
    virtual ~HtmlRunDelegate();

    const ScreenElement *getScreenElement() const Q_DECL_OVERRIDE;

    QWidget *getWidget() Q_DECL_OVERRIDE;
    bool hasText() const Q_DECL_OVERRIDE;
    bool hasInterestingText() const Q_DECL_OVERRIDE;
    const QString getText() const Q_DECL_OVERRIDE;
    void connectSlots(gui::ScreenRunDelegate *d) Q_DECL_OVERRIDE;
    void feedBack(const FeedbackMode &mode) Q_DECL_OVERRIDE;
    void enable();
    void setEnabled(bool enable) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void answered(ScreenElementRunDelegate *);

protected:
    void resizeEvent(QResizeEvent *e);
    void changeEvent(QEvent *event);
public Q_SLOTS:
    void collectAnswer(const QString &answer);

private:
    const HtmlElement *element;
    HtmlRunDelegatePrivate *const d;
};
}
}

#endif
