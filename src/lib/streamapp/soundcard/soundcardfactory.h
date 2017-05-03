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

#include "../defines.h"
#include "../factorytypes.h"
#include "../typedefs.h"

#include "../utils/stringexception.h"

#include "dummysoundcard.h"
#include "portaudiowrapper.h"
#include "soundcard.h"

#include <string>
#include <QString>
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
    SoundCardFactory();

public:
    ~SoundCardFactory();

    /**
     * Create an ISoundCard.
     * @param ac_sDriverName the driver name
     * @param ac_eType the type
     * @param a_sError receives soundcard error info, if any
     * @return an instance or 0 if it couldn't be created
     */
    static ISoundCard *CreateSoundCard(const QString& cardName, const
                                       gt_eDeviceType ac_eType, QString& a_sError );

    /**
     * Get a list of all available drivers for a type.
     * @param ac_eType one of mt_eType
     * @param a_sError receives soundcard error info, if any
     * @return a vector of strings
     */
    static tStringVector GetDriverNames(gt_eDeviceType ac_eType, QString &a_sError);
};

}

#endif //#ifndef __SOUNDCARDWRAPPERFACTORY_H_
