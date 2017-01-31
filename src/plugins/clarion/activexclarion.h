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
 
#ifndef __APEX_ACTIVEXCLARION_H__
#define __APEX_ACTIVEXCLARION_H__


#include "clarionwrapper.h"

namespace appcore
{
  class WaitableObject;
}
using namespace appcore;

namespace clarion
{
  struct ActiveXStuff;
  class WaitConnection;

    /**
      * ActiveXClarionWrapper
      *   IClarionWrapper implementation using ActiveX.
      ************************************************* */
  class ActiveXClarionWrapper : public IClarionWrapper
  {
  public:
      /**
        * Constructor.
        * Initializes the class' data members.
        */
    ActiveXClarionWrapper();

      /**
        * Destructor.
        */
    ~ActiveXClarionWrapper();

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mp_bConnect();

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mp_bDisConnect();

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mp_bLoadFile( const QString& ac_pFileName );

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mp_bStart();

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mp_bStop();

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mf_bIsPlaying() const;

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mp_bWaitForStop();

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mf_bHasParameter( const QString& ac_pParName, int* const ac_pIndex = 0 ) const;

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mf_bCheckParameters( const QStringList& ac_saParams ) const;

      /**
        * Implementation of the IClarionWrapper method.
        */
    bool mp_bSetParameter( const QString& ac_pPar, const QString& ac_pVal );

  private:
    WaitConnection* m_pWaitThread;
    WaitableObject* m_pBEDCSConnectedEvent;
    WaitableObject* m_pEndOfStimulusEvent;
    ActiveXStuff*   m_pStuff;
  };


    /**
      * ClarionWrapperCreator
      *********************** */
  class ClarionWrapperCreator : public QObject, public ClarionWrapperCreatorInterface
  {
    Q_OBJECT
    Q_INTERFACES( clarion::ClarionWrapperCreatorInterface )
  public:
    virtual ~ClarionWrapperCreator();

    virtual QStringList availablePlugins() const;

    virtual IClarionWrapper *create () const ;
  };

}

#endif //#ifndef __APEX_ACTIVEXCLARION_H__
