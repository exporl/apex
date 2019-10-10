/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_EXPERIMENTRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_EXPERIMENTRUNDELEGATE_H_

#include "apextools/apextypedefs.h"
#include "apextools/global.h"

#include <QMap>
#include <QStringList>

#include <map>

namespace bertha
{
class ConnectionData;
class ExperimentData;
}

namespace apex
{

class ProcedureInterface;

namespace gui
{

class ScreenRunDelegate;
}

namespace stimulus
{

class OutputDevice;

class StimulusOutput;

class CohDevice;

class DummyDevice;

class Filter;

class tConnections;

class tConnectionsMap;
}

namespace data
{

class CohDeviceData;
class FilterData;
class ExperimentData;
class ConnectionData;
class ApexTrial;
class ProcedureData;
class MainConfigFileData;
}

namespace device
{

class Controllers;
}

namespace gui
{

class ApexMainWindow;
}

class Procedure;
class ApexMultiProcedure;
class ExperimentRunDelegatePrivate;
using gui::ScreenRunDelegate;
class ParameterManager;
class ApexControl;
class TrialStartTime;
class ApexTimer;
class ApexScreen;
class ApexResultSink;
class Calibrator;
class RandomGenerators;
class Feedback;
class RTResultSink;
class ProcedureApi;
class StudyModule;

// FIXME [job refactory] this class shouldn't be exported

class APEX_EXPORT ExperimentRunDelegate : public QObject
{
    Q_OBJECT

    /**
     * Class containing all "dynamic" data for an experiment, ie data that is
     * derived
     * from the corresponding data structure (ExperimentData) and eventually
     * updated
     * while running
     */
public:
    ExperimentRunDelegate(data::ExperimentData &experiment,
                          const data::MainConfigFileData &maindata,
                          QWidget *parent, bool deterministic = false,
                          bool useBertha = false);

    virtual ~ExperimentRunDelegate();

    // Modules
    void makeModules();

    // Procedure* modProcedure() const;
    TrialStartTime *trialStartTime() const;
    ApexTimer *modTimer() const;
    ApexScreen *modScreen() const;
    ApexResultSink *modResultSink() const;
    RTResultSink *modRTResultSink() const;
    RandomGenerators *modRandomGenerators() const;
    device::Controllers *modControllers() const;
    Calibrator *modCalibrator() const;
    stimulus::StimulusOutput *modOutput() const;
    Feedback *modFeedback() const;
    StudyModule *modStudy() const;
    const ModuleList *modules() const;

    ScreenRunDelegate *GetScreen(const QString &id);
    gui::ApexMainWindow *GetMainWindow() const;

    stimulus::tConnectionsMap &GetConnections() const;

    stimulus::tDeviceMap &GetDevices() const;
    stimulus::OutputDevice *GetDevice(const QString &id) const;

    stimulus::tFilterMap &GetFilters() const;
    stimulus::Filter *GetFilter(const QString &p_name) const;

    stimulus::Stimulus *GetStimulus(const QString &id) const;
    QStringList stimuli() const;

    const device::tControllerMap &GetControllers() const;
    device::ControlDevice *GetController(const QString &id) const;

    const stimulus::tDataBlockMap &GetDatablocks() const;
    stimulus::DataBlock *GetDatablock(const QString &p_name) const;
    void AddDatablock(const QString &name, stimulus::DataBlock *db);

    const data::ExperimentData &GetData() const;
    const bertha::ExperimentData &getBerthaExperimentData() const;
    bool usingBertha() const;

    bool GetAutoAnswer() const;
    void enableAutoAnswer();

    ParameterManager *GetParameterManager() const;

    const data::MainConfigFileData &mainData() const;

    // modules
    stimulus::StimulusOutput *GetModOutput() const;
    StudyModule *getStudyModule() const;

    QWidget *parentWidget() const;

    ProcedureInterface *makeProcedure(ProcedureApi *api,
                                      const data::ProcedureData *data);

private:
    void FixStimuli();

    data::ExperimentData &experiment;
    // There are quite a bit of includes necessary, move the hassle to the c
    // file
    ExperimentRunDelegatePrivate *d;

    void MakeOutputDevices();
    void MakeControlDevices();
    void MakeDatablocks();
    void MakeStimuli();
    void MakeFilters();
    void MakeFeedback();
    stimulus::Filter *MakeFilter(const QString &ac_sID, const QString &ac_sType,
                                 data::FilterData *const ac_pParams);
#ifdef ENABLE_COH
    stimulus::CohDevice *MakeCohDevice(data::CohDeviceData *ac_Params);
#endif
    stimulus::OutputDevice *MakeDummyDevice(data::DeviceData *p_params);

    void MakeBerthaExperimentData();
    void MakeBerthaDataBlock(const data::DatablockData &dataBlockData);
    void
    FillDataBlockConnections(QMap<QString, QVector<bertha::ConnectionData>> &);
    void
    MakeBerthaSummationBlocks(QMap<QString, QVector<bertha::ConnectionData>> &);
    void AddBerthaConnection(QMap<QString, QVector<bertha::ConnectionData>> &,
                             const data::ConnectionData *);

signals:
    void showMessage(QString message) const;
};
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_EXPERIMENTRUNDELEGATE_H_
