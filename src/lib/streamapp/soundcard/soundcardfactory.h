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

#ifndef __SOUNDCARDWRAPPERFACTORY_H_
#define __SOUNDCARDWRAPPERFACTORY_H_

#include "defines.h"
#include "typedefs.h"
#include "factorytypes.h"
#include "utils/stringexception.h"

#if defined( S_WIN32 ) //|| defined( S_MAC )
#include "win32_asiowrapper.h"
#endif
#if defined( S_MAC )
#include "osx_coreaudiowrapper.h"
#endif
#if defined( S_POSIX )
#include "linux_jackwrapper.h"
#endif
#if defined( S_TEST_JACK )
#include "_archs/linux/linux_jackwrapper.h"
#endif
#include "soundcard/portaudiowrapper.h"
#include "soundcard/soundcard.h"
#include "soundcard/qtaudiowrapper.h"
#include "soundcard/dummysoundcard.h"

#include <QDebug>

namespace streamapp
{

/**
 * SoundCardFactory
 *   class for creating the different soundcards.
 *   Doesn't delete created items.
 ************************************************ */
class SoundCardFactory
{
private:
    SoundCardFactory()
    {
    }

public:
    ~SoundCardFactory()
    {
    }

    /**
     * Create an ISoundCard.
     * @param ac_sDriverName the driver name
     * @param ac_eType the type
     * @param a_sError receives soundcard error info, if any
     * @return an instance or 0 if it couldn't be created
     */
    static ISoundCard *CreateSoundCard(const std::string& ac_sDriverName, const
            gt_eDeviceType ac_eType, std::string& a_sError )
    {
        qDebug( "CreateSoundCard" );
        try {
            if (ac_eType == PORTAUDIO) {
                return new PortAudioWrapper(ac_sDriverName);
            } else if (ac_eType == DUMMY) {
                    return new DummySoundcard(ac_sDriverName);
#if defined(S_WIN32)
            } else if (ac_eType == ASIO) {
                if (ac_sDriverName != sc_sDefault.toStdString())
                    return new AsioWrapper(ac_sDriverName);
                Q_FOREACH (const std::string &driver,
                        AsioWrapper::sf_saGetDriverNames(a_sError)) {
                    try {
                        return new AsioWrapper(driver);
                    } catch (...) {
                        // ignored
                    }
                }
#endif
#if defined(S_MAC)
            } else if (ac_eType == COREAUDIO) {
                return new CoreAudioWrapper(ac_sDriverName);
#endif
#if defined(S_POSIX) || defined(S_TEST_JACK)
            } else if (ac_eType == JACK) {
                if (ac_sDriverName != sc_sDefault.toStdString())
                    return new JackWrapper(ac_sDriverName);
                tStringVector drvrs(JackWrapper::sf_saGetDriverNames(a_sError));
                if (drvrs.size())
                    return new JackWrapper(drvrs.at(0));
#endif
            } else {
                a_sError = "SoundcardFactory: unknown driver";
            }
        }
        catch (const utils::StringException& e)
        {
            a_sError = e.mc_sError;
        } catch (...) {
            if (a_sError.empty())
                a_sError = "SoundCardFactory: caught unhandled exception";
        }
        return 0;
    }

    /**
        * Get a list of all available drivers for a type.
        * @param ac_eType one of mt_eType
        * @param a_sError receives soundcard error info, if any
        * @return a vector of strings
        */
    static tStringVector GetDriverNames(gt_eDeviceType ac_eType, std::string &a_sError)
    {
        if (ac_eType == PORTAUDIO)
            return PortAudioWrapper::sf_saGetDriverNames(a_sError);
#if defined(S_WIN32)
        if (ac_eType == ASIO)
            return AsioWrapper::sf_saGetDriverNames(a_sError);
#endif
#if defined(S_MAC)
        if (ac_eType == COREAUDIO)
            return CoreAudioWrapper::sf_saGetDriverNames(a_sError);
#endif
#if defined(S_POSIX) || defined(S_TEST_JACK)
        if (ac_eType == JACK)
            return JackWrapper::sf_saGetDriverNames(a_sError);
#endif
        a_sError = "SoundcardFactory: unknown driver";
        return tStringVector();
    }
};

}

#endif //#ifndef __SOUNDCARDWRAPPERFACTORY_H_
