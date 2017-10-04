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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_EXPERIMENTFILEMANAGER_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_EXPERIMENTFILEMANAGER_H_

#include <QObject>
#include <memory>
#include <vector>

namespace apex
{
namespace data
{
class Screen;
}
namespace editor
{
using data::Screen;

class ScreenEditor;
class ScreenEditorExperimentData;

/**
 * The ExperimentFileManager class is responsible in the
 * screeneditor program for managing an Apex experiment file.  It
 * is created with a given file.
 *
 * This class uses the \ref ScreenEditorExperimentFileParser for
 * parsing and saving the experiment file.
 *
 * This class can open \ref ScreenEditor's for the screens in the
 * Experiment, and connects signals on the ScreenEditors to
 * appropriate slots, so that screens and the experiment can be
 * properly saved, and a screen can be added.
 */
class ExperimentFileManager : public QObject
{
    Q_OBJECT
    QScopedPointer<ScreenEditorExperimentData> data;
    QString filename;
    std::vector<ScreenEditor *> editors;

public:
    ExperimentFileManager(QObject *parent, const QString &file);
    ~ExperimentFileManager();

    /**
     * Open a \ref ScreenEditor for every screen in the experiment,
     * and connect its relevant signals to the proper slots in this
     * class.
     */
    void openScreenEditors();
private slots:
    void newScreen();
    void saveScreen(ScreenEditor *se, Screen *s);
    void saveAllAs();
    void saveAllScreens();

private:
    void showScreenEditor(Screen *s);
};
}
}
#endif
