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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_TEXTEDITEDITORDELEGATE_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_TEXTEDITEDITORDELEGATE_H_

#include "screenelementeditordelegate.h"

#include <QFrame>

class QLineEdit;

namespace apex
{
namespace data
{
class TextEditElement;
class ScreenElement;
}
namespace editor
{
using data::TextEditElement;
using data::ScreenElement;

/**
 * This class is an ScreenElementEditorDelegate representing a
 * TextEditElement.
 */
class TextEditEditorDelegate : public QFrame, public ScreenElementEditorDelegate
{
    Q_OBJECT

    TextEditElement *element;
    QLineEdit *lineedit;
    bool ignoretextchangedsignal;

public:
    TextEditEditorDelegate(TextEditElement *e, QWidget *parent,
                           ScreenWidget *w);
    ~TextEditEditorDelegate();

    QFrame *getWidget();
    ScreenElement *getScreenElement();

    void setInputMask(const QString &s);

    int getPropertyCount();
    QString getPropertyName(int nr);
    QVariant getPropertyData(int nr, int role);
    PropertyType getPropertyType(int nr);
    bool setProperty(int nr, const QVariant &v);
    void setText(const QString &s);
    bool eventFilter(QObject *o, QEvent *e);

public slots:
    void textEdited(const QString &s);

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

    QFont getFont();
};
}
}
#endif
