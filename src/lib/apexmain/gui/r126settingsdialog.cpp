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
 
#ifdef R126

#include "r126settingsdialog.h"
#include "map/r126database.h"
#include "services/paths.h"
using namespace r126;

#include <qradiobutton.h>
#include <qlineedit.h>
#include <qpixmap.h>

namespace apex{

  R126SettingsDialog::R126SettingsDialog( R126DataBase* const a_cpDataBase ) :
      R216SettingsBase( 0, 0, TRUE, 0 ),
    m_pDataBase( a_cpDataBase ),
    m_eVersion( a_cpDataBase->mf_pGetDataBaseSettings()->mf_eGetVersion() ),
    m_sServer( a_cpDataBase->mf_pGetDataBaseSettings()->mf_sGetServerName() )
  {
    Init();
  }

  R126SettingsDialog::~R126SettingsDialog()
  {
  }

  void R126SettingsDialog::Init()
  {
    R216SettingsBase::setIcon(QPixmap( Paths::Get().mf_sMainLogoPath()));
    if( !m_sServer.isEmpty() )
    {
      lineEditRemotePc->setText( m_sServer );
      radioButtonRemotePc->setChecked( true );
    }
    else
      radioButtonThisPc->setChecked( true );

    if( m_eVersion == r126::R126VERSION_1_3 )
      radioButtonSV0->setChecked( true );
    else if( m_eVersion == r126::R126VERSION_2_0 )
      radioButtonSV1->setChecked( true );
    else if( m_eVersion == r126::R126VERSION_2_1 )
      radioButtonSV2->setChecked( true );
  }

  void R126SettingsDialog::ThisComputerClicked()
  {
    radioButtonRemotePc->setChecked( false );
    m_sServer = "\\";
  }

  void R126SettingsDialog::RemoteComputerClicked()
  {
    radioButtonThisPc->setChecked( false );
    m_sServer = lineEditRemotePc->text();
  }

  void R126SettingsDialog::SV0Clicked()
  {
    m_eVersion = r126::R126VERSION_1_3;
    radioButtonSV1->setChecked( false );
    radioButtonSV2->setChecked( false );
  }

  void R126SettingsDialog::SV1Clicked()
  {
    m_eVersion = r126::R126VERSION_2_0;
    radioButtonSV0->setChecked( false );
    radioButtonSV2->setChecked( false );
  }

  void R126SettingsDialog::SV2Clicked()
  {
    m_eVersion = r126::R126VERSION_2_1;
    radioButtonSV0->setChecked( false );
    radioButtonSV1->setChecked( false );
  }

  void R126SettingsDialog::TestConnectionClicked()
  {
    if( radioButtonRemotePc->isChecked() )
      m_sServer = lineEditRemotePc->text();
    m_pDataBase->mf_SetDataBaseSettings( m_eVersion, m_sServer );
    m_pDataBase->mf_bTestConnection();
  }

}

#endif //#ifdef R126

