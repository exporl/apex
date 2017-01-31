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
 
#ifndef __FILESELECTOR_H__
#define __FILESELECTOR_H__

#include "labeledit.h"

class QPushButton;

namespace guicore
{

  class FriendLayout;

    /**
      * FileSelector
      *   LineEdit with file selector button.
      *   @see LineEdit
      ************************************** */
  class FileSelector : public LineEdit
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param a_pParent the paranet widget or 0 for desktop level
        * @param ac_eWhere the position of the selector button relative to the LineEdit
        */
    FileSelector( QWidget* a_pParent, const guicore::FriendLayout::mt_eFriendPosition ac_eWhere = guicore::FriendLayout::right );

      /**
        * Destructor.
        */
    ~FileSelector();

      /**
        * Selection mode.
        */
    enum mt_eMode
    {
      mc_eExistingFile,
      mc_eNewFile,
      mc_eExistingDir
      //mt_eNewDir   not supported by qt?
    };

      /**
        * Set the mode.
        * @param ac_eMode one of mt_eMode
        */
    INLINE void mp_SetMode( const mt_eMode& ac_eMode )
    {
      mv_eMode = ac_eMode;
    }

      /**
        * Get the current selection mode.
        * @return the mode
        */
    INLINE const mt_eMode& mf_eMode() const
    {
      return mv_eMode;
    }

      /**
        * Set the file type filter.
        * Only effects file modes.
        * @param ac_sSelect the selection string, eg "Text File (*.txt)"
        */
    INLINE void mp_SetFileFilter( const QString& ac_sSelect )
    {
      mv_sSelect = ac_sSelect;
    }

      /**
        * Get the current file filter.
        * @return text string reference
        */
    INLINE const QString& mf_sFileFilter() const
    {
      return mv_sSelect;
    }

      /**
        * Set the title of the dialog.
        * @param ac_sTitle the title string
        */
    INLINE void mp_SetDialogTitle( const QString& ac_sTitle )
    {
      mv_sTitle = ac_sTitle;
    }

      /**
        * Get the current dialog title.
        * @return text string reference
        */
    INLINE const QString& mf_sDialogTitle() const
    {
      return mv_sTitle;
    }

      /**
        * Set the directory where the dialog should start.
        * @param ac_sStartDir the path string
        */
    INLINE void mp_SetStartDir( const QString& ac_sStartDir )
    {
      mv_sStartDir = ac_sStartDir;
    }

      /**
        * Get the current start directory.
        * @return text string reference
        */
    INLINE const QString& mf_sStartDir() const
    {
      return mv_sStartDir;
    }

      /**
        * Get the currently selected item.
        * Can be a directory or a file.
        * Equals the content of the LineEdit.
        * @return text string reference
        */
    INLINE const QString& mf_sSelected() const
    {
      return mv_sSelected;
    }

      /**
        * Set geometry.
        */
    void setGeometry( const QRect &ac_Rect );

      /**
        * Get the button.
        * Can be used eg to set focus,
        * or a custom text
        * @return pointer to the button
        */
    INLINE QPushButton* mf_pGetButton() const
    {
      return mc_pButton;
    }

  signals:
      /**
        * Emitted when an item is selected through the button.
        */
    void ms_Selected( const QString& );

  protected:
      /**
        * Override resizing.
        */
    void resizeEvent( QResizeEvent* );

  private slots:
      /**
        * Called after selecting through button.
        */
    void mp_Select();

  private:
    QPushButton* const  mc_pButton;
    FriendLayout*       mc_pLayout;
    QString             mv_sStartDir;
    QString             mv_sSelected;
    QString             mv_sSelect;
    QString             mv_sTitle;
    mt_eMode            mv_eMode;

    FileSelector( const FileSelector& );
    FileSelector& operator = ( const FileSelector& );
  };

}

#endif //#ifndef __FILESELECTOR_H__
