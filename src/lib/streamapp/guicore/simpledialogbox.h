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
 
#ifndef __SIMPLEDIALOGBOX_H__
#define __SIMPLEDIALOGBOX_H__

#include <qglobal.h>
#if QT_VERSION < 0x040000
#include <qmessagebox.h>
#else
#include <QMessageBox>
#endif

class QPixmap;

namespace guicore
{

    /**
      * SimpleDialogBox
      *   implements a customizable dialog box with static functions.
      *   User can specify title, dialog icon, dialog type and the message.
      ********************************************************************* */
  class SimpleDialogBox
  {
  public:
      /**
        * Constructor.
        * Passing an empty label for a button will hide the button,
        * except for the first one.
        * @param eIcon the icon to use
        * @param ac_sCaption the title
        * @param ac_sErrorMessage the message
        * @param ac_sButton1 the label on button 1
        * @param ac_sButton2 the label on button 2
        * @param ac_sButton3 the label on button 3
        */
    SimpleDialogBox(  const QMessageBox::Icon eIcon,
                      const QString& ac_sCaption,
                      const QString& ac_sErrorMessage,
                      const QString& ac_sButton1 = "Ok",
                      const QString& ac_sButton2 = "",
                      const QString& ac_sButton3 = ""  );

      /**
        * Destructor.
        */
    ~SimpleDialogBox();

      /**
        * Show the dialog and wait for an answer,
        * @return the answer
        */
    const int exec();

      /**
        * Define the dialog types.
        */
    enum mt_eType
    {
      mc_eError,    //!< with error icon
      mc_eWarning,  //!< with warning icon
      mc_eMessage   //!< with message icon
    };

      /**
        * Display a dialog box.
        * @param ac_sMessage the message
        * @param ac_eType the type
        */
    static void sf_MessageBox( const QString& ac_sMessage, const mt_eType ac_eType = mc_eError );

      /**
        * Display a dialog box with title.
        * @param ac_sCaption the title
        * @param ac_sMessage the message
        * @param ac_eType the type
        */
    static void sf_MessageBox( const QString& ac_sCaption, const QString& ac_sMessage, const mt_eType ac_eType = mc_eError );

      /**
        * Display a dialog box with icon and title.
        * @param ac_sIcon path of the icon file to use
        * @param ac_sCaption the title
        * @param ac_sMessage the message
        * @param ac_eType the type
        */
    static void sf_MessageBox( const QString& ac_sIcon, const QString& ac_sCaption, const QString& ac_sMessage, const mt_eType ac_eType = mc_eError );

      /**
        * Display a dialog box.with icon and title.
        * @param ac_Icon the icon to use
        * @param ac_sCaption the title
        * @param ac_sMessage the message
        * @param ac_eType the type
        */
    static void sf_MessageBox( const QPixmap& ac_Icon, const QString& ac_sCaption, const QString& ac_sMessage, const mt_eType ac_eType = mc_eError );

  private:
     QMessageBox* m_pMb;
  };

}

#endif //#ifndef __SIMPLEDIALOGBOX_H__
