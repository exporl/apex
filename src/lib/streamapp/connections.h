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
 
#ifndef __CONNECTIONS_H__
#define __CONNECTIONS_H__

#include "typedefs.h"
#include "multiproc.h"
#include "audioformat.h"
#include "streamutils.h"
#include "callback/callback.h"

#ifdef _MSC_VER
#include <hash_map>
#else
//gcc's (3.3.4) hash_map isn't complete and gives compilation errors (iterators not found, operator!= not found,..)
#include <map>
#endif

namespace appcore
{
  class CriticalSection;
}

namespace streamapp
{

    /**
      * StreamCallback
      *   manages the result of streams and processors.
      *   Streams and processors work on subsequent blocks, so every call gets/sets a
      *   different Stream. This class wraps the objects in a Callback that stores the
      *   result so that the same result can be used an unlimited number of times.
      *   The implementations implement mf_CallbackFunc() in which they set the result.
      *   At the start of a callback, the result is unlocked, when the result is first needed
      *   it is set and then locked for the duration of the callback.
      *   If mf_bIsPlaying() is false, the callback never gets executed.
      *   @see ConnectItem
      **************************************************************************************** */
  class StreamCallback : public Callback
  {
  public:
      /**
        * Constructor.
        * @param ac_bIsPlaying initial play condition
        */
    StreamCallback( const bool ac_bIsPlaying = true ) :
      mc_pResult( 0 ),
      mv_bLocked( false ),
      mv_bIsPlaying( ac_bIsPlaying )
    {
    }

      /**
        * Destructor.
        */
    virtual ~StreamCallback()
    {
    }

      /**
        * Implementation of the Callback method.
        * If locked, does nothing.
        * Else it calls mf_CallbackFunc() and locks afterwards.
        * If deriving from derived classes of StreamCallback,
        * you must override mf_Callback, or tweak mf_CallbackFunc in the derived
        * else callback will be re-entered infinitely.
        */
    virtual void mf_Callback()
    {
      if( mf_bResultLocked() )
        return;
      mf_CallbackFunc();
      mp_LockResult();
    }

      /**
        * Lock the result.
        * Controlled by host.
        */
    INLINE void mp_LockResult()
    {
      mv_bLocked = true;
    }

      /**
        * Unlock the result.
        * Controlled by host.
        */
    INLINE void mp_UnLockResult()
    {
      mv_bLocked = false;
    }

      /**
        * Check locked.
        * @return true if locked
        */
    INLINE const bool& mf_bResultLocked() const
    {
      return mv_bLocked;
    }

      /**
        * Set playing or disabled.
        * Controlled by host.
        * @param ac_bPlay true for playing
        */
    INLINE void mp_SetPlaying( const bool ac_bPlay )
    {
      mv_bIsPlaying = ac_bPlay;
    }

      /**
        * Get playing status.
        * @return true if playing
        */
    INLINE bool mf_bIsPlaying() const
    {
      return mv_bIsPlaying;
    }

      /**
        * Get the result of the operation.
        * This result will always be valid.
        * @return the result
        */
    const Stream& mf_GetResult()
    {
      mf_Callback();
      return *mc_pResult;
    }

      /**
        * Get the number of channels of the result.
        * @return the number
        */
    virtual unsigned  mf_nGetNumChannels() const = 0;

      /**
        * Get the buffersize of the result.
        * @return the size
        */
    virtual unsigned  mf_nGetBufferSize() const = 0;

      /**
        * Callback function to be implemented.
        * This function must set a valid result.
        */
    virtual void      mf_CallbackFunc() = 0;

  protected:
    const Stream* mc_pResult;

  private:
    bool mv_bLocked;
    bool mv_bIsPlaying;
  };

  typedef ArrayStorage<StreamCallback*> CallbackInputs;
  typedef ArrayStorage<unsigned> InputChannels;
  typedef ArrayStorage<bool> tFreeChannels;

    /**
      * FreeChannels
      *   a boolean array used by ConnectItem to keep track of connected channels.
      **************************************************************************** */
  class FreeChannels : public tFreeChannels
  {
  public:
      /**
        * Constructor.
        * @param ac_nSize the number of channels.
        */
    FreeChannels( const unsigned ac_nSize ) :
      tFreeChannels( ac_nSize )
    {
    }

