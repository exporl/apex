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
 
#ifndef __APEXFILEDIALOG_H__
#define __APEXFILEDIALOG_H__

#include "servicemanager.h"

#include <QFileDialog>

namespace apex
{

    /**
      * FileDialog
      *   QFileDialog for Apex.
      *   Qt tries to use the Os' filedialog if possible,
      *   which might have a different style; we want every
      *   widget to look the same so use our own dialog
      *   instead of using QFileDialog's static functions.
      *   NOTE for some reason the Qt's dialog is slower..
      *   [ stijn ] changed to nativ on windows, this will
      *   still look ok since everything else in apex also
      *   uses the standard windows colors.
      ***************************************************** */
#ifdef WIN32
  class FileDialog : public Service<FileDialog>
#else
  class FileDialog : private QFileDialog, public Service<FileDialog>
#endif
  {
  public:
      /**
        * Constructor.
        */
    FileDialog();

      /**
        * Destructor.
        */
    ~FileDialog();
    
      /**
        * @see Service
        */
    const char* Name()
    { return "Filedialog"; }
    
      /**
        * Get an existing file.
        * @return the file, or an empty string if canceled
        */
    QString mf_sGetExistingFile( const QString& ac_sPath, const QStringList& ac_sFilters );
    
    QString mf_sGetExistingFile( const QString& ac_sPath, const QString& ac_sFilter );

      /**
        * Get new or existing file.
        * @return the file, or an empty string if canceled
        */
    QString mf_sGetAnyFile( const QString& ac_sPath, const QString& ac_sFilter );
  };

}

#endif //#ifndef __APEXFILEDIALOG_H__
