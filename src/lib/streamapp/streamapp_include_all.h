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
 
#ifndef STRAPPINCALL_H
#define STRAPPINCALL_H

#include "includes.h" //always put this first, it will make sure the platform is defined

#include "audioformatconvertor.h"
#include "audioformatstream.h"
#include "audiosamplebuffer.h"
#include "connections.h"
#include "eofcheck.h"
#include "memorystream.h"
#include "multiproc.h"
#include "multistream.h"
#include "silentreader.h"
#include "stream.h"
#include "streamappdefines.h"
#include "streamappfactory.h"
#include "streamutils.h"

#include "appcore/deleter.h"
#include "appcore/destroyer.h"
#include "appcore/managedsingleton.h"
#include "appcore/raiifactory.h"
#include "appcore/singleton.h"

#include "appcore/events/bufferthread.h"
#include "appcore/events/callbackevent.h"
#include "appcore/events/eventqueuethread.h"
#include "appcore/events/eventrouter.h"
#include "appcore/events/events.h"
#include "appcore/events/fakebufferthread.h"
#include "appcore/events/processevent.h"

#include "appcore/notifier/notifier.h"

#include "appcore/queue/dequeuethread.h"
#include "appcore/queue/queuebase.h"
#include "appcore/queue/queuemanager.h"

#include "appcore/threads/criticalsection.h"
#include "appcore/threads/locks.h"
#include "appcore/threads/readwritelock.h"
#include "appcore/threads/thread.h"
#include "appcore/threads/waitableobject.h"

#include "callback/callbackrunner.h"
#include "callback/eofcheckcallbacks.h"
#include "callback/streamappcallbacks.h"

#include "containers/array.h"
#include "containers/dynarray.h"
#include "containers/matrix.h"
#include "containers/matrixoperations.h"
#include "containers/ownedarray.h"
#include "containers/pingpong.h"
#include "containers/queue.h"
#include "containers/rawmemory.h"
#include "containers/rawmemorybundle.h"

#include "file/binaryfile.h"
#include "file/multiwavefile.h"
#include "file/wavefile.h"

#include "soundcard/soundcard.h"
#include "soundcard/soundcardfactory.h"

#include "processors/spectrum.h"
#include "processors/matrixmixer.h"

#include "resamplers/resampler.h"

#include "mixers/mixerfactory.h"

#include "network/tcpsocket.h"
#include "network/socketconn.h"
#include "network/socketconnfactory.h"
#include "network/socketconnserver.h"
#include "network/socketfunc.h"
#include "network/eventsocketconn.h"
#include "network/eventsocketconnlogger.h"
#include "network/eventsocketconnmgr.h"

#include "utils/dataconversion.h"
#include "utils/math.h"
#include "utils/noisy.h"
#include "utils/stringutils.h"
#include "utils/stringexception.h"
#include "utils/tracer.h"
#include "utils/tracermemory.h"
#include "utils/vectorutils.h"

#endif //#ifndef STRAPPINCALL_H