      /**
        * Destructor.
        */
    ~FreeChannels()
    {
    }

      /**
        * Check if specified channel is free.
        * @param ac_nIndex the channel
        * @return true if free
        */
    INLINE const bool& mf_bIsFreeChannel( const unsigned ac_nIndex ) const
    {
      return tFreeChannels::operator() ( ac_nIndex );
    }

      /**
        * Get the index of the first free channel.
        * @return the index or -1 if none free
        */
    int mf_nGetFirstFreeChannel() const
    {
      const unsigned& nChan = tFreeChannels::mf_nGetBufferSize();
      for( unsigned i = 0 ; i < nChan ; ++i )
      {
        if( mf_bIsFreeChannel( i ) )
          return (int) i;
      }
      return -1;
    }

      /**
        * Get the number of free channels.
        * @return the number
        */
    unsigned mf_nGetNumFreeChannels() const
    {
      unsigned nRet = 0;
      const unsigned& nChan = tFreeChannels::mf_nGetBufferSize();
      for( unsigned i = 0 ; i < nChan ; ++i )
        if( mf_bIsFreeChannel( i ) )
          ++nRet;
      return nRet;
    }
  };

    /**
      * ConnectItem
      *   manages connections between StreamCallbacks.
      *   When connected, ConnectItem will use StreamCallback's result, combine it with
      *   the results of other connected items and set this as it's own result.
      *   Suppose StreamCallback str1 with 2 channels, str2 with 1 channel and
      *   ConnectItem con with 4 channels (sockets).
      *   @code
      *   con->mp_AddInput( str1 );
      *   con->mp_AddInput( str2 );
      *   @endcode
      *   When calling mf_CallbackFunc(), con will first call the callbacks of str1
      *   and str2 to get their result, and organize it's result as follows:
          @verbatim
          str1 channel0 -> con result channel 0
          str1 channel1 -> con result channel 1
          str2 channel0 -> con result channel 2
          null          -> con result channel 3
          @endverbatim
      *   No samples are copied, just pointers, and null is a pointer to a Stream
      *   having only zeroes in it.
      *   Multiple connetions from the same item and same channel are allowed.
      ********************************************************************************* */
  class ConnectItem : public StreamCallback
  {
  public:
      /**
        * Constructor.
        * @param ac_nBufferSize the buffersize used
        * @param ac_nSockets the number of connectable channels
        */
    ConnectItem( const unsigned ac_nBufferSize, const unsigned ac_nSockets );

      /**
        * Destructor.
        */
    virtual ~ConnectItem();

      /**
        * Connect an input.
        * Connects as much channels as possible to the first x free channels.
        * @param ac_pItem the item to connect
        */
    virtual void mp_AddInput( StreamCallback* const ac_pItem );

      /**
        * Connect one channel of the input.
        * Connects to the first x free channels.
        * @param ac_pItem the item to connect
        * @param ac_nItemOutChannel the channel to use of ac_pItem
        */
    virtual void mp_AddInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel );

