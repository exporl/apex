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
 
#ifndef QLABELEDEDIT
#define QLABELEDEDIT

#include "customlayout.h"

#if QT_VERSION < 0x040000
#include <qfont.h>
#include <qlabel.h>
#include <qlineedit.h>
#else
#include <QFont>
#include <QLabel>
#include <QLineEdit>
#endif

class QLabel;

namespace guicore
{

    /**
      * LineEdit
      *   defines the line edit class.
      *   Used to make sure all edits used look the same.
      *************************************************** */
  class LineEdit : public QLineEdit
  {
  public:
      /**
        * Constructor
        * @param a_pParent the parent widget or 0 for desktop level
        * @param ac_sName the object name
        * @param ac_bByPass if true, doesn't set the font size.
        */
    LineEdit( QWidget* a_pParent, const QString& ac_sName = QString::null, const bool ac_bByPass = true ) :
#if QT_VERSION < 0x040000
      QLineEdit( a_pParent, ac_sName.data() )
#else
      QLineEdit( a_pParent )
#endif
    {
      if( !ac_bByPass )
      {
        QFont _font( QLineEdit::font() );
        _font.setPointSize( 10 );
        QLineEdit::setFont( _font );
      }
#if QT_VERSION > 0x040000
      QLineEdit::setText( ac_sName );
#endif
    }
  };

    /**
      * LabeldEdit
      *   a QLineEdit with a QLabel attached to it.
      *   Deprecated, use LabeldItem<LineEdit>.
      ******************************************** */
  class LabeldEdit : public LineEdit
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget or 0 for desktop level
        * @param ac_sText the label text
        * @param ac_eWhere the label position
        * @param ac_dLabelSize the label size
        */
    LabeldEdit( QWidget* a_pParent, const QString& ac_sText,
                const guicore::FriendLayout::mt_eFriendPosition ac_eWhere = guicore::FriendLayout::left,
                const double ac_dLabelSize = 0.5 );

      /**
        * Destructor.
        */
    ~LabeldEdit();

      /**
        * Set the geometry.
        * Overriden to use FriendLayout's scaling.
        * @param ac_Rect the geometry rectangle
        */
    void setGeometry( const QRect& ac_Rect );

      /**
        * Set the label alignment.
        * @param nFlag the alignment flag (see Qt)
        */
#if QT_VERSION < 0x040000
    void mp_SetLabelAlignment( int nFlag );
#else
    void mp_SetLabelAlignment( Qt::Alignment nFlag );
#endif

  protected:
    void resizeEvent( QResizeEvent* e );
    void DoResize();

  private:
    QLabel* const m_pLabel;
    guicore::FriendLayout* mc_pLayout;

    LabeldEdit& LabelEdit( const LabeldEdit& );
    LabeldEdit& operator = ( const LabeldEdit& );
  };

  /**
      * LabeldItem
      *   template class for items with a label.
      *   Keeps it simple by having the label public.
      *********************************************** */
  template<class tWidget>
  class LabeldItem : public tWidget
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parant widget or 0 for desktoplevel
        * @param ac_sText the label's text
        * @param ac_eWhere the label's position relative to tWidget
        * @param ac_dLabelSize the label's size ratio
        */
    LabeldItem( QWidget* a_pParent, const QString ac_sText, const guicore::FriendLayout::mt_eFriendPosition ac_eWhere = guicore::FriendLayout::left, const double ac_dLabelSize = 0.5 ) :
        tWidget( a_pParent ),
      m_pLabel( new QLabel( ac_sText, a_pParent ) ),
      mc_pLayout( new FriendLayout( (QWidget*) this, m_pLabel, ac_eWhere, ac_dLabelSize ) )
    {
      switch( ac_eWhere )
      {
        #if QT_VERSION < 0x040000
        case FriendLayout::right: m_pLabel->setAlignment( QLabel::AlignRight ); break;
        case FriendLayout::top: m_pLabel->setAlignment( QLabel::AlignCenter ); break;
        case FriendLayout::bottom: m_pLabel->setAlignment( QLabel::AlignCenter ); break;
        #else
        case FriendLayout::right: m_pLabel->setAlignment( Qt::AlignRight ); break;
        case FriendLayout::top: m_pLabel->setAlignment( Qt::AlignCenter ); break;
        case FriendLayout::bottom: m_pLabel->setAlignment( Qt::AlignCenter ); break;
        #endif
        default : break;
      }
    }

      /**
        * Destructor.
        */
    ~LabeldItem()
    {
      delete mc_pLayout;
    }

      /**
        * Set the geometry.
        * Override to use CustomLayout's resizing.
        * @param ac_Rect the new geometry
        */
    void setGeometry( const QRect& ac_Rect )
    {
      static bool s_bRecurseMe = true;
      if( s_bRecurseMe )
      {
        s_bRecurseMe = false;
        mc_pLayout->mp_ResizeThemAll( ac_Rect );
        s_bRecurseMe = true;
      }
      else
        tWidget::setGeometry( ac_Rect );
    }

      /**
        * The Label.
        */
    QLabel* const m_pLabel;

  protected:
      /**
        * Override and ignore resizeevents.
        */
    void resizeEvent( QResizeEvent* ){}

  private:
    guicore::FriendLayout* mc_pLayout;

    LabeldItem( const LabeldItem& );
    LabeldItem& operator = ( const LabeldItem& );
  };

}

#endif //QLABELEDEDIT
