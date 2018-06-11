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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_BUTTONEDITORDELEGATE_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_BUTTONEDITORDELEGATE_H_

#include "screenelementeditordelegate.h"

#include <QFrame>

class QPushButton;

namespace apex
{
namespace data
{
class ButtonElement;
class ScreenElement;
}

using data::ButtonElement;
using data::ScreenElement;

namespace editor
{
/**
 * This class is an ScreenElementEditorDelegate representing a
 * ButtonElement.
 */
class ButtonEditorDelegate : public QFrame, public ScreenElementEditorDelegate
{
    Q_OBJECT

    ButtonElement *element;
    QPushButton *button;

public:
    ButtonEditorDelegate(ButtonElement *e, QWidget *parent, ScreenWidget *w);
    ~ButtonEditorDelegate();
    QFrame *getWidget();
    ScreenElement *getScreenElement();
    int getPropertyCount();
    QString getPropertyName(int nr);
    QVariant getPropertyData(int nr, int role);
    PropertyType getPropertyType(int nr);
    bool setProperty(int nr, const QVariant &v);

protected:
    void setText(const QString &s);
    const QString getText();

    bool eventFilter(QObject *o, QEvent *e);

    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void setButtonGeometry();
    QFont getFont();
    void resetText();
};
}
}
#endif
