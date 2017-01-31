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

#ifndef _EXPORL_SRC_LIB_APEXDATA_FILTER_WAVPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXDATA_FILTER_WAVPARAMETERS_H_

#include "../device/devicedata.h"

#include "filterdata.h"

namespace apex
{
namespace data
{

/**
 * @class WavParameters
 * @brief Parameters used by all WavFilters
 */
class APEXDATA_EXPORT WavFilterParameters : public FilterData
{
    public:

        WavFilterParameters();
        virtual ~WavFilterParameters();

        double gain() const;
        double baseGain() const;
        bool invertGain() const;
};


/**
 * @class WavFaderParameters
 * @brief Parameters for WavFader and WavFaderOut.
 */
class APEXDATA_EXPORT WavFaderParameters : public WavFilterParameters
{
    public:

        /**
         * Type of fade.
         */
        enum FadeType
        {
            INVALID,
            LINEAR,
            COSINE
        };

        WavFaderParameters();
        ~WavFaderParameters();

        /**
         * Get the fader type.
         * @return one of @ref FadeType
         */
        FadeType type() const;

        /**
         * Get the fade length, in mSec.
         */
        double length() const;

        /**
         * Get fade direction.
         * @return true for fadein, false for fadeout
         */
        bool isFadeIn() const;
};


/**
 * @class WavGeneratorParameters
 * @brief Parameters used by all generators.
 */
class APEXDATA_EXPORT WavGeneratorParameters : public WavFilterParameters
{
    public:

        WavGeneratorParameters();
        ~WavGeneratorParameters();

        /**
         * Check if generator should be continuous.
         * @return true if so
         */
        bool isContinuous() const;
        virtual bool isGenerator() const;

};

/**
 * @class SinglePulseGeneratorParameters
 * @brief Parameters for SinglePulseGenerator.
 */
class APEXDATA_EXPORT SinglePulseGeneratorParameters : public WavGeneratorParameters
{
    public:

        SinglePulseGeneratorParameters();
        ~SinglePulseGeneratorParameters();

        /**
         * Get the pulse width.
         * @return width in samples
         */
        unsigned pulseWidth() const;

        /**
         * Get the polarity.
         * @return true for positive
         */
        bool polarity() const;
};

/**
 * @class DataLoopGeneratorParameters
 * @brief Parameters for DataLoopGenerator.
 */
class APEXDATA_EXPORT DataLoopGeneratorParameters : public WavGeneratorParameters
{
    public:

        DataLoopGeneratorParameters();
        ~DataLoopGeneratorParameters();

        /**
         * Check if the loop should start at a random position.
         * @return true if so.
         */
        bool startsWithRandomJump() const;
        double jump() const;

};

/**
 * @class SineGeneratorParameters
 * @brief Parameters for SineGenerator.
 */
class APEXDATA_EXPORT SineGeneratorParameters : public WavGeneratorParameters
{
    public:

        SineGeneratorParameters();
        ~SineGeneratorParameters();

        /**
         * Get the sine's frequency.
         * @return frequency in Hz.
         */
        double frequency() const;

        /**
         * Get the sine's phase.
         * @return phase in milliseconds.
         */
        double phase() const;
};


}
}

#endif
