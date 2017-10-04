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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENWIDGET_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENWIDGET_H_

#include "screentypedefs.h"

#include <QFont>
#include <QLabel>
#include <QString>
#include <QWidget>

class QPushButton;

namespace apex
{
namespace data
{
class Screen;
class ScreenEmptyElement;
}
namespace editor
{
class ScreenEditor;
class ScreenWidget;
class ScreenElementEditorDelegate;

using data::Screen;

class ScreenWidget : public QWidget
{
    Q_OBJECT

    ScreenEditor *editor;
    Screen *screen;
    QFont defaultFont;
    elementToDelegateMapT elementToDelegateMap;
    ScreenElementEditorDelegate *rootDelegate;

public:
    ScreenWidget(ScreenEditor *editor, Screen *screen, QFont defaultFont);
    ~ScreenWidget();
    void buildScreenReps();
    ScreenEditor *getEditor();
    elementToDelegateMapT &getElementToDelegateMap();

    ScreenElementEditorDelegate *getEditorDelegate(ScreenElement *el);
    Screen *getScreen();
    const Screen *getScreen() const;

    QFont getDefaultFont();

    void resizeEvent(QResizeEvent *e);

    void deleteDelegate(ScreenElementEditorDelegate *e,
                        bool deleteElementToo = true);
    // called by a delegate to inform the screenWidget of its death..
    void delegateDeleted(ScreenElementEditorDelegate *e);

private:
    void setRootDelegateGeometry();
};
}
}
#endif
