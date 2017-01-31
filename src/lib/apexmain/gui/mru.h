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

#ifndef MRUH_H
#define MRUH_H

#include <qobject.h>
#include <qaction.h>
#include <vector>

class QString;
class QMenu;

namespace apex
{
    /**
      * Derived QAction that emits Activated with it's text
      ***************************************************** */
  class MRUAction : public QAction
  {
      Q_OBJECT
  public:
    MRUAction( QObject* a_pParent, const QString& ac_sText ) :
     QAction( a_pParent )
    {
      QAction::setText( ac_sText );
      connect( this, SIGNAL(triggered() ), this, SLOT( Activated() ) );
    }
    virtual ~MRUAction()
    {}

  signals:
    void Activated( const QString& );

  private slots:
    virtual void Activated()
    { emit Activated( QAction::text() ); }
  };

    /**
      * Class with simple MRU functionality
      *   inserts seperator and strings added to menu
      *   emits text of item clicked
      *********************************************** */
  class MRU : public QObject
  {
    Q_OBJECT
  public:
    MRU( QMenu* const ac_pMenuToMru );
    ~MRU();

      //load/save the list from/to an ini file
    void mp_LoadFromFile( const QString& ac_sFile );
    void mp_SaveToFile( const QString& ac_sFile );

      //get/set the open dir (doesn't affect the menu but is handy to store in file)
    void mp_SetOpenDir( const QString& ac_sFile );
    const QString& mf_sGetOpenDir() const;

    void mp_AddItem( const QString& ac_sItem, const bool ac_bEnable = true );
    void mp_RemoveItem( const QString& ac_sItem );

      //!will also remove seperator
    void mp_RemoveAllItems();

    void mf_Enable( const bool ac_bEnable );

    QStringList items() const;

    typedef std::vector<MRUAction*> mt_MRUvector;

  signals:
    void Released( const QString& ac_sFileName );

  private:
    QMenu* const   mc_pMenu;
    mt_MRUvector* const mc_pList;
    QString             mv_sOpenDir;
  };

}

#endif //#ifndef MRUH_H
