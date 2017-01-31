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

#include "apextools/exceptions.h"

#include "filter/pluginfilterinterface.h"

#include "services/mainconfigfileparser.h"
#include "services/pluginloader.h"

#include "pluginfilterprocessor.h"

#include <QPluginLoader>

using namespace streamapp;

namespace apex {

namespace stimulus {

    // PluginFilterProcessor =======================================================

    /**
     *
     * @param channels  number of channels
     * @param size      blocksize (frames)
     * @param fs        sample rate (samples/s)
     */
    PluginFilterProcessor::PluginFilterProcessor (unsigned channels,
            unsigned size,
            unsigned fs,
            const QString &name) :
        StreamProcessor (channels, size)
    {
        qCDebug(APEX_RS, "Looking for %s plugin", qPrintable (name));


        PluginFilterCreator* creator = PluginLoader::Get().
            createPluginCreator<PluginFilterCreator>(name);
        m_plugin.reset (creator->createFilter (name, channels, size, fs));
        return;
    }

    PluginFilterProcessor::~PluginFilterProcessor()
    {
    }

    void PluginFilterProcessor::resetParameters()
    {
        const appcore::Lock locker (section);

        m_plugin->resetParameters();
    }

    bool PluginFilterProcessor::setParameter (const QString &type, int channel,
            const QString &value)
    {
        const appcore::Lock locker (section);

        if (!m_plugin->setParameter (type, channel, value))
            throw ApexStringException (m_plugin->errorMessage());

        return true;
    }

    bool PluginFilterProcessor::isValidParameter (const QString &type,
            int channel) const
    {
        const appcore::Lock locker (section);

        return m_plugin->isValidParameter (type, channel);
    }


    //void PluginFilterProcessor::Reset() {
    //    const appcore::Lock locker (section);
    //    m_plugin->resetParameters ();
    //}

    void PluginFilterProcessor::Prepare (unsigned numberOfFrames) {
        const appcore::Lock locker (section);
        if (!m_plugin->prepare (numberOfFrames))
            throw ApexStringException (m_plugin->errorMessage());
    }

    const Stream &PluginFilterProcessor::mf_DoProcessing (const Stream& ac_Output)
    {
        const appcore::Lock locker (section);

        /* const unsigned channels = ac_Output.mf_nGetChannelCount();
           const unsigned frames = ac_Output.mf_nGetBufferSize();*/
        double** const data = ac_Output.mf_pGetArray();

        m_plugin->process (data);

        return ac_Output;
    }

}

}
