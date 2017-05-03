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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENEDITOREXPERIMENTFILEPARSER_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENEDITOREXPERIMENTFILEPARSER_H_

#include "apextools/xml/xmltools.h"

#include <QString>

#include <map>
#include <memory>

namespace apex
{
namespace data
{
class Screen;
class ScreensData;
}
namespace editor
{

class SEErrorHandler;

using data::Screen;
using data::ScreensData;

/**
    * The ScreenEditorExperimentData class contains data about an
    * Experiment file, as it is used by the screeneditor application.
    * Instances of this class are supposed to be created by the \ref
    * ScreenEditorExperimentFileParser class.
    */
class ScreenEditorExperimentData
{
    friend class ScreenEditorExperimentFileParser;

    typedef std::map<const Screen*, QDomElement > screenToElementMapT;
    QDomDocument expDocument;
    QDomElement screensElement;

    screenToElementMapT screenToElementMap;
public:
    ~ScreenEditorExperimentData();
    ScreensData* screens;
    void storeScreenChanges( const Screen* s );
    void addScreen( Screen* s );
};

/**
    * This class is a minimal experiment file parser, created
    * specifically for the screeneditor application. This was done
    * because the Apex ExperimentConfigFileParser was too tightly
    * connected with various other parts of the Apex code, and there
    * was no quick way to decouple this.  The screeneditor
    * application was created as part of a summer job, and there was
    * no time available for further refactoring in Apex.
    *
    * This class knows just enough about experiment files to be able
    * to parse screens in the file, and allow to save the file again,
    * with modified screens.
    *
    * For every experiment file parsed, a \ref
    * ScreenEditorExperimentData instance is returned.
    */
class ScreenEditorExperimentFileParser
{
public:
    ScreenEditorExperimentFileParser();
    ~ScreenEditorExperimentFileParser();

    /**
    * Parse a file, and store the data about it in the
    * ScreenEditorExperimentData class.  Ownership of the returned
    * instance is transferred to the caller.
    */
    ScreenEditorExperimentData *parse(const QString &file);
    void save( const ScreenEditorExperimentData* d, const QString& file );
    static QString schemaLoc;
};

}
}
#endif
