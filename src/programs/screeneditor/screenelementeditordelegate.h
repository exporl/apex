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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENELEMENTEDITORDELEGATE_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENELEMENTEDITORDELEGATE_H_

#include "properties.h"
#include "screentypedefs.h"

class QWidget;
class QString;
class QVariant;
class QMouseEvent;
class QFrame;

namespace apex
{
namespace data
{
class ScreenElement;
}
namespace editor
{
class ScreenWidget;
using data::ScreenElement;

/**
 * \class ScreenElementEditorDelegate ScreenElementEditorDelegate.h
 * screen/ScreenElementEditorDelegate.h
 *
 * The ScreenElementEditorDelegate class is a class that
 * represents a \ref ScreenElement towards the \ref ScreenEditor
 * class and associate classes.  It separates purely editor stuff
 * like editable properties, QWidget representation etc. from the
 * pure data.  This is done so that the data classes can be used
 * for other purposes as well.
 *
 * A related class is \ref ScreenElementRunDelegate, which has a
 * similar purpose, but towards a running apex program instead of
 * a ScreenEditor.
 *
 * Ownership of these EditorDelegates is managed by the class
 * creating them.  Normally this will be the class \ref
 * ScreenWidget, which deletes its RunDelegates in its destructor.
 * No class should ever delete a ScreenElementEditorDelegate
 * directly.  Instead, the ScreenWidget should be asked to delete
 * it by calling the method ScreenWidget::deleteDelegate().  This
 * will make the ScreenWidget delete the
 * ScreenElementEditorDelegate and its children, and update its
 * internal structures..
 *
 * This is an abstract base class.
 */
class ScreenElementEditorDelegate
{
protected:
    ScreenWidget *screenWidget;

public:
    ScreenElementEditorDelegate(ScreenWidget *w);

    virtual QFrame *getWidget() = 0;
    virtual ~ScreenElementEditorDelegate();
    virtual ScreenElement *getScreenElement() = 0;

    virtual int getPropertyCount() = 0;
    virtual QString getPropertyName(int nr) = 0;
    virtual QVariant getPropertyData(int nr, int role) = 0;
    virtual PropertyType getPropertyType(int nr) = 0;
    virtual bool setProperty(int nr, const QVariant &v);
    virtual void replaceChild(QWidget *oldChildWidget,
                              ScreenElementEditorDelegate *oldChildRep,
                              ScreenElement *newElement);

protected:
    void handleMouseReleaseEvent(QMouseEvent *me, QFrame *widget);
};
}
}
#endif
