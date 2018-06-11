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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENEDITOR_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENEDITOR_H_

#include <QMainWindow>

class QAction;
class QFrame;

namespace apex
{
namespace data
{
class Screen;
}

namespace editor
{
class PropertiesEditor;
class ScreenWidget;
class ScreenElementCreator;
class ScreenWidgetDB;

class ScreenElementEditorDelegate;

using apex::data::Screen;

class ScreenEditor : public QMainWindow
{
    Q_OBJECT
    ScreenWidget *screenWidget;

    QFrame *selectedWidget;
    ScreenElementEditorDelegate *selectedDelegate;
    PropertiesEditor *propeditor;
    ScreenWidgetDB *widgetDB;
    QString file;

    void setupWidgetsToolBox();
    void setupLayoutsToolBox();
    void setupObjectPropertiesWidget();
    void setupToolBoxes();

    void setupActions();
private slots:
    void saveScreen();
    void screenModified();

public:
    ScreenEditor(QWidget *parent = 0, Screen *screen = 0);
    ~ScreenEditor();
    ScreenWidget *getScreenWidget();

    PropertiesEditor *getPropertiesEditor();

    void selectWidget(QFrame *widget, ScreenElementEditorDelegate *rep);

    void addWidgetClicked(ScreenElementCreator *rep);

    static Screen *createDefaultScreen();

    // set the filename, which is shown in the title bar...
    void setFile(const QString &f);
    QString makeTitle();
signals:
    void newScreen();
    void saveScreen(ScreenEditor *, Screen *s);
    void saveAllAs();
    void saveAllScreens();
};
}
}
#endif
