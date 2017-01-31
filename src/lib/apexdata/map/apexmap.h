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

#ifndef _EXPORL_SRC_LIB_APEXDATA_MAP_APEXMAP_H_
#define _EXPORL_SRC_LIB_APEXDATA_MAP_APEXMAP_H_


#include "channelmap.h"
#include "apextools/global.h"

#include <QCoreApplication> //tr
#include <QMap>

namespace apex
{
namespace data
{

struct ApexMapPrivate;

/**
 * Contains all mapping information for Nucleus or L34
 *
 * @author Tom Francart,,,
 */
class APEXDATA_EXPORT ApexMap : public QMap<int, ChannelMap>
        //FIXME [job refactory] find a better name...
        // [Tom] L34Map
{
        Q_DECLARE_TR_FUNCTIONS(ApexMap)

    public:

        ApexMap();
        ApexMap(const ApexMap& other);
        ~ApexMap();

        int numberOfElectrodes() const;
        void setNumberOfElectrodes(int nb);

        ChannelMap defaultMap() const;
        void setDefaultMap(const ChannelMap& map);

        const QString toXml() const;

        //! Return true if a T and C level and electrode is present for channels
        //! 1-22
        bool isComplete() const;

        ApexMap& operator=(const ApexMap& other);
        bool operator==(const ApexMap& other);

    private:

        ApexMapPrivate* d;
};

}//ns data
}//ns apex

#endif
