/******************************************************************************
 * Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef APEXDATA_TEST_H
#define APEXDATA_TEST_H

#include "../testbench.h"

namespace apex
{
namespace data
{
class DatablockData;
class ConnectionData;
class CorrectorData;
}
}

class ApexDataTest : public ApexTest
{
        Q_OBJECT
        Q_INTERFACES(ApexTest)

    public:

        QString name() const;

    private Q_SLOTS:

        void testSimpleParameters();

        void testDatablockData();
        void testCalibrationData();
        void testCalibrationParameterData();
        void testConnectionData();
        void testCorrectorData();
        void testApexMap();
        void testChannelMap();
        void testApexAnswer();

        //tests for SoundLevelMeterData
        void testSoundLevelMeterDataCopy();
        void testSoundLevelMeterDataSupported();
        void testSoundLevelMeterDataSupported_data();
        void testSoundLevelMeterDataUnsupported();
        void testSoundLevelMeterDataUnsupported_data();

    private:

        void initDatablockData(apex::data::DatablockData* data);
        void initConnectionData(apex::data::ConnectionData* data);
        void initCorrectorData(apex::data::CorrectorData* data);
};

#endif
