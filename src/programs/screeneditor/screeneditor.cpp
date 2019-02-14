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

#include "apexdata/screen/emptyelement.h"
#include "apexdata/screen/gridlayoutelement.h"
#include "apexdata/screen/screen.h"

#include "apexmain/gui/guidefines.h"

#include "apextools/apextypedefs.h"

#include "experimentfilemanager.h"
#include "propertieseditor.h"
#include "screeneditor.h"
#include "screenelementcreator.h"
#include "screenelementeditordelegate.h"
#include "screenwidget.h"
#include "screenwidgetdb.h"

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>

namespace apex
{
namespace editor
{
using gui::sc_sDefaultFont;
using gui::sc_nDefaultFontSize;

ScreenEditor::ScreenEditor(QWidget *parent, Screen *screen)
    : QMainWindow(parent), selectedWidget(0), selectedDelegate(0)
{
    if (screen == 0)
        screen = createDefaultScreen();
    setWindowModality(Qt::WindowModal);
    QFont font(sc_sDefaultFont);
    font.setPointSize(sc_nDefaultFontSize);
    screenWidget = new ScreenWidget(this, screen, font);
    screenWidget->buildScreenReps();
    setCentralWidget(screenWidget);

    widgetDB = new ScreenWidgetDB(this);
    setupToolBoxes();
    setupActions();
    setWindowTitle(makeTitle());

    resize(900, 600);
}

void ScreenEditor::selectWidget(QFrame *widget,
                                ScreenElementEditorDelegate *del)
{
    if (selectedWidget) {
        // back to normal..
        selectedWidget->setFrameShadow(QFrame::Plain);
        selectedWidget->setFrameShape(QFrame::NoFrame);
        selectedWidget->setLineWidth(1);
    }
    if (widget) {
        Q_ASSERT(del);
        widget->setFrameShadow(QFrame::Sunken);
        widget->setFrameShape(QFrame::StyledPanel);
        widget->setLineWidth(2);
    }

    selectedWidget = widget;
    selectedDelegate = del;

    propeditor->selectWidget(selectedDelegate);
}

void ScreenEditor::setupToolBoxes()
{
    setupWidgetsToolBox();
    setupLayoutsToolBox();
    setupObjectPropertiesWidget();
}

void ScreenEditor::setupLayoutsToolBox()
{
    QToolBar *tb = new QToolBar(tr("Layouts"), this);
    addToolBar(Qt::LeftToolBarArea, tb);
    tb->show();
    tb->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QList<QAction *> actions = widgetDB->getAddLayoutActions();
    for (QList<QAction *>::iterator i = actions.begin(); i != actions.end();
         ++i)
        tb->addAction(*i);
}

void ScreenEditor::setupWidgetsToolBox()
{
    QToolBar *tb = new QToolBar(tr("Widgets"), this);
    addToolBar(Qt::LeftToolBarArea, tb);
    tb->show();
    tb->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QList<QAction *> actions = widgetDB->getAddWidgetActions();
    for (QList<QAction *>::iterator i = actions.begin(); i != actions.end();
         ++i)
        tb->addAction(*i);
}

void ScreenEditor::addWidgetClicked(ScreenElementCreator *r)
{
    setWindowModified(true);
    if (!selectedWidget) {
        QMessageBox::warning(this, tr("Error"),
                             tr("First select a place for the new %1..")
                                 .arg(r->getElementName()));
        delete r;
        return;
    }

    ScreenElementEditorDelegate *parentRep = 0;
    QWidget *cur = selectedWidget;
    while (true) {
        // continue until we find either a parentRep or notice that this is the
        // rootWidget
        cur = cur->parentWidget();
        if (cur == screenWidget)
            break;
        else {
            parentRep = dynamic_cast<ScreenElementEditorDelegate *>(cur);
            if (parentRep)
                break;
        }
    }
    Screen *screen = screenWidget->getScreen();
    if (!parentRep) {
        Q_ASSERT(cur == static_cast<ScreenWidget *>(screenWidget));
        Q_ASSERT(selectedDelegate->getScreenElement() ==
                 screenWidget->getScreen()->getRootElement());

        screenWidget->deleteDelegate(selectedDelegate);

        ScreenElement *newElem = r->create(0, screen);
        screen->addElement(newElem);
        screen->setRootElement(newElem);
        screenWidget->buildScreenReps();
        selectedDelegate = 0;
        selectedWidget = 0;
    } else {
        ScreenElement *newElem =
            r->create(parentRep->getScreenElement(), screen);
        screen->addElement(newElem);
        parentRep->replaceChild(selectedWidget, selectedDelegate, newElem);
        selectedDelegate = 0;
        selectedWidget = 0;
    }
    selectWidget(0, 0);
    delete r;
}

void ScreenEditor::setupObjectPropertiesWidget()
{
    QDockWidget *dw = new QDockWidget(tr("Object properties"), this);
    propeditor = new PropertiesEditor(dw);
    connect(propeditor, SIGNAL(modified()), this, SLOT(screenModified()));
    dw->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dw->setFeatures(QDockWidget::DockWidgetMovable |
                    QDockWidget::DockWidgetFloatable);
    dw->setWidget(propeditor);
    dw->setSizePolicy(
        QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    dw->setMinimumSize(150, 100);
    dw->setMaximumWidth(250);
    addDockWidget(Qt::RightDockWidgetArea, dw);
}

ScreenEditor::~ScreenEditor()
{
    delete widgetDB;
}

ScreenWidget *ScreenEditor::getScreenWidget()
{
    return screenWidget;
}

Screen *ScreenEditor::createDefaultScreen()
{
    using apex::data::GridLayoutElement;
    using apex::data::EmptyElement;

    QString screenid = "<Screen Name>";
    ScreenElement *rootElement = new GridLayoutElement(
        "gridlayout1", 0, 3, 2, data::tStretchList(), data::tStretchList());

    apex::gui::ScreenElementMap idToElementMap;

    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 2; ++y) {
            QString id = EmptyElement::findFreeID(idToElementMap);
            EmptyElement *e = new EmptyElement(id, rootElement);
            idToElementMap[id] = e;
            e->setX(x);
            e->setY(y);
        }
    }
    return new Screen(screenid, rootElement, idToElementMap);
}

PropertiesEditor *ScreenEditor::getPropertiesEditor()
{
    return propeditor;
}

void ScreenEditor::setupActions()
{

    QMenuBar *main = menuBar();
    QMenu *fileMenu = new QMenu(this);

    QAction *saveExperimentAction =
        new QAction(tr("Save &Experiment"), fileMenu);
    connect(saveExperimentAction, SIGNAL(triggered()), this,
            SIGNAL(saveAllScreens()));
    fileMenu->addAction(saveExperimentAction);

    QAction *saveExperimentAsAction =
        new QAction(tr("Save Experiment &As"), fileMenu);
    connect(saveExperimentAsAction, SIGNAL(triggered()), this,
            SIGNAL(saveAllAs()));
    fileMenu->addAction(saveExperimentAsAction);

    QAction *saveScreenAction = new QAction(tr("&Save Screen"), fileMenu);
    connect(saveScreenAction, SIGNAL(triggered()), this, SLOT(saveScreen()));
    fileMenu->addAction(saveScreenAction);

    QAction *newScreenAction = new QAction(tr("&New Screen"), fileMenu);
    connect(newScreenAction, SIGNAL(triggered()), this, SIGNAL(newScreen()));
    fileMenu->addAction(newScreenAction);

    fileMenu->setTitle("&file");
    main->addMenu(fileMenu);
    main->show();
}

void ScreenEditor::saveScreen()
{
    Q_EMIT saveScreen(this, getScreenWidget()->getScreen());
}

void ScreenEditor::setFile(const QString &f)
{
    file = f;
    setWindowTitle(makeTitle());
}

QString ScreenEditor::makeTitle()
{
    QString ret = "Apex Screen Editor";
    QString screenID = getScreenWidget()->getScreen()->getID();
    QString filepart = "[*]" + screenID + "[*]";
    if (!file.isEmpty())
        filepart += " ( " + file + " )";
    ret += " - " + filepart;
    return ret;
}

void ScreenEditor::screenModified()
{
    setWindowTitle(makeTitle());
    setWindowModified(true);
}
}
}
