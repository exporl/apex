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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_PROPERTIESEDITOR_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_PROPERTIESEDITOR_H_

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QTreeView>

namespace apex
{
namespace editor
{
class ScreenElementEditorDelegate;
class ObjectPropertiesModel;

class ObjectPropertiesModel;
class ObjectPropertiesItemDelegate;

/**
 * This class is a widget showing the properties of a certain \ref
 * ScreenElementEditorDelegate.  It also supports modifying the
 * properties.
 */
class PropertiesEditor : public QTreeView
{
    Q_OBJECT

    ScreenElementEditorDelegate *selectedRep;
    ObjectPropertiesModel *model;

public:
    PropertiesEditor(QWidget *parent);
    ~PropertiesEditor();

    void selectWidget(ScreenElementEditorDelegate *rep);
signals:
    void modified();
};
}
}
#endif
