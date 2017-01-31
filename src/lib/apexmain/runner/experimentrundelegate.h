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

#ifndef __EXPERIMENTRUNDELEGATE_H__
#define __EXPERIMENTRUNDELEGATE_H__

//from libtools
#include "apextypedefs.h"
#include "status/errorlogger.h"
#include "global.h"

#include <qstringlist.h>
#include <map>

namespace apex
{
    namespace gui
    {
        class ScreenRunDelegate;
    }

    namespace stimulus {
        class OutputDevice;
        class StimulusOutput;
        class L34Device;
        class ClarionDevice;
        class DummyDevice;
        class Filter;
        class tConnections;
        class tConnectionsMap;
    }

    namespace data {
        class L34DeviceData;
        class ClarionDeviceData;
        class FilterData;
        class MixerParameters;
        class ExperimentData;
        class ApexTrial;
        class ApexProcedureConfig;
    }

    namespace device {
        class MixerParameters;
        class IMixer;
        class Controllers;
    }

    namespace gui {
        class ApexMainWindow;
    }


    class ApexProcedure;
    class ApexMultiProcedure;
    class ExperimentRunDelegatePrivate;
    using gui::ScreenRunDelegate;
    class ParameterManager;
    class Corrector;
    class ApexControl;

    class ApexTimer;
    class ApexScreen;
    class ApexResultSink;
    class RTResultSink;
    class Calibrator;
    class RandomGenerators;
    class Feedback;

    //FIXME [job refactory] this class shouldn't be exported
    class APEX_EXPORT ExperimentRunDelegate: public ErrorLogger {
        /**
         * Class containing all "dynamic" data for an experiment, ie data that is derived
         * from the corresponding data structure (ExperimentData) and eventually updated
         * while running
         */
    public:
        ExperimentRunDelegate (data::ExperimentData& experiment,
                               QWidget* parent, bool deterministic = false);
        virtual ~ExperimentRunDelegate();

        // Modules
        void makeModules(ApexControl* apexControl);

        ApexProcedure* modProcedure() const;
        ApexTimer* modTimer() const;
        ApexScreen* modScreen() const;
        Corrector* modCorrector() const;
        ApexResultSink* modResultSink() const;
        RTResultSink* modRTResultSink() const;
        RandomGenerators* modRandomGenerators() const;
        device::Controllers* modControllers() const;
        Calibrator*  modCalibrator() const;
        stimulus::StimulusOutput* modOutput() const;
        Feedback* modFeedback() const;
        const ModuleList* modules() const;


        ScreenRunDelegate* GetScreen (const QString& id );
        gui::ApexMainWindow* GetMainWindow() const;
        stimulus::tConnectionsMap& GetConnections() const;

        stimulus::tDeviceMap& GetDevices() const;
        stimulus::OutputDevice* GetDevice(const QString& id) const;
        stimulus::tFilterMap& GetFilters() const;
        stimulus::Filter* GetFilter(const QString& p_name) const;
        stimulus::Stimulus* GetStimulus(const QString& id) const;

        const device::tControllerMap& GetControllers() const;

        const stimulus::tDataBlockMap&              GetDatablocks() const;
        stimulus::DataBlock*                  GetDatablock(const QString& p_name) const;
        void    AddDatablock(const QString& name, stimulus::DataBlock* db);

        const data::ExperimentData& GetData() const {return experiment;};

        bool GetAutoAnswer() const;
        void SetAutoAnswer(const bool p);

        ParameterManager* GetParameterManager() const;


        // modules
        stimulus::StimulusOutput*  GetModOutput() const;

        QWidget* parentWidget() const;

    private:
        void FixStimuli();

        data::ExperimentData& experiment;
        // There are quite a bit of includes necessary, move the hassle to the c file
        ExperimentRunDelegatePrivate *priv;


        void MakeOutputDevices();
        void MakeControlDevices();

	ApexProcedure* MakeProcedure( data::ApexProcedureConfig* config = 0 );
	ApexMultiProcedure* MakeMultiProcedure( data::ApexProcedureConfig* config );
        void MakeDatablocks();
        void MakeStimuli();
        void MakeFilters();
        void MakeCorrector();
        void MakeFeedback();
        stimulus::Filter* MakeFilter( const QString& ac_sID, const QString& ac_sType,  data::FilterData* const ac_pParams );
        stimulus::L34Device* MakeL34Device( data::L34DeviceData* ac_Params );
        stimulus::ClarionDevice* MakeClarionDevice( data::ClarionDeviceData* p_Params );
        stimulus::OutputDevice* MakeDummyDevice( data::DeviceData* p_params);

#if defined(SETMIXER) || defined(PA5)
        apex::device::IMixer* MakeMixer( apex::data::MixerParameters* a_pParameters );
#endif

    };
}

#endif //#ifndef __EXPERIMENTRUNDELEGATE_H__