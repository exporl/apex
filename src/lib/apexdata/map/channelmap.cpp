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

// ChannelMap.cpp: implementation of the CChannelMap class.
// From Apex 2
//////////////////////////////////////////////////////////////////////

#include "apextools/exceptions.h"

#include "stimulus/nucleus/nicstream/stimulation_mode.h"

#include "channelmap.h"

namespace apex
{

namespace data
{

struct ChannelMapPrivate
{
    ChannelMapPrivate();

    //TODO give better names
    int e;         /// Stimulation electrode
    int channelNr;
    int mode;      /// Reference electrode
    int t;         /// Threshold Level
    int c;         /// Comfort Level
    double period; /// Stimulation Period, in microseconds
    int totalRate; /// Stimulation rate (Hz)
    double ipg;       /// Phase Gap     (microseconds)
    double minPw ;    /// Phase Width
};

ChannelMapPrivate::ChannelMapPrivate() : e(-1),
                                         channelNr(-1),
                                         mode(-1),
                                         t(-1),
                                         c(-1),
                                         period(-1),
                                         totalRate(-1),
                                         ipg(-1),
                                         minPw(-1)
{
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ChannelMap::ChannelMap() : d(new ChannelMapPrivate())
{
}

ChannelMap::ChannelMap(const ChannelMap& other) : d(new ChannelMapPrivate(*other.d))
{
}

ChannelMap::~ChannelMap()
{
    delete d;
}



/**
 * Return mode number for a certain stimulation mode, from apex 2
 * @param sMode
 * @return
 */
int ChannelMap::modeToStimulationModeType(QString sMode) {
        /*
        CG  Common ground stimulation.
        BP  Bi-polar stimulation.
        BP1  Bi-polar + 1 stimulation.
        BP2  Bi-polar + 2 stimulation.
        BP3  Bi-polar + 3 stimulation.
        BP4  Bi-polar + 4 stimulation.
        BP5  Bi-polar + 5 stimulation.
        BP6  Bi-polar + 6 stimulation.
        BP7  Bi-polar + 7 stimulation.
        BP8  Bi-polar + 8 stimulation.
        BP9  Bi-polar + 9 stimulation.
        BP10  Bi-polar + 10 stimulation.
        RBP  Reverse bi-polar stimulation.
        RBP1  Reverse bi-polar + 1 stimulation.
        RBP2  Reverse bi-polar + 2 stimulation.
        RBP3  Reverse bi-polar + 3 stimulation.
        RBP4  Reverse bi-polar + 4 stimulation.
        RBP5  Reverse bi-polar + 5 stimulation.
        RBP6  Reverse bi-polar + 6 stimulation.
        RBP7  Reverse bi-polar + 7 stimulation.
        RBP8  Reverse bi-polar + 8 stimulation.
        RBP9  Reverse bi-polar + 9 stimulation.
        RBP10  Reverse bi-polar + 10 stimulation.
        MP1  Mono-polar 1 stimulation
        MP2  Mono-polar 2 stimulation
        MP1_2  Mono-polar 1+2 stimulation
        RMP1  Reverse mono-polar 1 stimulation
        RMP2  Reverse mono-polar 2 stimulation
        RMP1_2  Reverse mono-polar 1+2 stimulation
        */
        if (sMode=="CG") {
                return(CG);
        } else if (sMode=="BP") {
                return(BP);
        } else if (sMode=="BP+1") {
                return(BP1);
        } else if (sMode=="BP+2") {
                return(BP2);
        } else if (sMode=="BP+3") {
                return(BP3);
        } else if (sMode=="BP+4") {
                return(BP4);
        } else if (sMode=="BP+5") {
                return(BP5);
        } else if (sMode=="BP+6") {
                return(BP6);
        } else if (sMode=="BP+7") {
                return(BP7);
        } else if (sMode=="BP+8") {
                return(BP8);
        } else if (sMode=="BP+9") {
                return(BP9);
        } else if (sMode=="BP+10") {
                return(BP10);
        } else if (sMode=="RBP") {
                return(RBP);
        } else if (sMode=="RBP+1") {
                return(RBP1);
        } else if (sMode=="RBP+2") {
                return(RBP2);
        } else if (sMode=="RBP+3") {
                return(RBP3);
        } else if (sMode=="RBP+4") {
                return(RBP4);
        } else if (sMode=="RBP+5") {
                return(RBP5);
        } else if (sMode=="RBP+6") {
                return(RBP6);
        } else if (sMode=="RBP+7") {
                return(RBP7);
        } else if (sMode=="RBP+8") {
                return(RBP8);
        } else if (sMode=="RBP+9") {
                return(RBP9);
        } else if (sMode=="RBP+10") {
                return(RBP10);
        } else if (sMode=="MP1") {
                return(MP1);
        } else if (sMode=="MP2") {
                return(MP2);
        } else if (sMode=="MP1+2") {
                return(MP1_2);
        } else if (sMode=="MP1+2") {
                return(MP1_2);
        } else if (sMode=="RMP1") {
                return(RMP1);
        } else if (sMode=="RMP2") {
                return(RMP2);
        } else if (sMode=="RMP1+2") {
                return(RMP1_2);
        } else {
                //Unkown type (error)
                qCDebug(APEX_RS, "%s", qPrintable("Invalid mode " + sMode));
                throw ApexStringException("Invalid mode " + sMode);
//              return(-1);
        }
}



int ChannelMap::referenceElectrode() const {
          if (d->mode==MP1)
                return -1;
        if (d->mode==MP2)
                return -2;
        if (d->mode==MP1_2)
                return -3;
        else {
                throw ApexStringException("Stimulation mode not supported");
        }
}


bool ChannelMap::isValid() const
{
    return  isBaseValid() &&
            d->e != -1 &&
            d->c != -1 &&
            d->t != -1 &&
            d->channelNr != -1;
}


bool ChannelMap::isBaseValid() const
{
    return  d->minPw != -1 &&
            (d->totalRate != -1 || d->period != -1)
            && d->ipg != -1
            && d->mode != -1;

}

const QString ChannelMap::toXml() const
{
        QString result;

        result+= QString("\t\t<phasewidth>%1</phasewidth>\n").arg(d->minPw);
        result+= QString("\t\t<period>%1</period>\n").arg(period(),0,'f', 2);
        result+= QString("\t\t<phasegap>%1</phasegap>\n").arg(d->ipg);
        result+= QString("\t\t<mode>%1</mode>\n").arg(d->mode);
        result+= QString("\t\t<e>%1</e>\n").arg(d->e);
        result+= QString("\t\t<c>%1</c>\n").arg(d->c);
        result+= QString("\t\t<t>%1</t>\n").arg(d->t);
        result+= QString("\t\t<channel>%1</channel>\n").arg(d->channelNr);

        return result;
}

/*
*       Returns the period in microseconds
*/
double  ChannelMap::period() const
{
    if (d->period != -1)
        return d->period;

    Q_ASSERT(d->totalRate != -1);
    return 1e6/(double)d->totalRate;
}

void ChannelMap::setPeriod(double p_period)
{
    d->period = p_period;
}

int ChannelMap::comfortLevel() const
{
    return d->c;
}

void ChannelMap::setComfortLevel(int level)
{
    d->c = level;
}

int ChannelMap::thresholdLevel() const
{
    return d->t;
}

void ChannelMap::setThresholdLevel(int level)
{
    d->t = level;
}

int ChannelMap::stimulationElectrode() const
{
    return d->e;
}

void ChannelMap::setStimulationElectrode(int electrode)
{
    d->e = electrode;
}

int ChannelMap::channelNumber() const
{
    return d->channelNr;
}

void ChannelMap::setChannelNumber(int nr)
{
    d->channelNr = nr;
}

int ChannelMap::mode() const
{
    return d->mode;
}

void ChannelMap::setMode(int mode)
{
    d->mode = mode;
}

double ChannelMap::phaseWidth() const
{
    return d->minPw;
}

void ChannelMap::setPhaseWidth(double pw)
{
    d->minPw = pw;
}

double ChannelMap::phaseGap() const
{
    return d->ipg;
}

void ChannelMap::setPhaseGap(double pg)
{
    d->ipg = pg;
}

int ChannelMap::totalRate() const
{
    return d->totalRate;
}

void ChannelMap::setTotalRate(int rate)
{
    d->totalRate = rate;
}

ChannelMap& ChannelMap::operator=(const ChannelMap& other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}

bool ChannelMap::operator==(const ChannelMap& other) const
{
    return  d->c == other.d->c &&
            d->channelNr == other.d->channelNr &&
            d->e == other.d->e &&
            d->ipg == other.d->ipg &&
            d->minPw == other.d->minPw &&
            d->mode == other.d->mode &&
            d->t == other.d->t &&
            d->totalRate == other.d->totalRate &&
            d->period == other.d->totalRate;
}


}

}



















