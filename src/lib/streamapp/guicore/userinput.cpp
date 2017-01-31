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
 
#include "userinput.h"
#include "customlayout.h"
#include "widgetattributes.h"

#include <qtextedit.h>
#include <qdialog.h>
#include <qlabel.h>

using namespace guicore;

UserInput::UserInput( const QString& ac_sTitle, const QString& ac_sText ) :
  UserInputBase( false )
{
#if QT_VERSION < 0x040000
  UserInputBase::setCaption( ac_sTitle );
#else
  UserInputBase::setWindowTitle( ac_sTitle );
#endif

  m_pLabel = new QLabel( ac_sText, this );
  widgetattributes::gf_ApplyDefaultAlignment( m_pLabel );
  layout->mp_AddItem( m_pLabel, new tItemLayout( 0.0, 0.0, 1.0, 0.7 ) );
}

UserInput::~UserInput()
{
}
