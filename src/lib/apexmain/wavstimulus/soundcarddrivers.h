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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_SOUNDCARDDRIVERS_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_SOUNDCARDDRIVERS_H_

#include "streamapp/containers/ownedarray.h"

#include "streamapp/factorytypes.h"

#include <QString>

#include <QtGlobal>

#include <map>

using namespace streamapp;

namespace apex
{
namespace stimulus
{

/**
  * Get a string for the gt_eDeviceType enum.
  * Do not change this; it should match the schema!!
  * @param ac_eType device type
  * @return device string
  */
template <class tStringType>
tStringType fSndEnumToString(const gt_eDeviceType ac_eType)
{
    if (ac_eType == PORTAUDIO)
        return "portaudio";
    else if (ac_eType == ASIO)
        return "asio";
    else if (ac_eType == JACK)
        return "jack";
    else if (ac_eType == COREAUDIO)
        return "coreaudio";
    else if (ac_eType == QTAUDIO)
        return "qtaudio";
    else if (ac_eType == DUMMY)
        return "dummy";
    else {
        Q_ASSERT(0 && "this devtype does not exist");
        return "";
    }
}

/**
  * Get a gt_eDeviceType for a given string
  * @param ac_sType device ctring
  * @return a gt_eDeviceType
  */
template <class tStringType>
gt_eDeviceType fSndStringToEnum(const tStringType ac_sType)
{
    if (ac_sType == "portaudio")
        return PORTAUDIO;
    else if (ac_sType == "asio")
        return ASIO;
    else if (ac_sType == "jack")
        return JACK;
    else if (ac_sType == "coreaudio")
        return COREAUDIO;
    else if (ac_sType == "qtaudio")
        return QTAUDIO;
    else if (ac_sType == "dummy")
        return DUMMY;
    else {
        Q_ASSERT(0 && "this devtype does not exist");
        return NODEV;
    }
}

/**
  * t_SndDriverInfo
  *   contains driver name + type (asio/portaudio)
  ************************************************ */
struct t_SndDriverInfo {
    t_SndDriverInfo(const gt_eDeviceType ac_eType, const QString ac_sDriverName)
        : m_eDevType(ac_eType),
          m_sDevType(fSndEnumToString<QString>(ac_eType)),
          m_sDriverName(ac_sDriverName)
    {
    }
    const gt_eDeviceType m_eDevType;
    const QString m_sDevType;
    const QString m_sDriverName;
};

/**
  * Contains all names per card
  */
typedef OwnedArray<t_SndDriverInfo> t_SndDrivers;

/**
  *  Contains all card definitions
  */
typedef std::map<QString, t_SndDrivers *> SndDriversMapBase;

/**
  * SndDriversMap
  *   @see SndDriversMapBase
  *   Deletes items in destructor.
  ******************************** */
class SndDriversMap : public SndDriversMapBase
{
public:
    /**
      * Constructor.
      */
    SndDriversMap()
    {
    }

    /**
      * Destructor.
      * deletes all t_SndDrivers
      */
    ~SndDriversMap()
    {
        SndDriversMapBase::iterator sit = SndDriversMapBase::begin();
        while (sit != SndDriversMapBase::end()) {
            delete sit->second;
            sit++;
        }
        SndDriversMapBase::clear();
    }
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_SOUNDCARDDRIVERS_H_
