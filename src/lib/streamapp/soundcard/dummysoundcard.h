/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef DUMMYSOUNDCARD_H
#define DUMMYSOUNDCARD_H

#include "../typedefs.h"

#include "soundcard.h"

#include <QTimer>

class QIODevice;

namespace streamapp
{

class DummySoundcard : public QObject, public ISoundCard
{
        Q_OBJECT

    public:

        DummySoundcard(const std::string& driverName = sc_sDefault.toStdString());
        ~DummySoundcard();

        tSoundCardInfo  mf_GetInfo() const;

        bool mp_bOpenDriver(const unsigned nInChannels,
                            const unsigned nOutChannels,
                            const unsigned long sampleRate,
                            const unsigned bufferSize);

        bool mp_bCloseDriver();
        unsigned mf_nGetIChan() const;
        unsigned mf_nGetOChan() const;
        unsigned mf_nGetBufferSize() const;
        unsigned long mf_lGetSampleRate () const;
        bool mf_bIsOpen() const;
        unsigned long mf_lGetEstimatedLatency() const;
        bool mp_bStart (Callback& callback);
        bool mp_bStop();
        bool mf_bIsRunning() const;
        AudioFormatReader* mf_pCreateReader() const;
        AudioFormatWriter* mf_pCreateWriter() const;
        const std::string& mf_sGetLastError() const;
        void mp_ClearIOBuffers();

    private:

        enum State
        {
            Closed,
            Opened,
            Running
        };

        State state;
        unsigned nInChannels;
        unsigned nOutChannels;
        unsigned long sampleRate;
        unsigned bufferSize;
        std::string lastError;
        Callback* callback;
        QTimer timer;

    private Q_SLOTS:

        void doCallback();
};

}

#endif //QTWRAPPER_H
