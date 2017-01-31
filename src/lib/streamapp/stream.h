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
    * @file stream.h
    *
    * Contains the definitions of the 3 most important elements in streamapp:
    * -# the Input, of which data is acquired ( Read() ), aka the source
    * -# the Output, to which data is given( Write() ), aka the sink
    * -# The Processor, which takes data and puts it out again ( Write() followed by Read() == Process() )
    *
    * With these three one can describe the entire audio/video/whatever data operations: I->P->O.
    * Off course I->O is also possible, but there aren't anymore schemes to be found on this world.
    * Unless when you think of an Input as a Processor that only outputs stuff without needing input,
    * and of an Output as a Processor that only takes stuff without giving anything back,
    * then theres just P, which looks quite boring and uncreative to me, and that is probably
    * also the reason why I have never seen it in real life.
    */

#ifndef __STREAM_H_
#define __STREAM_H_

#include "streamappdefines.h"

namespace streamapp
{

    /**
      * IStreamProcessor
      *   base class for processors.
      *   A processor takes a Stream for input, and outputs a Stream again,
      *   possibly processed in some way. Processors like buffers won't do
      *   anything with the input. Processing can be done in two ways:
      *   if the processing is replacing, the contents of the input itself are
      *   modified, else the input remains untouched and a different buffer is
      *   returned as output. The way all streamapp components work now, is to
      *   always implement the non-replacing method and to ignore the
      *   mf_bCanProcessReplacing() method. This is very likely to change: all
      *   processors that can support it will provide the in-place method too.
      *
      *   The methods querying channels and size can return 0, which indicates
      *   the processor can take any size as input, and returns the same size
      *   as output, example that clips input (positive only):
      *   @code
      *   virtual void mf_DoProcessReplacing( Stream& a_ToProcess )
      *   {
      *     const unsigned nChan = ac_ToProcess.mf_nGetNumChannels();
      *     const unsigned nSize = ac_ToProcess.mf_nGetBufferSize();
      *     for( unsigned i = 0 ; i < nChan ; ++i )
      *     {
      *       for( unsigned j = 0 ; j < nSize ; ++j )
      *       {
      *         if( a_ToProcess( i, j ) > myclipval )
      *           a_ToProcess.mp_Set( i, j, myclipval );
      *       }
      *     }
      *   }
      *   @endcode
      *
      *   Replacing example, reduces the amount of memory needed, in fact,
      *   only one buffer is needed, which is nice on systems with limited
      *   resources, eg a dsp:
      *   @code
      *   InputStream inputs;
      *   OutputStream outputs;
      *   IStreamProcessor proc;
      *   Stream& input = inputs.Read();
      *   proc.mf_DoProcessReplacing( input );
      *   outputs.Write( input );
      *   @endcode
      *
      *   Non-replacing, needed when same input is used in different places:
      *   @code
      *   InputStream inputs;
      *   OutputStream outputs;
      *   OutputStream outputs2;
      *   IStreamProcessor proc;
      *   IStreamProcessor proc2;
      *   const Stream& input = inputs.Read();
      *   const Stream& result1 = proc.mf_DoProcessing( input );
      *   const Stream& result2 = proc2.mf_DoProcessing( input );
      *   outputs.Write( result1 );
      *   outputs2.Write( result2 );
      *   @endcode
      ************************************************************************ */
  class IStreamProcessor
  {
  protected:
      /**
        * Protected Constructor.
        */
    IStreamProcessor()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~IStreamProcessor()
    {
    }

      /**
        * Get the number of input channels for processing.
        * @return the number, or 0 for any
        */
    INLINE virtual unsigned mf_nGetNumInputChannels() const
    {
      return 0;
    }

      /**
        * Get the number of output channels from processing.
        * Not necessarily the same as mf_nGetNumInputChannels().
        * @return the number, or 0 for any.
        */
    INLINE virtual unsigned mf_nGetNumOutputChannels() const
    {
      return mf_nGetNumInputChannels();
    }

      /**
        * Get the buffer size in samples for processing.
        * Returns the size of the output, which is per definition
        * *always* the same as the size of the input, no excpetions
        * are allowed, audio comes in blocks in the digital domain. ALWAYS.
        * @return the number of samples of one channel.
        */
    INLINE virtual unsigned mf_nGetBufferSize() const
    {
      return 0;
    }

      /**
        * Query if the class supports the mf_DoProcessReplacing() method.
        * @return true if so.
        */
    INLINE virtual bool mf_bCanProcessReplacing() const
    {
      return false;
    }

