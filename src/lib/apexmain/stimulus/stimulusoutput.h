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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULUSOUTPUT_H_
#define _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULUSOUTPUT_H_

#include "apextools/apextypedefs.h"

#include "apexmodule.h"
#include "outputdevice.h"
#include "stimulus.h"

#include <QString>

namespace appcore
{
class WaitableObject;
}

namespace apex
{

class DeviceCheckThread; // TODO combine clipping and checking for end, (but
                         // clipping at a lower rate)

namespace device
{
class Controllers;
}

namespace stimulus
{

/**
  * StimulusOutput
  *   class responsible for playing stimuli on hardware devices.
  *   Has a thread for checking when the devices are finished playing.
  ******************************************************************* */
class StimulusOutput : public ApexModule
{
public:
    /**
      * Constructor.
      * Needs constructed tDeviceMap and tFilterMap from ApexControl.
      */
    StimulusOutput(ExperimentRunDelegate &p_rd);

    /**
      * Destructor.
      */
    ~StimulusOutput();

    /**
      * The different modes.
      */
    enum mt_eMode {
        /** initial value, not important */
        notset,

        /** LoadStimulus calls AddInput() for each datablock in the first column
         * of the PlayMatrix. The first time PlayStimulus() is called, it calls
         * PlayAll() for every device, wait until it's AllDone(), and then calls
         * RemoveAll() to clear the device's datablocks. This sequence
         * (AddInput, PlayAll, AllDone, Remove) is then repeated for every
         * column. */
        normal,

        /** There's only one device and it's capable of sequenced playing;
         * LoadStimulus() converts the PlayMatrix with ID's to a DataBlockMatrix
         * and passes it to the device calling SetSequence() to load all it's
         * datablocks at once; PlayStimulus() calls PlayAll(), AllDone() and
         * RemoveAll(). */
        singlesequence,

        /** There are multiple devices and they all support sequenced playing.
         * This is the default. LoadStimulus() creates a DataBlockMatrix per
         * device and calls SetSequence on all devices with the corresponding
         * matrix. The matrix will only contain datablocks for the device
         * called. PlayStimulus() calls PlayAll(), AllDone() and RemoveAll(). */
        onlysequence
    };

    /** Get offline mode. If IsOffLine == true (this is, when at least one of
     * the devices has it's offline mode set), StimulusOutput() makes a copy of
     * the devices supporting offline generation for stimuli. (devices
     * supporting this are able to create a single datablock containing
     * everything from one datablockmatrix, without opening the actual device,
     * and to copy theirselves) For every offline device, LoadStimulus() calls
     * CreateOffLine() on the device, and then calls AddInput() with the created
     * datablock on the copy of the device. PlayStimulus() calls whatever is
     * needed for one of the modes described above, but excluding any datablocks
     * that belong to offline devices: StimulusOutput overrides mt_eMode for the
     * device itself, by delegating it to the copy that does the offline
     * calculations. The actual mode used for the device is then 'normal' since
     * only it's AddInput() function called, once per stimulus.
     *
     * @return false when no offline devices are in use. */
    bool IsOffLine() const
    {
        return mc_bOffLine;
    }

    /**
      * Xml stuff.
      * @return result string
      */
    QString GetResultXML() const;

    /**
      * More Xml stuff.
      * @return end string
      */
    QString GetEndXML() const;

    /**
      * Get the object that will be signaled when all devices are done.
      * @return a WaitableObject reference
      */
    const appcore::WaitableObject &GetStimulusEnd();

    /**
      * Play currently loaded Stimulus.
      * Returns immediately after starting the devices.
      */
    void PlayStimulus();

    /**
      * Stop playing.
      * Use after PlayStimulus to interrupt playing.
      * Non-blocking.
      */
    void StopDevices();

    /**
      * Resume playing.
      * Use after StopDevices to resume playing.
      * Non-blocking.
      */
    void ResumeDevices();

    /**
     * Stop and close all devices
     */
    void CloseDevices();

    /**
     * Load ths Stimulus with the given ID.
     *
     * @param ac_sID the Stimulus' ID
     * @param p_restoreParams if true, all parameters will be restored to their
     * default values, otherwise the current values (from previous stimuli) will
     * be used
     */
    void LoadStimulus(const QString &ac_sID, const bool p_restoreParams = true);

    void LoadStimulus(Stimulus *ac_Stimulus, const bool p_restoreParams);

    /**
      * Unload the current Stimulus.
      * Calls RemoveAll() for every device.
      */
    void UnLoadStimulus();

    /**
      * Set the device to start as last.
      * @param ac_sID the device's ID
      */
    void SetMaster(const QString &ac_sID);

    /**
      * See SendParametersToClients().
      */
    void HandleParams();

    void RestoreParams();

    /**
      * Set a single parameter immediately, also for a running device
      * The new value will not be registered with the parametermanager
      * @param ac_sID the parameter's ID
      * @param ac_sValue the parameter's new value
      * @return true if the parameter was found and set
      */
    bool HandleParam(const QString &ac_sID, const QVariant &ac_sValue);

    /**
      * Get the devices we're using.
      * @return a tDeviceMap
      */
    const tDeviceMap &GetDevices() const
    {
        return m_pDevices;
    }

    /**
 * Have all things that accept parameters update their parameters
     */
    void SendParametersToClients();

    /**
 * Ask all clients (devices, controllers, filters) to do prepare
     */
    void PrepareClients();

    void setSilenceBeforeNextStimulus(double seconds);

    void releaseClients();

private:
    /**
      * Load one column from playmatrix' datablocks to their devices.
      * Only used when mode == normal.
      * @param ac_pMat the matrix
      * @param ac_nCol the column's index
      */
    void LoadOneSim(const PlayMatrix *ac_pMat, const unsigned ac_nCol);

    /**
      * Load everything.
      * @param ac_pMat the matrix
      */
    void LoadAll(const PlayMatrix *ac_pMat);

    /**
      * Load the filters.
      * Called once in constructor.
      */
    void LoadFilters();

    /**
      * Create offline copies and use them.
      */
    //     void DoOffLine();

    /**
      * Connect the filters.
      */
    void ConnectFilters();

    /**
      * Connect the DataBlocks.
      */
    void ConnectDataBlocks();

    /**
      * Play and wait for all devices to end.
      * Blocking.
      */
    void PlayUntilDone();

    /**
      * Play and signal thread to start checking.
      * Non-blocking.
      */
    void PlayAndWaitInThread();

    /**
     * Ask all clients to reset their parameters to the default values
     * and the values defined in the experiment file
     */
    void ResetParams();

    const mt_eMode mc_eMode;
    const bool mc_bOffLine;

    const tDeviceMap &m_pDevices;
    const tFilterMap &m_pFilters;
    const tDataBlockMap &m_DataBlocks;
    tDeviceMap m_OffLineDevices;

    Stimulus *m_pCurStim;
    // shortcuts to avoid indexing map
    OutputDevice *m_pCurDev;
    OutputDevice *m_pMaster;

    DeviceCheckThread *const mc_pWaitThread;
    bool m_bThreadRunning;

    const device::tControllerMap &m_pControllers;
};

} // end ns stimulus
} // end ns apex

#endif // STIMULUSOUTPUT_H
