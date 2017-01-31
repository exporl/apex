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

#include "streamapp/soundcard/soundcardfactory.h"

#include "soundcarddisplayer.h"
#include "soundcarddrivers.h"

#include <QString>

#include <iostream>

#include <portaudio.h>

using namespace streamapp;

namespace apex {
  namespace stimulus {

    void sf_ShowCardsForDriver( const gt_eDeviceType ac_tType )
    {
      std::string sErr;
      tStringVector saCards( SoundCardFactory::GetDriverNames( ac_tType, sErr ) );
      const tStringVector::size_type nCards = saCards.size();
      if( nCards )
      {
        std::cout << fSndEnumToString< std::string >( ac_tType ) << " cards: " << std::endl;
        for( tStringVector::size_type i = 0 ; i < saCards.size() ; ++i )
        {
          QString temp( saCards.at( i ).c_str() );
          std::cout << temp.toLocal8Bit().constData() << std::endl;
        }
      }
    }

    void ShowSoundCardsPortAudio() {
        if(!Pa_Initialize()) {
            PaDeviceIndex numberOfDevices = Pa_GetDeviceCount();
            for( PaDeviceIndex deviceIndex = 0 ; deviceIndex<numberOfDevices ; deviceIndex++ ) {
                const PaDeviceInfo *device = Pa_GetDeviceInfo(deviceIndex);
                const PaHostApiInfo *hostApi = Pa_GetHostApiInfo(device->hostApi);

                QString deviceName = QString::fromLocal8Bit(device->name);
                QString hostApiName = QString::fromLocal8Bit(hostApi->name);
                std::cout << "Device: " + deviceName.toStdString();
//                if ( deviceIndex == streamapp::PortAudioInitter::sf_pInstance()->mf_nGetInputDeviceID("default") )
//                    std::cout << " (default input device!)";
//                if ( deviceIndex == streamapp::PortAudioInitter::sf_pInstance()->mf_nGetOutputDeviceID("default") )
//                    std::cout << " (default output device!)";
                std::cout << std::endl;
                std::cout << "\t DeviceID=" << deviceIndex << "\t Hostapi: " + hostApiName.toStdString() << std::endl;

                std::cout << "Default portaudio device: "  << Pa_GetDefaultOutputDevice() << std::endl;


                // TODO: remove (copied from portaudiowrapper.cpp
                PaHostApiTypeId preferredHostApiTypeIds[] = {paWASAPI, paWDMKS, paDirectSound, paMME, paASIO, paALSA, paInDevelopment};

                PaHostApiIndex numberOfInstalledHostApis = Pa_GetHostApiCount();

                PaDeviceIndex preferred = paNoDevice;
                for( PaHostApiIndex preferredHostApiIndex = 0 ; preferredHostApiTypeIds[preferredHostApiIndex] != paInDevelopment ; preferredHostApiIndex++ ) {
                    for( PaHostApiIndex installedHostApiIndex = 0 ; installedHostApiIndex < numberOfInstalledHostApis ; installedHostApiIndex++ ) {
                        if( preferredHostApiTypeIds[preferredHostApiIndex] == Pa_GetHostApiInfo(installedHostApiIndex)->type )
                            preferred = Pa_GetHostApiInfo(installedHostApiIndex)->defaultInputDevice;
                    }
                }

                std::cout << "Default device for preferred HostAPI: " << preferred << std::endl;
                // END
            }
        std::cout << "--------------------------" << std::endl;
        // TODO: remove, copied from portaudiowrapper
        PaHostApiTypeId preferredHostApiTypeIds[] = {paWASAPI, paWDMKS, paDirectSound, paMME, paASIO, paALSA, paInDevelopment};
        for( PaHostApiIndex preferredHostApiIndex = 0 ; preferredHostApiTypeIds[preferredHostApiIndex] != paInDevelopment ; preferredHostApiIndex++ ) {
            PaHostApiIndex hostApiIndex = Pa_HostApiTypeIdToHostApiIndex(preferredHostApiTypeIds[preferredHostApiIndex]);
            if (hostApiIndex < 0) {
                continue;
            }
            const PaHostApiInfo *hinfo = Pa_GetHostApiInfo(hostApiIndex);
            std::cout << "HOSTAPI " << hostApiIndex << " " << hinfo->name << "'s devices: " << std::endl;
            for (PaDeviceIndex hostDevice = 0; hostDevice < hinfo->deviceCount; hostDevice++) {
                PaDeviceIndex deviceNum = Pa_HostApiDeviceIndexToDeviceIndex(hostApiIndex, hostDevice);
                const PaDeviceInfo *dinfo = Pa_GetDeviceInfo(deviceNum);
                if (dinfo) {
                    std::cout << "device " << deviceNum << " " << dinfo->name  << " belonging  to " << hinfo->name
                              << " with default sample rate " << dinfo->defaultSampleRate << " outputchannels " << dinfo->maxOutputChannels << std::endl;
                }

                PaStreamParameters out;
                out.channelCount = dinfo->maxOutputChannels;
                out.device = deviceNum;
                out.hostApiSpecificStreamInfo = 0;
                out.sampleFormat = paInt32;
                out.suggestedLatency = 0.0;
                std::vector< unsigned long > a_SampleRates;
                const unsigned nRatesToTry = 9;
                const unsigned long RatesToTry[] = { 8000, 12000, 16000, 24000, 32000, 44100, 48000, 96000, 192000 };
                for( unsigned i = 0 ; i < nRatesToTry ; ++i )
                if (Pa_IsFormatSupported( 0, &out, RatesToTry[ i ] ) == paFormatIsSupported) {
                    a_SampleRates.push_back( RatesToTry[ i ] );
                }
                if (std::find(a_SampleRates.begin(), a_SampleRates.end(), dinfo->defaultSampleRate) != a_SampleRates.end()) {
                    std::cout << "!!! PaDeviceInfo's default sample rate is not reported as supported by Pa_IsFormatSupported" << std::endl;
                    std::cout << "Supported formats: ";
                    for (std::vector< unsigned long >::const_iterator i = a_SampleRates.begin(); i != a_SampleRates.end(); ++i) {
                        std::cout << *i << "  ";
                    }
                    std::cout << std::endl;
                }
            }
            PaDeviceIndex devindex = hinfo->defaultOutputDevice;
            std::cout << std::endl << "host api default output device " << devindex << " " << Pa_GetDeviceInfo(devindex)->name << std::endl;
        }
        std::cout << "------------" << std::endl;
        std::cout << "default output device over entire system " << Pa_GetDefaultOutputDevice() << " with name "
                  << Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->name << std::endl;
        std::cout << "default host api " << Pa_GetDefaultHostApi() << " with name "
                  << Pa_GetHostApiInfo(Pa_GetDefaultHostApi())->name << std::endl;
        Pa_Terminate();
        } else {
            std::cout << "portaudio not started!" << std::endl;
        }
    }

    void SoundCardDisplayer::ShowSoundCards()
    {
        std::cout << "Audio devices in this system:" << std::endl;
        std::cout << "--------------------------" << std::endl;
        //there's no need for #ifdefs here, that's
        //done in SoundCardFacotry already
        sf_ShowCardsForDriver( ASIO );
        // sf_ShowCardsForDriver( PORTAUDIO );
        ShowSoundCardsPortAudio();
        sf_ShowCardsForDriver( JACK );
        sf_ShowCardsForDriver( COREAUDIO );
        std::cout << "--------------------------" << std::endl;
    }

  }
}
