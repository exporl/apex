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

#ifndef WAVFILTER_H
#define WAVFILTER_H

#include "stimulus/filter.h"
#include "../filtertypes.h"
#include "apextypedefs.h"

namespace streamapp
{
class Amplifier;
class IStreamProcessor;
class PositionableInputStream;
}

namespace apex
{
namespace stimulus
{

class StreamGenerator;
class DataBlock;

/**
  * WavFilter
  *   interface for wav filters and generators.
  *   @see IApexdevice for how the parameter system works.
  ******************************************************** */
class WavFilter : public Filter
{
	public:
		/**
		  * Constructor.
		  */
		WavFilter ( const QString& ac_sID,
		            data::FilterData* const ac_pParams,
		            unsigned long sr, unsigned bs ) :
				Filter ( ac_sID, ac_pParams ),
				mv_bNeedsRestore ( false ),
                sampleRate ( sr ),
				blockSize ( bs )
		{}

		/**
		  * Destructor.
		  */
		virtual ~WavFilter()
		{}

		/**
		  * Get the IStreamProcessor for this filter.
		  * mf_bIsRealFilter() must return true when using this.
		  * @return an IStreamProcessor, or 0 if it's a generator
		  */
		virtual streamapp::IStreamProcessor* GetStrProc() const
		{
			if ( !mf_bIsRealFilter() )
				assert ( 0 );
			return 0;
		}

		/**
		  * Get the StreamGenerator for this filter.
		  * mf_bIsRealFilter() must return false for this.
		  * @return a StreamGenerator, or 0 if it's an IStreamProcessor
		  */
		virtual StreamGenerator* GetStreamGen() const
		{
			if ( mf_bIsRealFilter() )
				assert ( 0 );
			return 0;
		}

		/**
		  * Check if the actual implementation is a filter
		  * or a generator. A filter has input anbd output,
		  * a generator has only output.
		  * Used when WavDevice needs to know how to cast us.
		  */
		virtual bool mf_bIsRealFilter() const = 0;

		/**
		  * @see IApexdevice
		  */
		//virtual void RestoreParametersImpl() = 0;

		/**
		  * @see IApexdevice
		  */
		void mp_NeedRestore()
		{ mv_bNeedsRestore = true; }

		/**
		  * @see IApexdevice
		  */
		bool mf_bNeedsRestore()
		{ return mv_bNeedsRestore; }


		/**
		    * Reset the filter to its initial state and set all internal parameters to built in
		    * default values
		    * throw exception if problem
		 */
		virtual void Reset()
		{
			qDebug ( "WavFilter::Reset: for %s should be overwritten", qPrintable ( GetID() ) );
		};

		/**
		* Prepare filter for processing
		* throw exception if problem
		 */
		virtual void Prepare()
		{
			// dummy, not everyone wants to prepare
		};

		/**
		  * If a filter needs to know the total length
		  * (not incorporating continuous generators)
		  * of the stimulus being played, it should return
		  * true here.
		  * @return true if filter wants to have it's
		  * @c mp_SetStreamLength called at the start of
		  * each stimulus presentation
		  */
		virtual bool mf_bWantsToKnowStreamLength() const
		{
			return false;
		}

		/**
		  * Called to inform filter of stimulus length.
		  * Used to know when to start fadeout etc.
		  * @param ac_nSamples total samples in stream
		  */
		virtual void mp_SetStreamLength ( const unsigned long ac_nSamples )
		{
			( void ) ac_nSamples;
		}

		void SetBlockSize( unsigned bs );
		unsigned GetBlockSize();
		void SetSampleRate( unsigned long sr );
		unsigned long GetSampleRate();

	private:
		bool mv_bNeedsRestore;
		unsigned long sampleRate;
		unsigned blockSize;
};

/**
  * WavAmplifier
  *   applies gain to a Stream.
  ***************************** */
class WavAmplifier : public WavFilter
{
	public:
		/**
		  * Constructor.
		  */
		WavAmplifier ( const QString& ac_sID,
		               data::FilterData* const pParams,
				 	   unsigned long sr, unsigned bs);

		/**
		  * Destructor.
		  */
		~WavAmplifier();

		/**
		  * Set a parameter.
		  * Only accepts "gain".
		  * Final gain = basegain +- ac_Val.
		  */
		//bool SetParameter( const QString& ac_ParamID, const QString& ac_Val );
		virtual bool SetParameter ( const QString& type, const int channel, const QVariant& value ) ;

		/**
		  * Implementation of the WavFilter method.
		  */
		streamapp::IStreamProcessor*  GetStrProc() const;

		/**
		  * Implementation of the WavFilter method.
		  */
		INLINE bool mf_bIsRealFilter() const
		{ return true; }

		virtual void Reset();

	private:
		streamapp::Amplifier* m_StrProc;
		tQStringUintMap m_GainChannels;
};


}
} //end ns
#endif //WAVFILTER_H