      /**
        * Connect one channel of the input to the specified channel.
        * Connects to ac_nThisInChannel.
        * @param ac_pItem the item to connect
        * @param ac_nItemOutChannel the channel to use of ac_pItem
        * @param ac_nThisInChannel the channel to use here
        */
    virtual void mp_AddInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel, const unsigned ac_nThisInChannel );

      /**
        * Remove the inputchannel specified.
        * Frees the socket.
        * @param ac_nThisInChannel the channel
        */
    virtual void mp_RemoveInput( const unsigned ac_nThisInChannel );

      /**
        * Remove all connections from the item.
        * @param ac_pItem the item
        */
    virtual void mp_RemoveInput( StreamCallback* const ac_pItem );

      /**
        * Remove one connection from the item.
        * @param ac_pItem the item
        * @param ac_nItemOutChannel the channel of the item to disconnect
        */
    virtual void mp_RemoveInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel );

      /**
        * Remove one connection from the item.
        * @param ac_pItem the item
        * @param ac_nItemOutChannel the channel of the item to disconnect
        * @param ac_nThisInChannel this' channel to disconnect from.
        */
    virtual void mp_RemoveInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel, const unsigned ac_nThisInChannel );

      /**
        * Free all connected sockets.
        */
    virtual void mp_RemoveAll();

      /**
        * Get the number of free sockets.
        * @return the number
        */
    unsigned  mf_nGetFreeChannels() const;

      /**
        * Get the index of the first free socket.
        * @return the index or -1 if none free
        */
    int       mf_nGetFirstFreeChannel() const;

      /**
        * Check if the given socket is free.
        * @param ac_nIndex the index
        * @return true if not connected
        */
    bool      mf_bIsFreeChannel( const unsigned ac_nIndex ) const;

      /**
        * Check if this item has any connections at all.
        * @return true if at least one connection
        */
    bool      mf_bIsConnected() const;

      /**
        * Implementation of the StreamCallback method.
        */
    virtual void mf_CallbackFunc();

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE virtual unsigned mf_nGetNumChannels() const
    {
      return mc_nChan;
    }

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE virtual unsigned mf_nGetBufferSize() const
    {
      return mc_nSize;
    }

      /**
        * mt_eConnectionInfo
        *   struct with connection info.
        *   Not used internally, but nice to query connections.
        ******************************************************* */
    struct mt_eConnectionInfo
    {
      mt_eConnectionInfo( StreamCallback* a_pFrom, const unsigned ac_nFromChannel, const unsigned ac_nToChannel ) :
        m_pFrom( a_pFrom ), m_nFromChannel( ac_nFromChannel ), m_nToChannel( ac_nToChannel )
      {}
      mt_eConnectionInfo() :
        m_pFrom( 0 ), m_nFromChannel( 0 ), m_nToChannel( 0 )
      {}
      StreamCallback* m_pFrom;
      unsigned        m_nFromChannel;
      unsigned        m_nToChannel;
    };

      /**
        * Get a list of all connections.
        * @return an array, must be deleted by callee
        */
    DeleteAtDestructionArray<mt_eConnectionInfo>* mf_pGetConnections() const;

  protected:
    const unsigned  mc_nChan;
    const unsigned  mc_nSize;

    CallbackInputs  m_Inputs;
    InputChannels   m_InputChans;
    FreeChannels    m_FreeChan;
    PtrStream       m_Result;

    ConnectItem& operator= ( const ConnectItem& );
    ConnectItem( const ConnectItem& );
  };

    /**
      * InputStreamCallback
      *   wraps InputStream into a StreamCallback.
      ******************************************** */
  class InputStreamCallback : public StreamCallback
  {
  public:
      /**
        * Constructor.
        * @param a_pInput the input stream
        * @param ac_bDeleteInput if true, deletes input when killed
        */
    InputStreamCallback( InputStream* const a_pInput, const bool ac_bDeleteInput = false ) :
      mc_pInput( a_pInput ),
      mc_bDeleteInput( ac_bDeleteInput )
    {
    }

      /**
        * Destructor.
        */
    ~InputStreamCallback()
    {
      if( mc_bDeleteInput )
        delete mc_pInput;
    }

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE virtual void mf_CallbackFunc()
    {
      mc_pResult = &mc_pInput->Read();
    }

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE unsigned mf_nGetNumChannels() const
    {
      return mc_pInput->mf_nGetNumChannels();
    }

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return mc_pInput->mf_nGetBufferSize();
    }

  private:
    InputStream* const  mc_pInput;
    const bool          mc_bDeleteInput;

    InputStreamCallback( const InputStreamCallback& );
    InputStreamCallback& operator = ( const InputStreamCallback& );
  };

    /**
      * ProcessorCallback
      *   wraps IStreamProcessor into a ConnectItem.
      *   A processor is the only item that accepts incoming
      *   and outgoing connections.
      ****************************************************** */
  class ProcessorCallback : public ConnectItem
  {
  public:
      /**
        * Constructor.
        * @param ac_pProc the processor
        * @param ac_bDeleteInput if true, deletes processor when killed
        */
    ProcessorCallback( IStreamProcessor* const ac_pProc, const bool ac_bDeleteInput = false ) :
        ConnectItem( ac_pProc->mf_nGetBufferSize(), ac_pProc->mf_nGetNumInputChannels() ),
      mc_pProc( ac_pProc ),
      mc_bDeleteInput( ac_bDeleteInput )
    {
    }

      /**
        * Destructor.
        */
    ~ProcessorCallback()
    {
      if( mc_bDeleteInput )
        delete mc_pProc;
    }

      /**
        * Implementation of the StreamCallback method.
        */
    virtual void mf_Callback()
    {
      if( mf_bResultLocked() )
        return;
      ConnectItem::mf_CallbackFunc();
      mf_CallbackFunc();
      mp_LockResult();
    }

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE virtual void mf_CallbackFunc()
    {
      mc_pResult = &mc_pProc->mf_DoProcessing( *mc_pResult );
    }

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE unsigned mf_nGetNumChannels() const
    {
      return mc_pProc->mf_nGetNumOutputChannels();  //!!!!!
    }

  private:
    IStreamProcessor* const mc_pProc;
    const bool              mc_bDeleteInput;

    ProcessorCallback( const ProcessorCallback& );
    ProcessorCallback& operator = ( const ProcessorCallback& );
  };

    /**
      * OutputStreamCallback
      *   wraps OutputStream into a ConnectItem.
      *   Note that the result is the input, since
      *   there is no output.
      ********************************************* */
  class OutputStreamCallback : public ConnectItem
  {
  public:
      /**
        * Constructor.
        * @param ac_pOutput the output stream
        * @param ac_bDeleteInput if true, deletes ac_pOutput when killed
        */
    OutputStreamCallback( OutputStream* const ac_pOutput, const bool ac_bDeleteInput = false ) :
        ConnectItem( ac_pOutput->mf_nGetBufferSize(), ac_pOutput->mf_nGetNumChannels() ),
      mc_pOutput( ac_pOutput ),
      mc_bDeleteInput( ac_bDeleteInput )
    {
    }

      /**
        * Destructor.
        */
    ~OutputStreamCallback()
    {
      if( mc_bDeleteInput )
        delete mc_pOutput;
    }

      /**
        * Implementation of the StreamCallback method.
        */
    virtual void mf_Callback()
    {
      if( mf_bResultLocked() )
        return;
      ConnectItem::mf_CallbackFunc();
      mf_CallbackFunc();
      mp_LockResult();
    }

      /**
        * Implementation of the StreamCallback method.
        */
    INLINE virtual void mf_CallbackFunc()
    {
      mc_pOutput->Write( *mc_pResult );
    }

  private:
    OutputStream* const mc_pOutput;
    const bool          mc_bDeleteInput;

    OutputStreamCallback( const OutputStreamCallback& );
    OutputStreamCallback& operator = ( const OutputStreamCallback& );
  };

  #ifdef _MSC_VER
  typedef stdext::hash_map<std::string,ConnectItem*> tConnectable;
  typedef stdext::hash_map<std::string,StreamCallback*> tUnConnectable;
  #else
  typedef std::map<std::string,ConnectItem*> tConnectable;
  typedef std::map<std::string,StreamCallback*> tUnConnectable;
  #endif

    /**
      * ConnectionManager
      *   manages items and connections between items.
      *   Items are first registered using a string as id.
      *   Afterwards connections are made by specifying the id
      *   and eventually the channel numbers to connect.
      *   Call mf_Callback to execute the entire network of connections.
      *   This will call all needed Read(), Write() and mf_DoProcessing()
      *   calls in the right order, and each of them only once per callback.
      *   All methods are thread-safe.
      *   TODO might be optimized by calculating network order on beforehand
      *********************************************************************** */
  class ConnectionManager : public Callback
  {
  public:
      /**
        * Constructor.
        */
    ConnectionManager();

      /**
        * Destructor.
        */
    ~ConnectionManager();

      /**
        * Register an InputStream.
        * @param ac_sId the id, must be unique
        * @param ac_pInput the stream
        * @param ac_bPlaying initial state
        */
    void mp_RegisterItem( const std::string& ac_sId, InputStream* const ac_pInput, const bool ac_bPlaying = true );

      /**
        * Register an OutputStream.
        * @param ac_sId the id, must be unique
        * @param ac_pOutput the stream
        */
    void mp_RegisterItem( const std::string& ac_sId, OutputStream* const ac_pOutput );

      /**
        * Register an IStreamProcessor.
        * @param ac_sId the id, must be unique
        * @param ac_pProcessor the processor
        */
    void mp_RegisterItem( const std::string& ac_sId, IStreamProcessor* const ac_pProcessor );

      /**
        * Register a ConnectItem.
        * @param ac_sId the id, must be unique
        * @param ac_pInput the item
        */
    void mp_RegisterItem( const std::string& ac_sId, ConnectItem* const ac_pInput );

      /**
        * Connect all channels from both items.
        * Connects as much as possible to the first free channels.
        * @param ac_sFromID origin
        * @param ac_sToID destination
        */
    void mp_Connect( const std::string& ac_sFromID, const std::string& ac_sToID );

      /**
        * Connect a single channel from both items.
        * @param ac_sFromID origin
        * @param ac_sToID destination
        * @param ac_nFromChannel origin channel
        * @param ac_nToChannel destination channel
        */
    void mp_Connect( const std::string& ac_sFromID, const std::string& ac_sToID, const unsigned ac_nFromChannel, const unsigned ac_nToChannel );

      /**
        * Remove all connections between two items.
        * @param ac_sFromID origin
        * @param ac_sToID destination
        */
    void mp_RemoveConnection( const std::string& ac_sFromID, const std::string& ac_sToID );

      /**
        * Remove a specific connection between two items.
        * @param ac_sFromID origin
        * @param ac_sToID destination
        * @param ac_nFromChannel origin channel
        * @param ac_nToChannel destination channel
        */
    void mp_RemoveConnection( const std::string& ac_sFromID, const std::string& ac_sToID, const unsigned ac_nFromChannel, const unsigned ac_nToChannel );

      /**
        * Unregister an item, hereby removing all it's connections.
        * @param ac_sID the unique id.
        */
    void mp_UnregisterItem( const std::string& ac_sID );

      /**
        * Set to use all items.
        */
    void mp_PlayAllItems();

      /**
        * Set to use a specific item.
        * If an item is set to "not playing", all items needing input from the first
        * item will only receive zeroes at their sockets.
        * @param ac_sItem the item's id
        * @param ac_bPlay if false, item is not used.
        */
    void mp_PlayItem( const std::string& ac_sItem, const bool ac_bPlay = true );

      /**
        * Check if the id is for a registered item.
        * @param ac_sItem the id
        * @return true if id exists and is registered
        */
    bool mf_bIsRegistered( const std::string& ac_sItem ) const;

      /**
        * Check if all inputs have connections.
        * If an input is unconnected but has it's play flag set,
        * input won't be read hence classes waiting for all input
        * to be read completely will never be triggered.
        * @return false if any input has no connections at all
        */
    bool mf_bAllInputsConnected() const throw();

      /**
        * Call delete on all items still registered.
        */
    void mp_DeleteRegisteredItems() throw();

      /**
        * Get a vector of strings displaying all connections made.
        * @return vector of strings
        */
    tStringVector mf_saGetRoutes() const;

      /**
        * Implementation of the Callback method.
        * All connections are executed here.
        */
    void mf_Callback();

  private:
    int             mf_nGetItemIndex( const std::string& ac_sID ) const;
    ConnectItem*    mf_pGetConnectable( const std::string& ac_sID ) const;
    StreamCallback* mf_pGetUnConnectable( const std::string& ac_sID ) const;
    StreamCallback* mf_pGetAny( const std::string& ac_sID ) const;
    std::string     mf_sGetAny( StreamCallback* const ac_pID ) const;  //!slow, searching by val

    tConnectable    m_Connectable;
    tUnConnectable  m_UnConnectable;

    const appcore::CriticalSection* mc_pLock;

    ConnectionManager( const ConnectionManager& );
    ConnectionManager& operator = ( const ConnectionManager& );
  };

}

#endif //#ifndef __CONNECTIONS_H__
