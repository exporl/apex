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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_R126SETTINGSDIALOG_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_R126SETTINGSDIALOG_H_

#include "r216settingsbase.h"
#include "../map/r126dbasesetting.h"
#include <qstring.h>

namespace r126{
  class R126DataBase;
}
using namespace r126;

namespace apex{

  //!Wizard allowing user to select a map from a R126 database
  //!has only two pages which are defined through designer
  //!for simplicity, ApexMapWizard handles everything happening on any page
  class R126SettingsDialog : public R216SettingsBase
  {
  public:
    R126SettingsDialog( R126DataBase* const a_cpDataBase );
    ~R126SettingsDialog();

    void Init();

      //base overrides
  protected slots:
    virtual void ThisComputerClicked();
    virtual void RemoteComputerClicked();
    virtual void SV0Clicked();
    virtual void SV1Clicked();
    virtual void SV2Clicked();
    virtual void TestConnectionClicked();

  private:
    R126DataBase* const m_pDataBase;
    eR126VersionNumber  m_eVersion;
    QString             m_sServer;
  };

}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_R126SETTINGSDIALOG_H_
