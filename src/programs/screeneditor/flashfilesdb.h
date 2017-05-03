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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_FLASHFILESDB_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_FLASHFILESDB_H_

#include <vector>
#include <QString>

/**
 * This is a very simple class containing just a list of all Flash
 * Files that the screeneditor has already seen.  It is a singleton.
 */
struct FlashFilesDB
{
    std::vector<QString> files;

    static FlashFilesDB& instance();

private:
    FlashFilesDB();
};

#endif
