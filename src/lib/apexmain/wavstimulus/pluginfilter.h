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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_PLUGINFILTER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_PLUGINFILTER_H_

#include "filter/pluginfilterinterface.h"
#include "pluginfilterprocessor.h"
#include "wavfilter.h"

namespace apex
{

class ParameterManager;

namespace data
{
class PluginFilterData;
}

namespace stimulus
{

/** Implementation of plugin filter.
 *  @author Tom Francart,,, <tom.francart@med.kuleuven.be>
 */
class PluginFilter : public WavFilter
{
public:
    PluginFilter(const QString &name, data::PluginFilterData *parameters,
                 unsigned long sr, unsigned bs);

    ~PluginFilter();

    //! wavdevice needs to know how to cast us
    virtual bool mf_bIsRealFilter() const
    {
        return true;
    }

    virtual bool mf_bWantsToKnowStreamLength() const
    {
        return true;
    }

    virtual void mp_SetStreamLength(const unsigned long ac_nSamples);

    virtual void Reset();

    virtual void Prepare();

    // virtual bool SetParameter (const QString &id, const QString &value);
    virtual bool SetParameter(const QString &type, const int channel,
                              const QVariant &value);

    virtual streamapp::IStreamProcessor *GetStrProc() const;

private:
    // void UpdateProcessorParameters();

    PluginFilterProcessor *m_StrProc;

    const data::PluginFilterData *const m_data;
    unsigned streamLength;
};
}
}

#endif
