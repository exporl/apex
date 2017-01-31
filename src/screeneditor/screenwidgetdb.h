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
 
#ifndef SCREENWIDGETDB_H
#define SCREENWIDGETDB_H

#include <QList>

class QAction;

namespace apex
{
  namespace editor
  {
    class ScreenEditor;

    /**
     * The ScreenWidgetDB class contains information about the
     * possible "add widget" and "add layout" actions in a
     * ScreenEditor.  It keeps a list of both kinds of actions.
     */
    class ScreenWidgetDB
    {
      ScreenEditor* editor;
      QList<QAction*> addWidgetActions;
      QList<QAction*> addLayoutActions;
    public:
      ScreenWidgetDB( ScreenEditor* editor );
      QList<QAction*> getAddWidgetActions();
      QList<QAction*> getAddLayoutActions();
    };
  }
}
#endif
