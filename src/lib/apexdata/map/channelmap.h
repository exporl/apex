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

#ifndef _APEX_APEXDATA_MAP_CHANNELMAP_H_
#define _APEX_APEXDATA_MAP_CHANNELMAP_H_

#include "apextools/global.h"

#include <QCoreApplication>

namespace apex
{

namespace data
{

struct ChannelMapPrivate;

class APEXDATA_EXPORT ChannelMap
{
    Q_DECLARE_TR_FUNCTIONS(ChannelMap)
public:
    ChannelMap();
    ChannelMap(const ChannelMap &other);
    virtual ~ChannelMap();

    // getters

    double period() const;
    int referenceElectrode(int stim) const;
    int constantReferenceElectrode() const;
    bool hasConstantReferenceElectrode() const;
    bool hasValidReferenceElectrode(int stim) const;
    int comfortLevel() const;
    int thresholdLevel() const;
    int stimulationElectrode() const;
    int channelNumber() const;
    int mode() const;
    double phaseWidth() const;
    double phaseGap() const;
    int totalRate() const;

    // setters

    void setPeriod(double p_period);
    void setComfortLevel(int level);
    void setThresholdLevel(int level);
    void setStimulationElectrode(int electrode);
    void setChannelNumber(int nr);
    void setMode(int mode);
    void setPhaseWidth(double pw);
    void setPhaseGap(double pg);
    void setTotalRate(int rate);

    bool isValid() const;
    bool isBaseValid() const;

    const QString toXml() const;

    ChannelMap &operator=(const ChannelMap &other);
    bool operator==(const ChannelMap &other) const;

    static int modeToStimulationModeType(const QString &sMode);

private:
    ChannelMapPrivate *d;
};

} // ns data

} // ns apex

#endif