      /**
        * Do processing.
        * Is not allowed to change the input samples, although some classes
        * will ignore this rule since mf_DoProcessReplacing isn't hardly used
        * anywhere, yet. This will be documented however so there are no surprises.
        * @param ac_Input the Stream to process
        * @return the resulting Stream, can be same ac ac_Input if nothing is done with it
        */
    INLINE virtual const Stream& mf_DoProcessing( const Stream& ac_Input )
    {
      return ac_Input;
    }

      /**
        * Do in-place processing.
        * Is allowed to change the input.
        * @param a_ToProcess the input, will contain the result afterwards
        */
    INLINE virtual void mf_DoProcessReplacing( Stream& /*a_ToProcess*/ )
    {
    }
  };

    /**
      * StreamProcessor
      *   the IStreamProcessor implementation including channel and size members.
      *   Provided for convenience.
      *************************************************************************** */
  class StreamProcessor : public IStreamProcessor
  {
  public:
      /**
        * Constructor.
        * Sets the members.
        * @param ac_nChannels the number of input and output channels
        * @param ac_nBufferSize the size in samples of one channel
        */
    StreamProcessor( const unsigned ac_nChannels, const unsigned ac_nBufferSize ) :
      mc_nChan( ac_nChannels ),
      mc_nSize( ac_nBufferSize )
    {
    }

      /**
        * Destructor.
        */
    virtual ~StreamProcessor()
    {
    }

      /**
        * Implementation of the IStreamProcessor method.
        */
    INLINE unsigned mf_nGetNumInputChannels() const
    {
      return mc_nChan;
    }

      /**
        * Implementation of the IStreamProcessor method.
        */
    INLINE unsigned mf_nGetNumOutputChannels() const
    {
      return mc_nChan;
    }

      /**
        * Implementation of the IStreamProcessor method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return mc_nSize;
    }

  protected:
    const unsigned mc_nChan;
    const unsigned mc_nSize;

    StreamProcessor( const StreamProcessor& );
    StreamProcessor& operator = ( const StreamProcessor& );
  };

    /**
      * InputStream
      *   interface for anything that can read a Stream subsequently.
      *   Starting point of streaming, always contains memory in some way.
      *   Every call to Read() returns a reference to memory containing new
      *   samples. This memory is not necessarily always the same.
      ********************************************************************* */
  class InputStream
  {
  protected:
      /**
        * Protected Constructor.
        */
    InputStream()
    {
    }

  public:
      /**
        * ~Destructor.
        */
    virtual ~InputStream()
    {
    }

      /**
        * Read a block of samples.
        * @return const reference to the samples.
        */
    virtual const Stream& Read() = 0;

      /**
        * Query the state.
        * When mf_bIsEndOfStream() returns false, this indicates
        * the source is exhausted, and no new samples are available.
        * Implementations are expected to fill their entire memory with
        * zeroes after the last sample has been read, in order not to return
        * any garbage (you don't want to hear the last few samples over and over again, do you?)
        * @return
        */
    INLINE virtual bool mf_bIsEndOfStream() const
    {
      return false;
    }

      /**
        * The number of channels contained in the Stream returned by Read().
        * This must not change during streaming.
        * @see Read()
        * @return the number
        */
    virtual unsigned mf_nGetNumChannels() const = 0;

      /**
        * The number of samples contained in one channel of the Stream returned by Read().
        * This must not change during streaming.
        * @return the number
        */
    virtual unsigned mf_nGetBufferSize() const = 0;

      /**
        * The number of samples actually read during the last Read() call.
        * Equal to or less then mf_nGetBufferSize(). If it's less, the
        * mf_bIsEndOfStream() returns true.
        * If Read() hasn't been called yet this method also returns 0.
        * TODO implement this everywhere.
        * Example:
        * @code
        * while( !s.mf_bIsEndOfStream() )
        *   s.Read();
        * std::cout << "The last call to Read() only had " << mf_lGetNumSamplesRead() << " new samples.";
        * std::cout << " The remaining " << s.mf_nGetBufferSize() - s.mf_lGetNumSamplesRead() << " were all zero.\n";
        * @endcode
        * @return the number of new samples from the last Read() call.
        */
    INLINE virtual unsigned mf_nGetNumSamplesRead() const
    {
      return 0;
    }
  };

