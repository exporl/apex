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
 
  /**
    * @file win32_mmemixerimpl.h
    * helpers for the IMatrixMixer implementation using win32 Mme
    */

#ifndef __WIN32_MMEMIXERIMPL_H__
#define __WIN32_MMEMIXERIMPL_H__

#include <string>
#include "containers/matrix.h"
#include "mixers/mixerelement.h"

namespace streamapp
{
  namespace mme
  {

    struct tMixer;            //mixer+info
    struct tMixerLine;        //mixerline+controls+source lines
    struct tMixerControl;     //single (volume) parameter entity
    struct tMixerMultiplexer; //

      /**
        * MmeVolumeControl
        *   represents a single volume control entity
        *   like a slider in the volume control panel
        *   an entity can contain multiple parameters however;
        *   currently only the first one (==the volume slider) is affected
        *   If ac_bInit == true, sets all appropriate elements to 0dB
        ****************************************************************** */
    class MmeVolumeControl : public IMixerElement
    {
    public:
        /**
          * Constructor.
          * @param a_Mixer the mixer
          * @param a_Line the mixer line
          * @param ac_bInit if true, initializes all gains to 0dB
          */
      MmeVolumeControl( tMixer& a_Mixer, tMixerLine& a_Line, const bool ac_bInit = true );

        /**
          * ~MmeVolumeControl
          */
      ~MmeVolumeControl();

        /**
          * Set a gain.
          * @param ac_dGainInDb the gain
          * @param ac_nChannel the channel to set the gain on
          */
      void mp_SetValue( const GainType ac_dGainInDb, const unsigned ac_nChannel );

        /**
          * Get a gain.
          * @param ac_nChannel channel to get gain for
          * @return the gain
          */
      GainType mf_dGetValue( const unsigned ac_nChannel ) const;

        /**
          * Get the number of channels.
          * @return the number
          */
      unsigned mf_nGetNumChannels() const;

        /**
          * Get the channel's name.
          * @param ac_nChannel the channel number
          * @return the name
          */
      const std::string& mf_sGetName( const unsigned ac_nChannel ) const;

    private:
      tMixer&           m_Mixer;
      tMixerControl*    m_pControl;
      const std::string mc_sName;

      MmeVolumeControl( const MmeVolumeControl& );
      MmeVolumeControl& operator = ( const MmeVolumeControl& );
    };

      /**
        * MmeOutputControls
        *   array of MmeVolumeControls for the output.
        *********************************************  */
    class MmeOutputControls : public IMixerElements
    {
    public:
        /**
          * Constructor.
          * @param ac_Controls the number of controls to allocate
          */
      MmeOutputControls( const unsigned ac_Controls );

        /**
          * Destructor.
          */
      ~MmeOutputControls();
    };

      /**
        * MmeOutputControls
        *   array of MmeVolumeControls for the input and a multiplexer.
        **************************************************************  */
    class MmeInputControls : public IMixerElements
    {
    public:
        /**
          * Constructor.
          * @param ac_Controls  the number of controls to allocate
          * @param a_pMultiPlexer the multiplexer for this control
          */
      MmeInputControls( const unsigned ac_Controls, tMixerMultiplexer* a_pMultiPlexer );

        /**
          * Destructor.
          */
      ~MmeInputControls();

        /**
          * Select one of the inputs to use.
          * @param ac_nInput the number of the input
          */
      void mp_SelectInputForMux( const unsigned ac_nInput );

        /**
          * Get the currently selected input.
          * @return the index
          */
      unsigned mf_nGetSelectedInput() const;

    private:
      tMixerMultiplexer* m_pMux;
    };

      /**
        * MmeMixerData
        *   maps the entire mixing structure for one mixer device:
        *   input, output and all their lines.
        ********************************************************** */
    class MmeMixerData
    {
    public:
        /**
          * Constructor.
          */
      MmeMixerData();

        /**
          * ~MmeMixerData
          */
      ~MmeMixerData();

        /**
          * Open a mixer device
          * @param ac_nMixerID the id
          * @param a_sError contains error description
          * @return false if there was an error, a_sError contains the error
          */
      bool mp_bOpen( const unsigned ac_nMixerID, std::string& a_sError );

        /**
          * Get the slave input controls.
          * @return the controls
          */
      MmeInputControls*   mf_pGetSlaveInputs() const;

        /**
          * Get the master volume control.
          * @return the control
          */
      MmeVolumeControl*   mf_pGetMasterOutput() const;

        /**
          * Get the slave output controls.
          * @return the controls
          */
      MmeOutputControls*  mf_pGetSlaveOutputs() const;

    private:
      bool mp_bGetLines( std::string& a_sError );
      bool mf_bGetLineControls( tMixerLine& a_Line, std::string& a_sError ) const;
      bool mf_bGetSourceLines( tMixerLine& a_Line, const unsigned ac_nDestinationLine, std::string& a_sError ) const;

      bool        m_bOpen;
      tMixer*     m_pMixer;
      tMixerLine* m_pMixerLines;

      MmeMixerData( const MmeMixerData& );
      MmeMixerData& operator = ( const MmeMixerData& );
    };

  }
}

#endif //#ifndef __WIN32_MMEMIXERIMPL_H__
