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

#ifndef _APEX_SRC_LIB_STREAMAPP_MULTIPROC_H__
#define _APEX_SRC_LIB_STREAMAPP_MULTIPROC_H__

#include "stream.h"
#include "containers/dynarray.h"

namespace streamapp
{

     /**
       * The container used in SequentialProcessor.
       */
   typedef DynamicArray<IStreamProcessor*> DynProcList;

    /**
      * SequentialProcessor
      *   combines any number of subsequent IStreamProcessors into one IStreamProcessor.
      *   Any processor added must have the same number of inputs as the numkber of outputs
      *   of the one before it. Buffersize must be the same for all processors added.
      ************************************************************************************* */
  class SequentialProcessor : public IStreamProcessor, public DynProcList
  {
  public:
      /**
        * Constructor.
        * @param ac_bDeleteProcsWhenDeleted if true, deletes the items added when destructing or removing an item.
        */
    SequentialProcessor( const bool ac_bDeleteProcsWhenDeleted = true );

      /**
        * Destructor.
        * Calls mp_RemoveAllItems().
        * @see mp_RemoveAllItems
        */
    ~SequentialProcessor();

      /**
        * Add a processor to the end of the list.
        * @param a_pItem the processor
        */
    void mp_AddItem( IStreamProcessor* a_pItem );

      /**
        * Remove a processor.
        * If ac_bDeleteInputsWhenDeleted is true, deletes the processor.
        * @param a_pItem
        */
    void mp_RemoveItem( IStreamProcessor* a_pItem );

      /**
        * Remove all processors from the list.
        * If ac_bDeleteInputsWhenDeleted is true, also deletes them.
        */
    void mp_RemoveAllItems();

      /**
        * Get the number of processors currently in the list.
        */
    INLINE const unsigned& mf_nGetNumItems() const
    {
      return DynProcList::mf_nGetNumItems();
    }

      /**
        * Implementation of the IStreamProcessor method.
        * Get first processor's number of input channels.
        * @return the number
        */
    unsigned mf_nGetNumInputChannels() const;

      /**
        * Implementation of the IStreamProcessor method.
        * Get the last processor's number of output channels.
        */
    unsigned mf_nGetNumOutputChannels() const;

      /**
        * Implementation of the IStreamProcessor method.
        */
    const Stream& mf_DoProcessing( const Stream& ac_Output );

  private:
    const bool mc_bDeleteContent;

    SequentialProcessor( const SequentialProcessor& );
    SequentialProcessor& operator = ( const SequentialProcessor& );
  };

    /**
      * SlotProcessor
      *   abstraction of an audio bus.
      *   An audio bus has a number of slots, into which a processor can
      *   be inserted. Found in eg Cubase, Logic or decent hardware mixers.
      *   Is like SequentialProcessor, but with a predefined number of 'slots',
      *   which each can be bypassed; uses the same rules.
      *   Also has global pre- and post gain.
      *   @see SequentialProcessor
      ************************************************************************* */
  class SlotProcessor : public IStreamProcessor
  {
  public:
      /**
        * Constructor.
        * @param ac_nSlots the number of slots to use.
        */
    SlotProcessor( const unsigned ac_nSlots );

      /**
        * Destructor.
        * Removes any processors that are still in use.
        * @see mp_RemoveAllItems()
        */
    virtual ~SlotProcessor();

      /**
        * Set the processor to insert at the specified location.
        * @param ac_nItem the slot number
        * @param ac_pItem the processor
        * @param ac_bDeleteWhenDone if true, deletes the processor when it's plugged out
        * @param ac_bBypassNow if true, immedeatly bypasses the processor
        */
    void mp_SetItem( const unsigned ac_nItem, IStreamProcessor* const ac_pItem,
                     const bool ac_bDeleteWhenDone = true, const bool ac_bBypassNow = false );

      /**
        * Remove a processor at the specified location.
        * Will be deleted it told so in mp_SetItem().
        * @see mp_SetItem()
        * @param ac_nItem the slot number
        */
    void mp_RemoveItem( const unsigned ac_nItem );

      /**
        * Remove everything from all slots.
        * @see mp_RemoveItem()
        */
    void mp_RemoveAllItems();

      /**
        * Set the gain to be applied to the incoming signal.
        * @param ac_dGain the gain (linaer scale, no dB)
        */
    void mp_SetPreGain( const double ac_dGain )
    { mv_dPreGain = ac_dGain; }

      /**
        * Set the gain to be applied to the outgoing signal.
        * @param ac_dGain the gain (linaer scale, no dB)
        */
    void mp_SetPostGain( const double ac_dGain )
    { mv_dPostGain = ac_dGain; }

      /**
        * Implementation of the IStreamProcessor method.
        * Get first processor's number of input channels.
        * @return the number
        */
    unsigned mf_nGetNumInputChannels() const;

      /**
        * Implementation of the IStreamProcessor method.
        * Get the last processor's number of output channels.
        */
    unsigned mf_nGetNumOutputChannels() const;

      /**
        * Implementation of the IStreamProcessor method.
        */
    const Stream& mf_DoProcessing( const Stream& ac_Output );

  private:
    const unsigned                  mc_nSlots;
    double                          mv_dPreGain;
    double                          mv_dPostGain;
    ArrayStorage<bool>              m_bDelete;
    ArrayStorage<bool>              m_bBypass;
    ArrayStorage<IStreamProcessor*> m_Items;

    SlotProcessor( const SlotProcessor& );
    SlotProcessor& operator = ( const SlotProcessor& );
  };

}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_MULTIPROC_H__
