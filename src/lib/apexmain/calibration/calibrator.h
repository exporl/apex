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

#ifndef _APEX_SRC_CALIBRATION_CALIBRATOR_H_
#define _APEX_SRC_CALIBRATION_CALIBRATOR_H_

#include "apexmodule.h"

//from libtools
#include "global.h"

#include <memory>

namespace apex
{

namespace data
{

class CalibrationData;

} // namespace data

class CalibratorPrivate;

/**
 * Apex calibrator module, to replace the old calibrator module
 */
class APEX_EXPORT Calibrator : public ApexModule
{
public:
    Calibrator (ExperimentRunDelegate *runDelegate,
            const data::CalibrationData &data);
    ~Calibrator();

    /* Tries to load the calibration data from the calibration database. If not
     * found or recalibration is forced, shows the calibration dialog. */
    bool calibrate (bool forceRecalibration = false);
	
    /* Send values of calibrated parameters for current profile to the
     * parameter manager. Must only be used after a call to calibrate() that
     * returned successfully. */
    void updateParameters();

    virtual QString GetEndXML() const;
    virtual QString GetResultXML() const;

private:
    const std::auto_ptr<CalibratorPrivate> d;
	bool calibrateManually(bool forceRecalibration = false);
};

} // namespace apex

#endif
