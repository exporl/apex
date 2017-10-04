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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_FILTER_WAVGENERATOR_H_
#define _EXPORL_SRC_LIB_APEXMAIN_FILTER_WAVGENERATOR_H_

#include "wavstimulus/wavfilter.h"

namespace streamapp
{
class IStreamProcessor;
class PositionableInputStream;
}

namespace apex
{
namespace data
{
class FilterData;
}

namespace stimulus
{

class StreamGenerator;
class DataBlock;

/**
        * WavGenerator
        *   WavFilter implementation for generators.
        *   Actual implementation is a stream,
        *   but parsing is done as filter.
        ******************************************** */
class WavGenerator : public WavFilter
{
public:
    /**
      * Constructor.
      */
    WavGenerator(const QString &ac_sID, const QString &ac_sType,
                 data::FilterData *const a_cpParams, unsigned long sr,
                 unsigned bs, bool deterministic = false);

    /**
      * Destructor.
      */
    virtual ~WavGenerator();

    /**
      * Implementation of the WavFilter method.
      */
    // virtual bool        SetParameter( const QString& ac_ParamID, const
    // QString& ac_Val );
    virtual bool SetParameter(const QString &type, const int channel,
                              const QVariant &value);

    /**
      * Implementation of the WavFilter method.
      */
    virtual StreamGenerator *GetStreamGen() const;

    /**
      * Implementation of the WavFilter method.
      * Always returns false;
      */
    bool mf_bIsRealFilter() const
    {
        return false;
    }

    virtual void Reset();

    /**
   * Prepare filter for processing
   * throw exception if problem
     */
    virtual void Prepare();

    //        void SetSource(stimulus::DataBlock* s) { m_src=s;};
    //        stimulus::DataBlock* GetSource() const { Q_CHECK_PTR(m_src);
    //        return m_src; };

private:
    StreamGenerator *m_pStrGen;
    QString m_type;
};

} // ns stimulus
} // ns apex

#endif
