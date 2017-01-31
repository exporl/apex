#include "apexdata/experimentdata.h"

#include "apexmain/calibration/calibrationdatabase.h"
#include "apexmain/calibration/calibrator.h"

#include "apexmain/experimentparser.h"

#include "apexmain/runner/experimentrundelegate.h"

#include "apexmain/services/mainconfigfileparser.h"

#include "spincalibrator.h"
#include "spinexperimentconstants.h"
#include "spinexperimentcreator.h"

#include <QDebug>
#include <QFile>

#define TEMP_FILE "tmp.apx"

namespace spin
{

struct CalibrationParameter
{
    double target;
    double output;

    bool operator==(const CalibrationParameter& other) const;
    bool operator!=(const CalibrationParameter& other) const;

    /**
     * @return output - other.output
     */
    double differenceWith(const CalibrationParameter& other);
};

//hardware setup => (profile => (parametername => parameters))
typedef QMap<QString, QMap<QString, QMap<QString, CalibrationParameter> > > CalibrationMap;

class SpinCalibratorPrivate
{
    public:

        SpinCalibratorPrivate(const data::SpinConfig& config,
                              const data::SpinUserSettings& settings,
                              QWidget* parent);
        ~SpinCalibratorPrivate();

        bool calibrate(bool* changesMade = 0);
        void propagateChanges();

    private:

        void saveSpinParameters(CalibrationMap* to = 0);

        CalibrationMap spinParameters;

        apex::data::ExperimentData* experimentData;
        apex::ExperimentRunDelegate* runDelegate;
        apex::Calibrator* calibrator;
};

}//ns spin

using namespace spin;

//SpinCalibrator **************************************************************

SpinCalibrator::SpinCalibrator(const data::SpinConfig& config,
                               const data::SpinUserSettings& settings,
                               QWidget* parent) :
                           priv(new SpinCalibratorPrivate(config, settings, parent))
{
}

SpinCalibrator::~SpinCalibrator()
{
    delete priv;
}

bool SpinCalibrator::calibrate(bool* changesMade)
{
    return priv->calibrate(changesMade);
}

void SpinCalibrator::propagateChanges()
{
    priv->propagateChanges();
}


//SpinCalibratorPrivate *******************************************************

SpinCalibratorPrivate::SpinCalibratorPrivate(const data::SpinConfig& config,
                                             const data::SpinUserSettings& settings,
                                             QWidget* parent)
{
    SpinExperimentCreator creator(config, settings);
    creator.createExperimentFile(TEMP_FILE);
    apex::ExperimentParser parser(TEMP_FILE);
    experimentData = parser.parse(true);
    runDelegate = new apex::ExperimentRunDelegate(*experimentData,
                                                  apex::MainConfigFileParser::Get().data(), parent);
    runDelegate->makeModules();
    calibrator = new apex::Calibrator(runDelegate, *experimentData->calibrationData());
}

SpinCalibratorPrivate::~SpinCalibratorPrivate()
{
    delete runDelegate;
    delete experimentData;
    delete calibrator;

    QFile::remove(TEMP_FILE);
}

bool SpinCalibratorPrivate::calibrate(bool* changesMade)
{
    saveSpinParameters();

    if (!calibrator->calibrate(true))
        return false;

    if (changesMade != 0)
    {
        qCDebug(APEX_RS) << "checking for changes";
        CalibrationMap newParameters;
        saveSpinParameters(&newParameters);

        *changesMade = newParameters != spinParameters;
    }

    return true;
}

void SpinCalibratorPrivate::propagateChanges()
{
    CalibrationMap newParameters;
    saveSpinParameters(&newParameters);

    if (newParameters == spinParameters)
        return;

    //find the changed hardware setup
    QString changedSetup;
    Q_FOREACH (QString setup, newParameters.keys())
    {
        if (newParameters[setup] != spinParameters[setup])
            changedSetup = setup;
    }

    Q_ASSERT(!changedSetup.isNull());

    //find the profile that has been changed and all profiles that need
    //to be changed
    QString changedProfile;
    QStringList profilesToChange;
    Q_FOREACH (QString profile, newParameters[changedSetup].keys())
    {
        if (profile.startsWith(constants::CALIBRATION_PROFILE))
        {
            if (newParameters[changedSetup][profile] !=
                spinParameters[changedSetup][profile])
            {
                changedProfile = profile;
            }
            else
                profilesToChange << profile;
        }
    }

    //find the changed parameters, check how they changed and store in a map
    QMap<QString, double> changedParameters;
    Q_FOREACH (QString parameter,
               newParameters[changedSetup][changedProfile].keys())
    {
        CalibrationParameter oldParam =
                    spinParameters[changedSetup][changedProfile][parameter];
        CalibrationParameter newParam =
                    newParameters[changedSetup][changedProfile][parameter];

        if (oldParam != newParam)
            changedParameters.insert(parameter, newParam.differenceWith(oldParam));
    }

    //ok everything is known. adjust the parameters and store in the database
    apex::CalibrationDatabase db;

    Q_FOREACH (QString profile, profilesToChange)
    {
        Q_FOREACH (QString parameter, newParameters[changedSetup][profile].keys())
        {
            if (changedParameters.contains(parameter))
            {
                CalibrationParameter paramData =
                        spinParameters[changedSetup][profile][parameter];
                double newOutput = paramData.output + changedParameters[parameter];

                db.calibrate(changedSetup, profile, parameter,
                             paramData.target, newOutput);
            }
        }
    }
}


void SpinCalibratorPrivate::saveSpinParameters(spin::CalibrationMap* to)
{
    apex::CalibrationDatabase db;
    CalibrationMap parameters;

    Q_FOREACH (QString setup, db.hardwareSetups())
    {
        Q_FOREACH (QString profile, db.calibrationProfiles(setup))
        {
            if (!profile.startsWith(constants::CALIBRATION_PROFILE))
                continue;

            Q_FOREACH (QString parameterName, db.parameterNames(setup, profile))
            {
                qCDebug(APEX_RS) << "getting parameter for";
                qCDebug(APEX_RS) << "    " << setup;
                qCDebug(APEX_RS) << "    " << profile;
                qCDebug(APEX_RS) << "    " << parameterName;
                CalibrationParameter& parameter =
                                parameters[setup][profile][parameterName];

                parameter.output = db.outputParameter(setup, profile, parameterName);
                parameter.target = db.targetAmplitude(setup, profile, parameterName);
            }
        }
    }

    if (to == 0)
        spinParameters = parameters;
    else
        *to = parameters;
}

bool CalibrationParameter::operator==(const CalibrationParameter& other) const
{
    return output == other.output && target == other.target;
}

bool CalibrationParameter::operator!=(const CalibrationParameter& other) const
{
    return !(*this == other);
}

double CalibrationParameter::differenceWith(const CalibrationParameter& other)
{
    return output - other.output;
}