    /**
      * PositionableInputStream
      *   an InputStream that knows about time.
      *   InputStream is a bit "infinite", in the sense that no-one can tell on
      *   beforehand when the stream will be exhausted. Moreover, it probably never will,
      *   like with a soundcard: there are always new samples when the soundcard
      *   is running, and when it is stopped one can't say it's end of stream, it
      *   is not, it's just paused, and the last Read() call did contain all new samples.
      *   For files on the other hand, it will stop, and one can tell on beforehand
      *   when that will be, since the number of samples is always known. It's not
      *   likely there will ever be an infinitely big harddisk.
      *
      *   Enough pilosophy, here's PositionableInputStream: it has the ability to
      *   query the number of samples left as well as to set the next read position.
      *   Combining these easily leads to the fact you can loop it, hereby making it act
      *   like it's parent, the great infinite InputStream.
      *
      *   When a PositionableInputStream is looped, it will automatically put the first
      *   sample after the mf_lTotalSamples()th one, independent of the position in the
      *   buffer where this should happen. Eg with num loops == 2, for the outside world
      *   it just looks like the Stream is twice as long.
      *   If you're too lazy or stupid to implement looping, you can
      *   just have mf_bCanLoop() return false...
      *   Btw the reason for putting looping in this class although it might seem more
      *   logical to supply a utility class to do it, using this class' mf_lSamplesLeft()
      *   and other methods, is that it is way faster because the implementation can
      *   use the same as it uses for reading the samples, without having to copy anything.
      ************************************************************************************* */
  class PositionableInputStream : public InputStream
  {
  protected:
      /**
        * Protected Constructor.
        */
    PositionableInputStream()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~PositionableInputStream()
    {
    }

      /**
        * Redeclaration of the InputStream method.
        */
    virtual const Stream& Read() = 0;

      /**
        * Get the samplerate.
        * @return the samplerate
        */
    virtual unsigned long mf_lSampleRate() const = 0;

      /**
        * Get the number of samples left to stream.
        * This is normally equal to mf_lTotalSamples() - mf_lCurrentPosition().
        * @return the number
        */
    virtual unsigned long mf_lSamplesLeft() const = 0;

      /**
        * Get the total number of samples available.
        * @return the number
        */
    virtual unsigned long mf_lTotalSamples() const = 0;

      /**
        * Get the position of next sample ( = first one of next block ) going to be read.
        * @return the position, somewhere between 0 and mf_lTotalSamples()
        */
    virtual unsigned long mf_lCurrentPosition() const = 0;

      /**
        * Set next sample position.
        * Sets the position of the next Stream() going to be read.
        * @see mf_lCurrentPosition()
        * @param ac_nPosition the position, must be between 0 and mf_lTotalSamples()
        */
    virtual void mp_SeekPosition( const unsigned long ac_nPosition ) = 0;

      /**
        * Check if the implementation supports looping.
        * @return false if it does not support it.
        */
    virtual bool mf_bCanLoop() const = 0;

      /**
        * Set the number of loops.
        * Setting num loops while playing will only succeed if ac_nLoops is greater
        * then the current number of loops, and if eof has not been reached yet,
        * since we cannot go back in time. (yet)
        * @param ac_nLoops the new number of loops.
        *
        */
    INLINE virtual void mp_SetNumLoops( const unsigned /*ac_nLoops*/ )
    {
    }

      /**
        * Get the current number of loops.
        * 1 means it's not looping.
        * @return the number
        */
    INLINE virtual unsigned mf_nGetNumLoops() const
    {
      return 1;
    }

      /**
        * Get the number of loops so far.
        * @return the number
        */
    INLINE virtual unsigned mf_nGetNumLooped() const
    {
      return 0;
    }

      /**
        * Query if the stream is looping.
        * @return true if number of loops is bigger then one
        */
    INLINE virtual bool mf_bIsLooping() const
    {
      return false;
    }
  };

    /**
      * OutputStream
      *   interface for anything that can write a Stream subsequently.
      *   Endpoint of streaming operation.
      ***************************************************************** */
  class OutputStream
  {
  protected:
      /**
        * Protected Constructor.
        */
    OutputStream()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~OutputStream()
    {
    }

      /**
        * Write a block of samples.
        * Only writes as much channels and samples as the properties state,
        * but it's no problem to pass a lot more in. Depending on the implementation,
        * it's also no problem to pass in less: it will just write as much as put in.
        * @param ac_Output the samples
        */
    virtual void Write( const Stream& ac_Output ) = 0;

      /**
        * Query the state.
        * Only returns true if there's something seriously wrong,
        * eg when the disk isk full.
        * @return false if everything is ok
        */
    INLINE virtual bool mf_bIsEndOfStream() const
    {
      return false;
    }

      /**
        * Get the maximum number of channels that can be written by Write().
        * @return the number
        */
    virtual unsigned mf_nGetNumChannels() const = 0;

      /**
        * Get the maximum number of samples that can be written by Write().
        * @return the number
        */
    virtual unsigned mf_nGetBufferSize() const = 0;

      /**
        * Get the number of samples written so far.
        * By my knowledge, no class implements this, it's here just for decoration.
        * TODO
        * @return the number

    INLINE virtual unsigned long mf_lGetNumSamplesWritten() const
    {
      return 0;
    }*/
  };

}

#endif //#ifndef __STREAM_H_
