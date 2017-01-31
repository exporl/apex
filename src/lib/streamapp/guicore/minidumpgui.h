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
 
#ifndef __MINIDUMPGUI_H__
#define __MINIDUMPGUI_H__

#include "userinputbase.h"

class QLabel;
class QString;

namespace guicore
{

    /**
      * ShowMiniDump
      *   dialog for showing MiniDump
      *   TODO add mail functionality
      ******************************* */
  class ShowMiniDump : public UserInputBase
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param ac_sEx the exception info
        * @param ac_sDmp the minidump file path
        * @param ac_sMailTo adress to mail dump to
        */
    ShowMiniDump( const QString& ac_sEx, const QString& ac_sDmp, const QString& ac_sMailTo = QString::null );

      /**
        * Destructor.
        */
    ~ShowMiniDump();

  public slots:
      /**
        * Called to display the exception info.
        */
    void ms_ShowEx();

      /**
        * Called to mail the dump
        */
    void ms_MailDmp();

  private:
    const QString Ex;
    const QString Dmp;
    const QString Mail;
    QLabel*       Label;
    QLabel*       Label2;
    QPushButton*  SeeEx;
    QPushButton*  MailDmp;

    ShowMiniDump( const ShowMiniDump& );
    ShowMiniDump& operator = ( const ShowMiniDump& );
  };

}

#endif //#ifndef __MINIDUMPGUI_H__
