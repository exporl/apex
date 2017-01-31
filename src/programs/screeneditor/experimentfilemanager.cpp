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

#include "apexdata/screen/screensdata.h"

#include "experimentfilemanager.h"
#include "screeneditor.h"
#include "screeneditorexperimentfileparser.h"
#include "screenwidget.h"

#include <QFileDialog>

#include <QtGlobal>

namespace apex
{
namespace editor
{
ScreenEditorExperimentFileParser& parserInstance()
{
    ScreenEditorExperimentFileParser::schemaLoc = "/home/job/apex/data/schemas/experiment.xsd";
    static ScreenEditorExperimentFileParser parser;
    return parser;
}

ExperimentFileManager::ExperimentFileManager( QObject* parent, const QString& file )
    : QObject( parent ),
    filename( file )
{
    QFileInfo fileinfo( file );
    data.reset(parserInstance().parse(file));
    Q_ASSERT(data);
}

ExperimentFileManager::~ExperimentFileManager()
{
}

void ExperimentFileManager::openScreenEditors()
{
    ScreensData::ScreenMap& screens = data->screens->getScreens();
    for ( ScreensData::ScreenMap::iterator i = screens.begin();
        i != screens.end(); ++i )
    {
    Screen* s = i->second;
    showScreenEditor( s );
    }
}

void ExperimentFileManager::saveAllScreens()
{
    for ( std::vector<ScreenEditor*>::iterator i = editors.begin();
        i != editors.end(); ++i )
    {
    data->storeScreenChanges( ( *i )->getScreenWidget()->getScreen() );
    ( *i )->setWindowModified( false );
    }
    parserInstance().save( data.data(), filename );
}

void ExperimentFileManager::saveScreen( ScreenEditor* se, Screen* s )
{
    data->storeScreenChanges( s );
    parserInstance().save( data.data(), filename );
    se->setWindowModified( false );
}

void ExperimentFileManager::saveAllAs()
{
    QString s = QFileDialog::getSaveFileName(
    static_cast<QWidget*>( sender() ),
    tr( "Save All" ) );
    if ( !s.isEmpty() )
    {
    filename = s;
    saveAllScreens();
    }
}

void ExperimentFileManager::newScreen()
{
    Screen* s = ScreenEditor::createDefaultScreen();
    data->addScreen( s );
    showScreenEditor( s );
}

void ExperimentFileManager::showScreenEditor( Screen* screen )
{
    ScreenEditor* editor = new ScreenEditor( 0, screen );
    connect( editor, SIGNAL( saveScreen( ScreenEditor*, Screen* ) ),
            this, SLOT( saveScreen( ScreenEditor*, Screen* ) ) );
    connect( editor, SIGNAL( saveAllScreens() ),
            this, SLOT( saveAllScreens() ) );
    connect( editor, SIGNAL( saveAllAs() ),
            this, SLOT( saveAllAs() ) );
    connect( editor, SIGNAL( newScreen() ),
            this, SLOT( newScreen() ) );
    editor->setFile( filename );
    editor->show();
    editors.push_back( editor );
}

}
}
