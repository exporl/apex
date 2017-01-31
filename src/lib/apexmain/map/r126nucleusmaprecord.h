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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_MAP_R126NUCLEUSMAPRECORD_H_
#define _EXPORL_SRC_LIB_APEXMAIN_MAP_R126NUCLEUSMAPRECORD_H_

#include <qstring.h>

namespace r126{

    static const unsigned sc_nChannels = 22;

    //!R126 Map Record
  class R126NucleusMAPRecord
  {
  public:
    QString m_sGUID;
    QString m_sSurname;
    QString m_sFirstname;
    QString m_sStrategy;
    QString m_sMAPDate;
    QString m_sStimulationMode;

        int GetStimulationMode() const;

    bool    m_baChannelEnabled[ sc_nChannels ];
    int     m_naElectrodes[ sc_nChannels ];
    int     m_naCLevels[ sc_nChannels ];
    int     m_naTLevels[ sc_nChannels ];
    int     m_nStimulationRate;
    int     m_nTotalRate;
    int     m_nMaxima;
        int     m_nMAPNumber;
    int     m_nInterPhaseGap;
        //int     m_nPeriod;
        int     m_nMinPHaseWidth;
        int     m_nChannels;

        R126NucleusMAPRecord():
                m_nStimulationRate(-1),
                m_nTotalRate(-1),
                m_nMaxima(-1),
                m_nMAPNumber(-1),
                m_nInterPhaseGap(-1),
                m_nMinPHaseWidth(-1),
                m_nChannels(-1) {};
  };

}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_MAP_R126NUCLEUSMAPRECORD_H_
