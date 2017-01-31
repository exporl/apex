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
 
#ifndef __MSGWINDOW_H__
#define __MSGWINDOW_H__

#include "typedefs.h"
#include "customlayoutwidget.h"

class QTextEdit;
class QPushButton;

namespace guicore
{
    /**
      * MessageWindow
      *   simple window for displaying messages.
      *   Shows a list of messages, supporting color codes.
      *   @note do not use if you have lots of subsequent
      *   messages, this tends to hang the entire application
      *   for some reason, especially with qt4.
      *   @note discovered very strange behaviour with qt4:
      *   if the window is destroyed after QApplication is
      *   destroyed, QTextEdit corrupts the heap somehow!
      *   This might be normal however, all QObjects should
      *   probably be destroyed before QApplication.
      ***************************************************** */
  class MessageWindow : public CustomLayoutWidget<>
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget or 0 for desktop level
        */
    MessageWindow( QWidget* a_pParent );

      /**
        * Destructor.
        */
    ~MessageWindow();

      /**
        * Timestamp all messages.
        * @param ac_bSet true for timestamp
        */
    void mp_SetTimeStamp( const bool ac_bSet = false )
    { mv_bTimeStamp = ac_bSet; }

      /**
        * Allow closing window.
        * @param ac_bSet true for allow
        */
    void mp_AllowClose( const bool ac_bSet = false )
    { mv_bAllowClose = ac_bSet; }

      /**
        * Get allow.
        * @return true if widget can be closed
        */
    const bool& mf_bAllowedToClose() const
    { return mv_bAllowClose; }

      /**
        * Set the font.
        * @param ac_Font the font
        */
    void mp_SetFont( const QFont& ac_Font );

      /**
        * Get the font.
        * @return the current font
        */
    QFont mf_GetFont() const;

      /**
        * Save to file when closed.
        */
    void mp_AutoSaveToFile( const QString& ac_sFile )
    { m_sAutoSave = ac_sFile; }

      /**
        * Invert the TextEdit's color scheme
        * @param ac_bSet true for black background
        */
    void mp_SetNegativeColor( const bool ac_bSet );

  public slots:
      /**
        * Add a message.
        * @param ac_sMess the message string
        */
    void ms_Message( const QString& ac_sMess );

      /**
        * Add a message.
        * @param ac_sMess the message string
        * @param ac_eType the message type, used for coloring output
        */
    void ms_Message( const QString& ac_sMess, const streamapp::gt_eMsgType& ac_eType );

      /**
        * Clear all messages.
        */
    void ms_Clear();

      /**
        * Copy to Clipboard.
        */
    void ms_Copy();

      /**
        * Save to file.
        */
    void ms_Save();

  protected:
      /**
        * Override closing
        */
    void closeEvent( QCloseEvent* );

      /**
        * Save to file.
        * @param ac_sFile the file path.
        */
    void mp_Save( const QString& ac_sFile );

  private:
    QTextEdit*    m_pMess;
    QPushButton*  m_pCopy;
    QPushButton*  m_pSave;
    QString       m_sAutoSave;
    bool          mv_bTimeStamp;
    bool          mv_bAllowClose;
    bool          mv_bInvertColor;
  };

}

#endif //#ifndef __MSGWINDOW_H__
