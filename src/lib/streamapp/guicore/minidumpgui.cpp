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
 
#include "minidumpgui.h"
#include "msgwindow.h"
#include "customlayout.h"
#include "widgetattributes.h"
#include "customlayoutwidget.h"

#if QT_VERSION < 0x040000
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapplication.h>
#else
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#endif

using namespace guicore;

namespace
{
  class ShowEx : public CustomLayoutWidget<QDialog>
  {
  public:
    ShowEx( const QString& ac_sEx ) :
        CustomLayoutWidget<QDialog>( 0 ),
      m_Wnd( this )
    {
#if QT_VERSION < 0x040000
      setCaption( "MiniDump - Exception Information" );
#else
      setWindowTitle( "MiniDump - Exception Information" );
#endif
      QFont fontr( m_Wnd.mf_GetFont() );
      fontr.setPointSize( 7 );
      m_Wnd.mp_SetFont( fontr );
      m_Wnd.mp_SetTimeStamp( false );
      m_Wnd.ms_Message( ac_sEx );
      m_pLayout = new CustomLayout();
      m_pLayout->mp_AddItem( &m_Wnd, new tItemLayout( 0.0, 0.0, 1.0, 1.0 ) );
      resize( 500, 350 );
    }
    ~ShowEx()
    {
    }

    MessageWindow m_Wnd;
  };
}

ShowMiniDump::ShowMiniDump( const QString& ac_sEx, const QString& ac_sDmp, const QString& ac_sMailTo ) :
  Ex( ac_sEx ),
  Dmp( ac_sDmp ),
  Mail( ac_sMailTo )
{
#if QT_VERSION < 0x040000
  setCaption( "MiniDump - Unhandled Exception" );
#else
  setWindowTitle( "MiniDump - Unhandled Exception" );
#endif

  Label = new QLabel( "A fatal error occured and the application has been stopped.", this );

  SeeEx = new QPushButton( this );
#if QT_VERSION < 0x040000
  SeeEx->setPaletteForegroundColor( QColor( 10, 10, 200 ) );
#else
  // currently no alternative, not essential
#endif
  SeeEx->setText( "Click here to see the excpetion info." );
  SeeEx->setFlat( true );
  SeeEx->show();
  connect( SeeEx, SIGNAL( released() ), SLOT( ms_ShowEx() ) );

  Label2 = new QLabel( "A minidump was created in " + ac_sDmp + "\nPlease send this to the author of this program.", this );

    //layout
  widgetattributes::gf_ApplyDefaultAlignment( Label );
  widgetattributes::gf_ApplyDefaultAlignment( Label2 );
  mp_SetNoCancel( true );
  layout->mp_AddItem( Label, new tItemLayout( 0.0, 0.0, 1.0, 0.1 ) );
  layout->mp_AddItem( SeeEx, new tItemLayout( 0.0, 0.2, 1.0, 0.15 ) );
  layout->mp_AddItem( Label2, new tItemLayout( 0.0, 0.4, 1.0, 0.3 ) );
}

ShowMiniDump::~ShowMiniDump()
{
}

void ShowMiniDump::ms_ShowEx()
{
  ShowEx ex( Ex );
  ex.exec();
}

void ShowMiniDump::ms_MailDmp()
{

}
