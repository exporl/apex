/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

//  1-channel Speech - noise detection from S. Doclo and E. De Clippel

#include "vad.h"

#include <QVector>

#include <cmath>

namespace syl
{

class VoiceActivityDetectorPrivate : public QSharedData
{
public:
    double sumSquared (const double *data) const;
    void process (const double *data);

    bool synchronous;

    unsigned length;
    bool weirdRMSLimiting;

    unsigned position;
    QVector<double> delay;

    double smoothTop;
    double smoothBottomUp;
    double smoothBottomDown;
    double smoothSigma;
    double speechTresholdMult;
    double speechTresholdAdd;
    double noiseTresholdMult;
    double noiseTresholdAdd;
    double warningUpMult;
    double warningUpAdd;
    double warningDownMult;
    double warningDownAdd;
    double top;
    double bottom;
    double sigma;
    double sigmaNoise;
    double prevResult;
};

double VoiceActivityDetectorPrivate::sumSquared (const double *data) const
{
    double sumSquared = 0;
    for (unsigned i = 0; i < length; ++i)
        sumSquared += data[i] * data[i];
    if (!weirdRMSLimiting)
        return log10 (qMax (sumSquared, 0.01));

    if (sumSquared <= 0)
        return -2;
    return log10 (sumSquared);
}

void VoiceActivityDetectorPrivate::process (const double *data)
{
    const double rms = sumSquared (data);

    if (position == length) {
        top = rms - 0.5;
        bottom = rms + 0.5;
        sigma = 0.0;
        sigmaNoise = 0.4;
        prevResult = 0;
        return;
    }

    top = top < rms ? rms : (1 - smoothTop) * rms + smoothTop * top;
    const double s = bottom >= rms ? smoothBottomDown : smoothBottomUp;
    bottom = (1 - s) * rms + s * bottom;
    sigma = top - bottom;

    double newResult;
    if (((prevResult == 1) &&
         (sigma < warningDownMult * sigmaNoise + warningDownAdd)) ||
        ((prevResult == 0) &&
         (sigma > warningUpMult * sigmaNoise + warningUpAdd)))
        newResult = 0.5;
    else if (sigma >= speechTresholdMult * sigmaNoise + speechTresholdAdd)
        newResult = 1;
    else if (sigma <= (noiseTresholdMult * sigmaNoise + noiseTresholdAdd))
        newResult = 0;
    else
        newResult = prevResult;

    if (newResult == 0)
       sigmaNoise = (1 - smoothSigma) * sigma + smoothSigma * sigmaNoise;
    prevResult = newResult;
}

// VoiceActivityDetector =======================================================

VoiceActivityDetector::VoiceActivityDetector (unsigned length,
        bool synchronous) :
    d (new VoiceActivityDetectorPrivate)
{
    d->synchronous = synchronous;

    d->length = length;
    d->weirdRMSLimiting = false;
    d->delay.resize (length);
    d->position = 0;
    d->prevResult = 0;

    d->smoothTop = 0.97;
    d->smoothBottomUp = 0.97;
    d->smoothBottomDown = 0.5;
    d->smoothSigma = 0.98;
    d->speechTresholdMult = 1.4;
    d->speechTresholdAdd = 0.05;
    d->noiseTresholdMult = 1.0;
    d->noiseTresholdAdd = 0.1;
    d->warningUpMult = 1.7;
    d->warningUpAdd = 0.05;
    d->warningDownMult = 1;
    d->warningDownAdd = 0.0;
}

VoiceActivityDetector::~VoiceActivityDetector()
{
}

VoiceActivityDetector::VoiceActivityDetector
        (const VoiceActivityDetector &other) :
    d (other.d)
{
}

VoiceActivityDetector &VoiceActivityDetector::operator=
        (const VoiceActivityDetector &other)
{
    d = other.d;
    return *this;
}


void VoiceActivityDetector::process (double *data, unsigned dataLength)
{
    if (!dataLength)
        return;

    unsigned &position = d->position;
    double * const delay = d->delay.data();
    const unsigned length = d->length;
    const bool synchronous = d->synchronous;

    Q_ASSERT (!synchronous || dataLength % length == 0);

    unsigned pos = 0;
    while (dataLength) {
        const unsigned fillNeeded = length - (position % length);
        const unsigned fillAvailable = qMin (dataLength, fillNeeded);

        memcpy (delay + length - fillNeeded, data + pos,
                fillAvailable * sizeof (double));
        if (!synchronous)
            qFill (data + pos, data + pos + fillAvailable, d->prevResult);
        dataLength -= fillAvailable;
        pos += fillAvailable;
        position += fillAvailable;

        if (position % length == 0)
            d->process (delay);

        if (synchronous)
            qFill (data + pos - length, data + pos, d->prevResult);
    }
}

unsigned VoiceActivityDetector::length() const
{
    return d->length;
}

unsigned VoiceActivityDetector::position() const
{
    return d->position;
}

void VoiceActivityDetector::setWeirdRMSLimiting (bool value)
{
    d->weirdRMSLimiting = value;
}

bool VoiceActivityDetector::weirdRMSLimiting() const
{
    return d->weirdRMSLimiting;
}

} // namespace syl
