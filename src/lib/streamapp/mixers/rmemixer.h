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
 
#ifndef __RMEMIXER_H__
#define __RMEMIXER_H__

#include "mixer.h"
#include "typedefs.h"
#include "containers/matrix.h"

namespace streamapp
{

    /**
      * namespace for RmeAudio specific classes
      ***************************************** */
  namespace rme
  {

     class IRmeMixerInfo;

      /**
        * RmeMixer
        *   IMixer implementation for the (onboard) Rme HDSP matrix mixer
        *   TODO: get gain and find the master headphone channels.
        ***************************************************************** */
    class RmeMixer : public IMatrixMixer
    {
    public:
        /**
          * Constructor.
          */
      RmeMixer();

        /**
          * Destructor.
          */
      ~RmeMixer();

        /**
          * Check if the mixer is available.
          * @return true if so.
          */
      static bool sf_bIsAvailable();

        /**
          * Implementation of the IMixer method.
          */
      mt_Range mf_tGetRange() const;

        /**
          * Mutes all inputs (but sets levale at 0dB),
          * sets all outputs to 0dB and
          * does default 1 to 1 routing for software out -> hardware out.
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
      GainType mf_dGetMasterInputGain() const;

        /**
          * Implementation of the IMixer method.
          */
      GainType mf_dGetMasterInputGain( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMixer method.
          */
      GainType mf_dGetMasterOutputGain() const;

        /**
          * Implementation of the IMixer method.
          */
      GainType mf_dGetMasterOutputGain( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMixer method.
          * Always return 0.
          */
      INLINE unsigned mf_nGetNumMasterInputs() const
      {
        return 0;
      }

        /**
          * Implementation of the IMixer method.
          * Returns 2, we think of the headphone output as the master.
          */
      INLINE unsigned mf_nGetNumMasterOutputs() const
      {
        return 2;
      }

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
      unsigned mf_nGetNumHardwareInputs() const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      unsigned mf_nGetNumSoftwareInputs() const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      unsigned mf_nGetNumHardwareOutputs() const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      const std::string& mf_sGetInputName( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMatrixMixer method.
          */
      const std::string& mf_sGetOutputName( const unsigned ac_nChannel ) const;

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

    private:
      void ThrowIfNotExist() const;

      const rme::IRmeMixerInfo* mc_pMixer;

      RmeMixer( const RmeMixer& );
      RmeMixer& operator = ( const RmeMixer& );
    };


    typedef MatrixStorage<short>  gt_RmeChannelCC;
    typedef MatrixStorage<int>    gt_RmeUnknownCC;
    typedef tStringVector         gt_RmeChannelNames;

      /**
        * IRmeMixer
        *   contains the control codes, the channel names, and the number of channels.
        ****************************************************************************** */
    class IRmeMixerInfo
    {
    public:
        /**
          * Constructor.
          */
      IRmeMixerInfo()
      {
      }

        /**
          * Destructor.
          */
      virtual ~IRmeMixerInfo()
      {
      }

      std::string         mc_sIdentifier;

      unsigned            mc_nHardwareInputs;
      unsigned            mc_nSoftwareInputs;
      unsigned            mc_nOutputs;

      gt_RmeChannelNames* mc_pInputNames;
      gt_RmeChannelNames* mc_pOutputNames;

      gt_RmeChannelCC*    mc_pChannelCC;
      gt_RmeUnknownCC*    mc_pUnknownCC;

    private:
      IRmeMixerInfo( const IRmeMixerInfo& );
      IRmeMixerInfo& operator = ( const IRmeMixerInfo& );
    };

      /**
        * Convert gain in Db to rme short
        */
    short gf_nConvertGain( const GainType ac_dGainInDb );

      /**
        * Convert rme short to gain in Db
        */
    GainType gf_dConvertGain( const short ac_nGainInLin );

      /**
        * Multiface
        *   this is for a single multiface == a 36 * 20 matrix.
        *   18 hardware inputs, 18 software inputs,
        *   16 + 2 + 2 hardware outputs
        ******************************************************* */
    class MultiFace : public IRmeMixerInfo
    {
    public:
      MultiFace();
      ~MultiFace();
    };

      /**
        * MultifaceInputChannelNames
        *   Multiface input channel names.
        ********************************** */
    class MultifaceInputChannelNames : public gt_RmeChannelNames
    {
    public:
        /**
          * Constructor.
          */
      MultifaceInputChannelNames();

        /**
          * Destructor.
          */
      ~MultifaceInputChannelNames();

    private:
        /**
          * Init the names.
          */
      void mp_Init();
    };

      /**
        * MultifaceOutputChannelNames
        *   Multiface input channel names.
        ********************************** */
    class MultifaceOutputChannelNames : public gt_RmeChannelNames
    {
    public:
        /**
          * Constructor.
          */
      MultifaceOutputChannelNames();

        /**
          * Destructor
          */
      ~MultifaceOutputChannelNames();

    private:
        /**
          * Init the names.
          */
      void mp_Init();
    };

      /**
        * MultifaceChannelCC
        *   Multiface Channel Control Codes.
        *********************************** */
    class MultifaceChannelCC : public gt_RmeChannelCC
    {
    public:
        /**
          * Constructor.
          */
      MultifaceChannelCC();

        /**
          * ~MultifaceChannelCC
          */
      ~MultifaceChannelCC();

    private:
        /**
          * Initialize the whole matrix.
          */
      void mp_Init();

        /**
          * Set one column.
          * @param ac_nColumn the column
          * @param ac_nOffset the column's offset code
          */
      void mp_InitColumn( const unsigned ac_nColumn, const short ac_nOffset );
    };

      /**
        * MultifaceUnknownCC
        *   Multiface Unknown Control Codes.
        ************************************ */
    class MultifaceUnknownCC : public gt_RmeUnknownCC
    {
    public:
        /**
          * Constructor.
          */
      MultifaceUnknownCC();

        /**
          * ~MultifaceUnknownCC
          */
      ~MultifaceUnknownCC();

    private:
        /**
          * Init the codes.
          */
      void mp_Init();
    };

  }
}

#endif //#ifndef __RMEMIXER_H__
