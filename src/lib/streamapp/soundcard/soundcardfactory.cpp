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

#include "soundcardfactory.h"

#if defined(S_WIN32) //|| defined( S_MAC )
#include "../_archs/win32/win32_asiowrapper.h"
#endif
#if defined(S_MAC)
#include "../_archs/osx/osx_coreaudiowrapper.h"
#endif
#if defined(ENABLEJACK)
#include "../_archs/linux/linux_jackwrapper.h"
#endif

Q_LOGGING_CATEGORY(APEX_SA, "apex.streamapp")

namespace streamapp
{

SoundCardFactory::SoundCardFactory()
{
}

SoundCardFactory::~SoundCardFactory()
{
}

ISoundCard *SoundCardFactory::CreateSoundCard(const QString &cardName,
                                              const gt_eDeviceType ac_eType,
                                              QString &a_sError)
{
    qCDebug(APEX_SA, "CreateSoundCard");
    try {
        if (ac_eType == PORTAUDIO) {
            return new PortAudioWrapper(cardName);
        } else if (ac_eType == DUMMY) {
            return new DummySoundcard(cardName);
#if defined(S_WIN32)
        } else if (ac_eType == ASIO) {
            if (cardName != sc_sDefault) {
                std::string card = cardName.toStdString();
                return new AsioWrapper(card);
            }
            std::string error = a_sError.toStdString();
            Q_FOREACH (const std::string &driver,
                       AsioWrapper::sf_saGetDriverNames(error)) {
                try {
                    return new AsioWrapper(driver);
                } catch (...) {
                    // ignored
                }
            }
#endif
#if defined(S_MAC)
        } else if (ac_eType == COREAUDIO) {
            return new CoreAudioWrapper(cardName);
#endif
#if defined(ENABLEJACK)
        } else if (ac_eType == JACK) {
            if (cardName != sc_sDefault)
                return new JackWrapper(cardName.toStdString());
            std::string error = a_sError.toStdString();
            tStringVector drvrs(JackWrapper::sf_saGetDriverNames(error));
            if (drvrs.size())
                return new JackWrapper(drvrs.at(0));
#endif
        } else {
            a_sError = "SoundcardFactory: unknown driver";
        }
    } catch (const utils::StringException &e) {
        a_sError = QString::fromStdString(e.mc_sError);
    } catch (...) {
        if (a_sError.isEmpty())
            a_sError = "SoundCardFactory: caught unhandled exception";
    }
    return 0;
}

tStringVector SoundCardFactory::GetDriverNames(gt_eDeviceType ac_eType,
                                               QString &a_sError)
{
    if (ac_eType == PORTAUDIO)
        return PortAudioWrapper::sf_saGetDriverNames(a_sError);
#if defined(S_WIN32)
    if (ac_eType == ASIO) {
        std::string error = a_sError.toStdString();
        return AsioWrapper::sf_saGetDriverNames(error);
    }
#endif
#if defined(S_MAC)
    if (ac_eType == COREAUDIO) {
        std::string error = a_sError.toStdString();
        return CoreAudioWrapper::sf_saGetDriverNames(error);
    }
#endif
#if defined(ENABLEJACK)
    if (ac_eType == JACK) {
        std::string error = a_sError.toStdString();
        return JackWrapper::sf_saGetDriverNames(error);
    }
#endif
    a_sError = "SoundcardFactory: unknown driver";
    return tStringVector();
}
}
