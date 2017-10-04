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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_PLUGINFILTERPROCESSOR_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_PLUGINFILTERPROCESSOR_H_

#include "streamapp/appcore/threads/locks.h"

#include "streamapp/audiosamplebuffer.h"

#include "streamapp/containers/matrix.h"

#include "streamapp/stream.h"

#include "streamapp/utils/checks.h"
#include "streamapp/utils/dataconversion.h"
#include "streamapp/utils/stringexception.h"

#include <memory>

using namespace streamapp;

class PluginFilterInterface;

namespace apex
{

namespace stimulus
{

class PluginFilterProcessor : public StreamProcessor
{
public:
    PluginFilterProcessor(unsigned channels, unsigned size, unsigned fs,
                          const QString &name);
    virtual ~PluginFilterProcessor();

    void Prepare(unsigned numberOfFrames);
    virtual const Stream &mf_DoProcessing(const Stream &ac_Output);

    bool setParameter(const QString &type, int channel, const QString &value);
    bool isValidParameter(const QString &type, int channel) const;
    void resetParameters();

private:
    const appcore::CriticalSection section;
    QScopedPointer<PluginFilterInterface> m_plugin;
};
}
}

#endif // ProcsH
