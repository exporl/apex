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
 
#ifndef __MIXERELEMENT_H__
#define __MIXERELEMENT_H__

#include "streamappdefines.h"
#include "containers/ownedarray.h"
#include "utils/stringexception.h"

namespace streamapp
{

    /**
      * IMixerElement
      *   represents a single mixer control element.
      *   The element has a number of channles, of
      *   which the gain can be set individually.
      ********************************************** */
  class IMixerElement
  {
  protected:
      /**
        * Protected Constructor.
        */
    IMixerElement()
    {}

  public:
      /**
        * Destructor.
        */
    ~IMixerElement()
    {}

      /**
        * Get number of channels
        * @return the number
        */
    virtual unsigned mf_nGetNumChannels() const = 0;

      /**
        * Set
        * @param ac_dGainIndB the value
        * @param ac_nChannel the channel, must be < mf_nGetNumChannels()
        */
    virtual void mp_SetValue( const GainType ac_dGainIndB, const unsigned ac_nChannel ) = 0;

      /**
        * Get
        * @param ac_nChannel the channel, must be < mf_nGetNumChannels()
        */
    virtual GainType mf_dGetValue( const unsigned ac_nChannel ) const = 0;

      /**
        * Get the name of a channel
        * @param ac_nChannel the channel, must be < mf_nGetNumChannels()
        * @return the name, or an empty string when no naming is supported
        */
    virtual const std::string& mf_sGetName( const unsigned ac_nChannel ) const = 0;
  };

    /**
      * IMixerElements
      *   array of mixer elements.
      *   Take care when using this, the mp_AddItem is reimplemented,
      *   so eg removing an item will leave the number of channels unchanged.
      *   TODO finish this
      *********************************************************************** */
  typedef OwnedArray<IMixerElement> tIMixerElements;
  class IMixerElements : public tIMixerElements
  {
  public:
      /**
        * Constructor
        * @param ac_nInitSize the initial size of the array
        * @param ac_nGrowSize the grow/shrink size
        */
    IMixerElements( const unsigned ac_nInitSize = 0, const unsigned ac_nGrowSize = 2 ) :
      tIMixerElements( ac_nInitSize, ac_nGrowSize ),
      mv_nChannels( 0 )
    {
    }

      /**
        * Destructor.
        */
    virtual ~IMixerElements()
    {
    }

      /**
        * Add an element.
        * @param ac_Item the element
        */
    INLINE void mp_AddItem( IMixerElement* ac_Item )
    {
      tIMixerElements::mp_AddItem( ac_Item );
      mv_nChannels += ac_Item->mf_nGetNumChannels();
    }

      /**
        * Get the total number of channels from all elements.
        * @return the number
        */
    INLINE const unsigned& mf_nGetNumChannels() const
    {
      return mv_nChannels;
    }

  private:
    unsigned mv_nChannels;
  };


    /**
      * MixerElementPicker
      *   given arrays of IMixerElements, picks out the
      *   right element given an input channel number.
      ************************************************** */
  class MixerElementPicker
  {
  public:
      /**
        * Constructor.
        */
    MixerElementPicker() :
      mv_nChannels( 0 )
    {
    }

      /**
        * Destructor.
        */
    ~MixerElementPicker()
    {
    }

      /**
        * Get the total number of available channels.
        * @return the number off course
        */
    INLINE const unsigned mf_nGetNumChannels() const
    {
      return mv_nChannels;
    }

      /**
        * Get the element on the specified node
        * @param ac_nInputChannel the input
        * @return the element and it's channel number
        * @throw StringException if not found
        *
        * example:
        *   suppose 3 input elements added, with 1, 2 and 1 channels resp.
        *   mf_pGetElement( 2 ) will return the second element:
        *
        *   elem0    0
        *   elem1    1
        *   elem1    2      ->this is channel 2, aka channel 1 of element 1
        *   elem2    3
        *
        */
    IMixerElement* mf_pGetElement( const unsigned ac_nInputChannel ) const
    {
      unsigned nChan = 0;
      const unsigned nItems = m_Elements.mf_nGetNumItems();
      for( unsigned i = 0 ; i < nItems; ++i )
      {
        const tIMixerElements* pCur = m_Elements.mf_GetItem( i );
        const unsigned nCurItems = pCur->mf_nGetNumItems();
        for( unsigned i = 0 ; i < nCurItems ; ++i )
        {
          IMixerElement* pCurEl = pCur->mf_GetItem( i );
          const unsigned nCurChan = pCurEl->mf_nGetNumChannels();
          for( unsigned i = 0 ; i < nCurChan ; ++i )
          {
            if( nChan == ac_nInputChannel )
              return pCurEl;
            ++nChan;
          }
        }
      }
      throw( utils::StringException( "MixerElementPicker::mf_GetElement(): element not found" ) );
    }

      /**
        * struct with search result
        */
    struct mt_SearchElement
    {
      mt_SearchElement( const tIMixerElements*  ac_Elements,
                        const unsigned          ac_ElementIndex ) :
        mc_Elements( ac_Elements ),
        mc_ElementIndex( ac_ElementIndex )
      {}

      const tIMixerElements*  mc_Elements;
      unsigned                mc_ElementIndex;
    };

      /**
        * Get the index of the element at the specified node,
        * and it's parent array.
        * same as above but as index + array
        * @param ac_nInputChannel the node
        * @return the index of the element that has the node,
        *         and the array to index
        * @throw StringException if not found
        */
    mt_SearchElement mf_GetElement( const unsigned ac_nInputChannel ) const
    {
      unsigned nChan = 0;
      const unsigned nItems = m_Elements.mf_nGetNumItems();
      for( unsigned i = 0 ; i < nItems; ++i )
      {
        const tIMixerElements* pCur = m_Elements.mf_GetItem( i );
        const unsigned nCurItems = pCur->mf_nGetNumItems();
        for( unsigned j = 0 ; j < nCurItems ; ++j )
        {
          IMixerElement* pCurEl = pCur->mf_GetItem( j );
          const unsigned nCurChan = pCurEl->mf_nGetNumChannels();
          for( unsigned k = 0 ; k < nCurChan ; ++k )
          {
            if( nChan == ac_nInputChannel )
              return mt_SearchElement( pCur, j );
            ++nChan;
          }
        }
      }
      throw( utils::StringException( "MixerElementPicker::mf_GetElement(): element not found" ) );
    }

      /** Add arrays.
        * Note that the order is important!
        * @param ac_pElem the array
        */
    void mp_AddElements( const IMixerElements* ac_pElem )
    {
      m_Elements.mp_AddItem( ac_pElem );
      mv_nChannels += ac_pElem->mf_nGetNumChannels();
    }

    typedef DynamicArray<const tIMixerElements*> mt_Elements;

  private:
    mt_Elements m_Elements;
    unsigned    mv_nChannels;
  };

}

#endif //#ifndef __MIXERELEMENT_H__
