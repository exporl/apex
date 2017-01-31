#ifndef SPIN_CALIBRATOR_H
#define SPIN_CALIBRATOR_H

#include "spinglobal.h"

class QWidget;

namespace spin
{

class SpinCalibratorPrivate;

namespace data
{
class SpinConfig;
class SpinUserSettings;
}

//FIXME this is only exported for the tests
class SPIN_EXPORT SpinCalibrator
{
    public:

        SpinCalibrator(const data::SpinConfig& config,
                       const data::SpinUserSettings& settings,
                       QWidget* parent);
        ~SpinCalibrator();

        bool calibrate(bool* changesMade = 0);

        /**
         * Calculates the differences between the old and the new calibration
         * parameters and propagates those changes to all spin-related
         * parameters.
         */
        void propagateChanges();

    private:

        SpinCalibratorPrivate* const priv;
};

}//ns spin

#endif
