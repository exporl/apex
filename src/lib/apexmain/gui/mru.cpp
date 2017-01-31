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

#include "mru.h"
#include <appcore/deleter.h>
#include <appcore/qt_inifile.h>
#include <appcore/qt_utils.h>
#include <algorithm>
#include <qstringlist.h>
#include <QMenu>
//#inc

namespace
{
  const QString sc_sFileKey( "recent files" );
  const QString sc_sFileVal( "file" );
  const QString sc_sDirKey( "recent dirs" );
  const QString sc_sDirVal( "dir" );
  const size_t  sc_nMaxItems = 10;
}

namespace apex
{
  MRU::MRU( QMenu* const ac_pMenuToMru ) :
    mc_pMenu( ac_pMenuToMru ),
    mc_pList( new mt_MRUvector() )
  {
  }

  MRU::~MRU()
  {
    mp_RemoveAllItems();
    delete mc_pList;
  }

  void MRU::mp_LoadFromFile( const QString& ac_sFile )
  {
    appcore::CIniFile file;
    f_CreateFileIfNotExist( ac_sFile, QIODevice::ReadWrite ); //make sure the file is there
    file.mp_SetFilename( ac_sFile );
    file.mp_LoadFromFile();
    if( file.mf_eGetStatus() == appcore::CIniFile::mc_eOK )
    {
      file.mp_AddKey( sc_sFileKey );  //make sure it exists
      file.mp_AddKey( sc_sDirKey );  //make sure it exists
      bool bFound = true;
      unsigned  n = 0;
      while( bFound && n < sc_nMaxItems - 1 )
      {
        const QString sItem = file.mf_sGetValueString( sc_sFileKey, sc_sFileVal + QString::number( n ) );
        if( !sItem.isEmpty() )
          mp_AddItem( sItem );
        else
          bFound = false;
        ++n;
      }
      const QString sItem = file.mf_sGetValueString( sc_sDirKey, sc_sDirVal );
      if( !sItem.isEmpty() )
        mp_SetOpenDir( sItem );
    }
  }

  void MRU::mp_SaveToFile( const QString& ac_sFile )
  {
    f_CreateFileIfNotExist( ac_sFile, QFile::ReadWrite );
    appcore::CIniFile file;
    file.mp_SetFilename( ac_sFile );
    file.mp_LoadFromFile();
    if( file.mf_eGetStatus() == appcore::CIniFile::mc_eOK )
    {
      file.mp_AddKey( sc_sFileKey );  //make sure it exists
      file.mp_AddKey( sc_sDirKey );  //make sure it exists

      const size_t size = mc_pList->size();
      for( unsigned n = 0 ; n < size ; ++n )
        file.mp_SetValueString( sc_sFileKey, sc_sFileVal + QString::number( n ), mc_pList->at( n )->text() );

      file.mp_SetValueString( sc_sDirKey, sc_sDirVal, mv_sOpenDir );
      file.mp_CommitToFile();
    }
  }

  void MRU::mp_SetOpenDir( const QString& ac_sFile )
  {
    mv_sOpenDir = ac_sFile;
  }

  const QString& MRU::mf_sGetOpenDir() const
  {
//    qDebug("MRU::mf_sGetOpenDir");
    return mv_sOpenDir;
  }

  void MRU::mp_AddItem( const QString& ac_sItem, const bool ac_bEnable )
  {
    const size_t size = mc_pList->size();
    if( !size )
      mc_pMenu->addSeparator();
    else
    {
        //check duplicate
      mt_MRUvector::const_iterator it = mc_pList->begin();
      while( it != mc_pList->end() )
      {
        if( (*it)->text() == ac_sItem )
          return;
        ++it;
      }
    }
    MRUAction* p = new MRUAction( mc_pMenu, ac_sItem );
    if( size + 1 >= sc_nMaxItems )
    {
      mt_MRUvector::iterator it = mc_pList->begin();
      mc_pMenu->removeAction (*it);
      mc_pList->erase( it );
    }
    mc_pList->push_back( p );
    mc_pMenu->addAction (p);
    p->setEnabled( ac_bEnable );
    connect( p, SIGNAL( Activated( const QString& ) ), this, SIGNAL( Released(const QString&) ) );
  }

  void MRU::mp_RemoveItem( const QString& /*ac_sItem*/ )
  {
    //unused..
  }

  void MRU::mp_RemoveAllItems()
  {
    disconnect();
    for_each( mc_pList->begin(), mc_pList->end(), appcore::Deleter() );
    mc_pList->clear();
  }

  void MRU::mf_Enable( const bool ac_bEnable )
  {
    mt_MRUvector::const_iterator it = mc_pList->begin();
    while( it != mc_pList->end() )
    {
      (*it)->setEnabled( ac_bEnable );
      ++it;
    }
  }

QStringList MRU::items() const
{
    QStringList result;

    mt_MRUvector::const_iterator it = mc_pList->begin();
    while (it != mc_pList->end())
        result << (*it++)->text();

    return result;
}

}
