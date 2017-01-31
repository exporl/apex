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

#ifndef __CLARIONWRAPPER_H__
#define __CLARIONWRAPPER_H__

#include <QString>
#include <QtPlugin>

namespace clarion
{

    /**
      * IClarionWrapper
      *   interface for the Clarion device.
      *   We use an interface so we can switch between
      *   managed or unmanaged implementations later.
      ************************************************ */
  class IClarionWrapper
  {
  public:
      /**
        * Destructor.
        */
    virtual ~IClarionWrapper()
    {}

      /**
        * The device status.
        */
    enum mt_eStatus
    {
      mc_eInit,       //!< not yet connected
      mc_eConnected,  //!< connected
      mc_eLoaded,     //!< connected and file loaded
      mc_eRunning,    //!< playing
      mc_eError       //!< fatal error
    };

      /**
        * Query the current status.
        * @return the status
        */
    virtual const mt_eStatus& mf_eGetStatus() const
    {
      return mv_eStatus;
    }

      /**
        * Connect to the device.
        * Only succeeds if status == mc_eInit.
        * @return false if an error occured
        * @see mf_cGetLastError()
        */
    virtual bool mp_bConnect() = 0;

      /**
        * Disconnect the device.
        * Only succeeds if status == mc_eConnected.
        * @return false if an error occured or if not connected
        */
    virtual bool mp_bDisConnect() = 0;

      /**
        * Load a stimulus file.
        * Only succeeds if status == mc_eConnected.
        * @param ac_sFileName the file's path
        * @return false if an error occured
        */
    virtual bool mp_bLoadFile( const QString& ac_sFileName ) = 0;

      /**
        * Start playing the currently loaded file.
        * Only succeeds if status == mc_eLoaded.
        * @return false if an error occured, or no file is loaded
        */
    virtual bool mp_bStart() = 0;

      /**
        * Stop playing immedeately.
        * Only has effect if playing started.
        * @return false if an error occured
        */
    virtual bool mp_bStop() = 0;

      /**
        * Check if the device is currently playing.
        * @return true if playing
        */
    virtual bool mf_bIsPlaying() const = 0;

      /**
        * This is a blocking call that returns when receiving a stop event
        * from the device.
        * Only succeeds if status == mc_eRunning
        * @return false if device is not currently playing
        */
    virtual bool mp_bWaitForStop() = 0;

      /**
        * Check if the device knows a certain parameter.
        * Can only be called if status == mc_eLoaded.
        * @param ac_sParName the name
        * @param ac_pIndex if non-zero, pointer to an int that will be assigned
        *   the parameter index in the Clarion's ControlVars array.
        * @return false if it's not a valid parameter
        */
    virtual bool mf_bHasParameter( const QString& ac_sParName, int* const ac_pIndex = 0 ) const = 0;

      /**
        * Check if all parameters are present.
        * Can only be called if status == mc_eLoaded.
        * Eg if 5 parameters are defined in the Clarion .bExp file,
        * checks if all 5 are also defined in the apex .xml experiment file.
        * @param ac_saParNames array of parameters from xml
        * @return true if ok, else see mf_cGetLastError() for the missing parameter
        */
    virtual bool mf_bCheckParameters( const QStringList& ac_saParNames ) const = 0;

      /**
        * Set a parameter on the device.
        * Can only be called if status == mc_eLoaded, and not while playing.
        * Assumes the parameter has been checked by mf_bHasParameter() already.
        * @param ac_sPar the parameter name
        * @param ac_sVal the new value
        * @return false if there's an error
        */
    virtual bool mp_bSetParameter( const QString& ac_sPar, const QString& ac_sVal ) = 0;

      /**
        * Query error message.
        * @return char pointer
        */
    virtual const char* mf_cGetLastError() const
    {
      return m_cError;
    }

    protected:
        /**
          * Protected Constructor.
          */
      IClarionWrapper() :
        mv_eStatus( mc_eInit )
      {}

      mutable char        m_cError[ 128 ];
      mutable mt_eStatus  mv_eStatus;
  };


  class ClarionWrapperCreatorInterface
  {
  public:
    virtual ~ClarionWrapperCreatorInterface()
    {
    }

    virtual QStringList availablePlugins() const = 0;
    virtual IClarionWrapper *create () const = 0;
  };

}

Q_DECLARE_INTERFACE( clarion::ClarionWrapperCreatorInterface, "be.exporl.apex.clarionwrapper/1.0" )

#endif //#ifndef __CLARIONWRAPPER_H__
