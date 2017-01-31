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

#include "apexmapwizard.h"
#include "r126settingsdialog.h"
#include "services/paths.h"

#include "map/r126database.h"
#include "map/r126dbasesetting.h"
#include "map/r126patientrecord.h"
#include "map/r126nucleusmaprecord.h"

#include <vector>
#include <QListBox>
#include <QLabel>

namespace apex{

  ApexMapWizard::ApexMapWizard()  :
      MapWizardBase( 0, 0, TRUE ), //modal
    m_pDataBase( new R126DataBase() )
  {
    MapWizardBase::setIcon( Paths::Get().mf_sMainLogoPath() );
      //FIXME get default values from a config file
    m_pDataBase->mf_SetDataBaseSettings( r126::R126VERSION_2_1, "TERPY" );
    setNextEnabled( WizardPage, false );
    setBackEnabled( WizardPage, false );
    setFinishEnabled( WizardPage, false );
    setNextEnabled( WizardPage_2, false );
    setBackEnabled( WizardPage_2, true );
    setFinishEnabled( WizardPage_2, false );
  }

  ApexMapWizard::~ApexMapWizard()
  {
    delete m_pDataBase;
  }

  void ApexMapWizard::FillPatientList()
  {
    listBoxPatient->clear();
    std::vector<R126PatientRecord*>& Rec = m_pDataBase->mf_FillPatientRecords();
    std::vector<R126PatientRecord*>::const_iterator it = Rec.begin();
    while( it != Rec.end() )
    {
      QString sName = (*it)->m_sSurName + ", " + (*it)->m_sFirstName;
      listBoxPatient->insertItem( sName );
      it++;
    }
  }

  void ApexMapWizard::FillMapList()
  {
    listBoxMap->clear();
    std::vector<R126NucleusMAPRecord*>& Rec = m_pDataBase->mf_FillNucleusMapsInfo( m_sSelectedPatient );
    std::vector<R126NucleusMAPRecord*>::const_iterator it = Rec.begin();
    unsigned i = 0;
    while( it != Rec.end() )
    {
      listBoxMap->insertItem( "Map " + QString::number( i+1 ) );
      it++;
      i++;
    }
  }

    //!fill patients list and start wizarding
  int ApexMapWizard::exec()
  {
    FillPatientList();
    return MapWizardBase::exec();
  }

    //back clicked in second page
  void ApexMapWizard::back()
  {
    //FillPatientList(); already there
    setFinishEnabled( WizardPage_2, false );
    MapWizardBase::back();
  }

    //next clicked in first page
  void ApexMapWizard::next()
  {
    FillMapList();
    MapWizardBase::next();
  }

  void ApexMapWizard::help()
  {
    QString sHelpText;
    if( MapWizardBase::currentPage() == WizardPage )
      sHelpText = "Select a Patient from the List or"
                  "Change Settings for the Database connection";
    else
      sHelpText = "Select a Map from the List";
    QMessageBox::information (NULL, "Help", sHelpText);
  }

  void ApexMapWizard::PatientSelected( QListBoxItem* )
  {
    std::vector<R126PatientRecord*>& Rec = m_pDataBase->mf_FillPatientRecords();
    R126PatientRecord* pSel = Rec.at( listBoxPatient->currentItem() );
    textLabelSex->setText( "Sex: " + pSel->m_sSex );
    textLabelName->setText( "Name: " + pSel->m_sSurName + ", " + pSel->m_sFirstName );
    textLabelDateBirth->setText( "Date of Birth: " + pSel->m_sBirth );
    textLabelDateImplant->setText( "Date of Implantation: " + pSel->m_sImplantDate );
    textLabelImplantType->setText( "Implant Type: " + pSel->m_sImplant );
    m_sSelectedPatient = pSel->m_sGUID;
    setNextEnabled( WizardPage, true );
  }

  void ApexMapWizard::MapSelected( QListBoxItem* )
  {
    std::vector<R126NucleusMAPRecord*>& Rec = m_pDataBase->mf_FillNucleusMapsInfo( m_sSelectedPatient );
    R126NucleusMAPRecord* pSel = Rec.at( listBoxMap->currentItem() );
    textLabelMaxima->setText( "Maxima: " + QString::number( pSel->m_nMaxima ) );
    textLabelRateTotal->setText( "Total Rate: " + QString::number( pSel->m_nTotalRate ) );
    textLabelNumberMode->setText( "Stimulation Mode: " + pSel->m_sStimulationMode );
    textLabelDateCreation->setText( "Creation Date: " + pSel->m_sMAPDate );
    textLabelNumberStrategy->setText( "Strategy: " + pSel->m_sStrategy );
    textLabelRateStimulation->setText( "Stimulation Rate: " + QString::number( pSel->m_nStimulationRate ) );
    m_sSelectedMap = pSel->m_sGUID;
    setFinishEnabled( WizardPage_2, true );
  }

  void ApexMapWizard::R126SettingsClicked()
  {
    R126SettingsDialog* dlg = new R126SettingsDialog( m_pDataBase );
    dlg->exec();
  }

  R126NucleusMAPRecord* ApexMapWizard::GetSelectedMap() const
  {
    return m_pDataBase->mf_pGetNucleusMap( m_sSelectedMap );
  }

}


#endif //#ifdef R126
