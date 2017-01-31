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
 
#ifndef __MIXERCALLBACK_H__
#define __MIXERCALLBACK_H__

#include "connections.h"

namespace streamapp
{
  class MatrixMixer;
  class OutputStream;
  class IStreamProcessor;
}
using namespace streamapp;

namespace apex
{
  namespace stimulus
  {

      /**
        * ApexMixerCallback
        *   ConnectItem implementation for Apex.
        *   Is implemented as a normal ConnectItem, but with a
        *   MatrixMixer in front of it, which allows to connect
        *   any number of inputs to an output.
        *   Normally a connectitem has as much input channels as the
        *   stream/processor inside it, by putting a mixer before,
        *   we can easily connect a number of outputs to the same input.
        *   The number of output channels is still the number of outputs
        *   from the stream/proc inside.
        *   Implementations have to set #mixer outputs, upon
        *   connecting the configuration will look like this:
        *
        *   for a 2 channel outputstream with 4 connected inputs from 3 inputstreams
        *   configured as
        *     ch0 -> ch0  //
        *     ch1 -> ch1  //stream 0
        *     ch0 -> ch1  //stream 1
        *     ch0 -> ch0  //stream 2
        *
        *     output channel      mixer inputchannel    mixer outputchannel(==OutputStream input channel)
        *                                                     0     1
        *           0           ->          0               gain  mute
        *           1           ->          1               mute  gain
        *           0           ->          2               mute  gain
        *           0           ->          3               gain  mute
        *
        *************************************************************************************************** */
    class IApexMixerCallback : public ConnectItem
    {
    protected:
      /**
        * Protected Constructor.
        * @param ac_nMaxNumInputs number of mixer inputs.
        * @param ac_nNumOutputs number of outputs
        * @param ac_nBufferSize buffersize
        */
      IApexMixerCallback( const unsigned ac_nMaxNumInputs, const unsigned ac_nNumOutputs, const unsigned ac_nBufferSize );
    public:
      /**
        * Destructor.
        */
      virtual ~IApexMixerCallback();

        /**
          * Implementation of the ConnectItem method.
          */
      virtual void mp_AddInput( StreamCallback* const ac_pItem );

        /**
          * Implementation of the ConnectItem method.
          */
      virtual void mp_AddInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel );

        /**
          * Implementation of the ConnectItem method.
          */
      virtual void mp_AddInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel, const unsigned ac_nThisInChannel );

        /**
          * Implementation of the ConnectItem method.
          */
      virtual void mp_RemoveInput( const unsigned ac_nThisInChannel );

        /**
          * Implementation of the ConnectItem method.
          */
      virtual void mp_RemoveInput( StreamCallback* const ac_pItem, const unsigned ac_nFromChannel, const unsigned ac_nToChannel );

        /**
          * Implementation of the StreamCallback method.
          */
      virtual void mf_Callback();

        /**
          * Implementation of the StreamCallback method.
          */
      virtual void mf_CallbackFunc();

        /**
          * Set the global mixer output gain.
          * This can be quite slow because all nodes have to be set.
          * @param ac_dGainIndB gain in dB
          */
      void mp_SetGain( const double ac_dGainIndB ); //slow..

        /**
          * Set the mixer gain at the given output channel.
          * @param ac_dGainIndB gain in dB
          * @param ac_nChannel the
          */
      void mp_SetGain( const double ac_dGainIndB, const unsigned ac_nChannel );

        /**
          * Get the global gain.
          * @return the gain.
          */
      INLINE double mf_dGetGain() const
      { return mv_dGain; }


    private:
      double              mv_dGain;
      MatrixMixer* const  mc_pMixer;
    };

      /**
        * ApexOutputCallback
        *   IApexMixerCallback implementation for OutputStreams.
        ******************************************************** */
    class ApexOutputCallback : public IApexMixerCallback
    {
    public:
        /**
          * Constructor.
          * @param ac_pOutput the output stream
          * @param ac_nMaxNumInputs number of mixer inputs.
          * @param ac_bDeleteOutput true if ac_pOuput should be deleted
          */
      ApexOutputCallback( OutputStream* const ac_pOutput, const unsigned ac_nMaxNumInputs, const bool ac_bDeleteOutput = true );

        /**
          * Destructor.
          */
      ~ApexOutputCallback();

        /**
          * Implementation of the StreamCallback method.
          */
      void mf_Callback();

        /**
          * Implementation of the StreamCallback method.
          * This will write the StreamCallback::mc_pResult to the OutputStream
          */
      void mf_CallbackFunc();

    private:
      const bool          mc_bDeleteOutput;
      OutputStream* const mc_pOutput;
    };

      /**
        * ApexProcessorCallback
        *   IApexMixerCallback implementation for Processors.
        ***************************************************** */
    class ApexProcessorCallback : public IApexMixerCallback
    {
    public:
        /**
          * Constructor.
          * @param ac_pProc the processor
          * @param ac_nMaxNumInputs number of mixer inputs.
          */
      ApexProcessorCallback( IStreamProcessor* const ac_pProc, const unsigned ac_nMaxNumInputs );

        /**
          * Destructor.
          */
      ~ApexProcessorCallback();

        /**
          * Implementation of the StreamCallback method.
          * This will write the StreamCallback::mc_pResult to the OutputStream
          */
      void mf_Callback();

        /**
          * Implementation of the StreamCallback method.
          * This will set the StreamCallback::mc_pResult to the IStreamProcessor result.
          */
      void mf_CallbackFunc();

    private:
      IStreamProcessor* const mc_pProc;
    };

  }
}

#endif //#ifndef __MIXERCALLBACK_H__
