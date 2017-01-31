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

#ifndef __STRDXMIXER_H__
#define __STRDXMIXER_H__

#include "../../mixers/mixer.h"
#include "../../mixers/mixerelement.h"

#include "../../typedefs.h"

namespace streamapp
{
    /**
      * namespace with win32 MME specific stuff
      ***************************************** */
  namespace mme
  {

    class MmeMixerData;
    class MmeVolumeControl;
    class MmeInputControls;
    class MmeOutputControls;

      /**
        * MmeMixer
        *   the windows mixer interface is quite complicated, but complete.
        *   The master output is the 'Volume Control' from the control panel,
        *   the master input is the input currently selected for multiplexing,
        *   and there can only be one.
        *   The matrix implementation does not support true routing: channel 0 of an input
        *   always is connected to channel 0 of the output, but all gains can be set
        *   individually.
        *   Another limitation is that a mono source is always routed to all outputs.
        *   Example scheme of matrix ( x is an available node ):
            @verbatim
                                                  Output channel no. (output == masteroutput
                                Input channel no.    0    1           == single hardware output)
            hardwareinputs:  0        0              x
            (only one can             1                   x
            be active)       1        2              x    x   ->cannot be set individually!
                             3        3              x
                                      4                   x
            softwareinputs:  0        5              x
                                      6                   x
                             1        7              x
                                      8                   x
                             2        9              x
                                      10                  x
            @endverbatim
        *
        * @see MmeMixerData
        *
        * TODO  check if scale is really exact in dB
        *       check if there is a way to find out which software input is the one of interest
        *       make this mixer interface/impl based on MixerElement arrays so an implementation
        *         only has to supply the arrays with soft/hard inputs and can reuse this class
        ***************************************************************************************** */
    class MmeMixer : public IMixer
    {
    public:
        /**
          * Constructor.
          * Initializes the mixer.
          * @param ac_nMixerNum the mixer index
          */
      MmeMixer( const unsigned ac_nMixerNum = 0 );

        /**
          * Constructor.
          * @param ac_sMixerName the mixer name
          */
      MmeMixer( const std::string& ac_sMixerName );

        /**
          * Destructor.
          */
      ~MmeMixer();

        /**
          * Get the number of available mixers.
          * @return the number
          */
      static unsigned       sf_nGetNumMixers();

        /**
          * Get the mixer index for the specified name
          * @param ac_sName the name
          * @return the index
          */
      static unsigned       sf_nMixerNumFromName( const std::string& ac_sName );

        /**
          * Get names of all available mixers.
          * @return vector of names
          */
      static tStringVector  sf_saGetMixerNames();

        /**
          * Implementation of the IMixer method.
          */
      mt_Range mf_tGetRange() const;

        /**
          * Mutes all inputs, sets all outputs to 0dB
          */
      void mp_Initialize();

        /**
          * Implementation of the IMixer method.
          */
      void mp_SetMasterInputGain( const GainType ac_dGainIndB );

        /**
          * Implementation of the IMixer method.
          */
      void mp_SetMasterInputGain( const GainType ac_dGainIndB, const unsigned ac_nChannel );

        /**
          * Implementation of the IMixer method.
          */
      void mp_SetMasterOutputGain( const GainType ac_dGainIndB );

        /**
          * Implementation of the IMixer method.
          */
      void mp_SetMasterOutputGain( const GainType ac_dGainIndB, const unsigned ac_nChannel );

        /**
          * Implementation of the IMixer method.
          */
      GainType mf_dGetMasterInputGain( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMixer method.
          */
      GainType mf_dGetMasterOutputGain( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMixer method.
          */
      unsigned mf_nGetNumMasterInputs() const;

        /**
          * Implementation of the IMixer method.
          */
      unsigned mf_nGetNumMasterOutputs() const;

        /**
          * Implementation of the IMixer method.
          */
      const std::string& mf_sGetMasterInputName( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMixer method.
          */
      const std::string& mf_sGetMasterOutputName( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      virtual unsigned mf_nGetNumHardwareInputs() const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      virtual unsigned mf_nGetNumSoftwareInputs() const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      virtual unsigned mf_nGetNumHardwareOutputs() const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      void mp_SetGain( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel );

        /**
          * Implementation of the IMatrixMixer method.
          */
      GainType mf_dGetGain( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel ) const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      void mp_Mute( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel );

        /**
          * Implementation of the IMatrixMixer method.
          */
      void mp_UnMute( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel );

        /**
          * Implementation of the IMatrixMixer method.
          */
      const std::string& mf_sGetInputName( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      const std::string& mf_sGetOutputName( const unsigned ac_nChannel ) const;

        /**
          * Selects the master input.
          * In case of a multi-channel master, any of the channels numbers will select it.
          * @param ac_nInputChannel the channel to select
          */
      void mp_SelectMasterInput( const unsigned ac_nInputChannel );

    private:
      void mp_Init( const unsigned ac_nMixerNum );

      MmeMixerData* const   mc_pData;
      MmeVolumeControl*     m_pMasterIn;
      MmeVolumeControl*     m_pMasterOut;
      MmeInputControls*     m_pIns;
      MmeOutputControls*    m_pOuts;
      MixerElementPicker    m_Picker;

      MmeMixer( const MmeMixer& );
      MmeMixer& operator = ( const MmeMixer& );
    };

  }
}

#endif //#ifndef __STRDXMIXER_H__
