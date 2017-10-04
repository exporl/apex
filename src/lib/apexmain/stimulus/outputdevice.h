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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_OUTPUTDEVICE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_OUTPUTDEVICE_H_

#include <QObject>

#include "apexdata/device/devicedata.h"

#include "device/iapexdevice.h"

#include "streamapptypedefs.h"

// FIXME: [job refactory] move implementations to cpp
namespace apex
{
namespace stimulus
{

class Filter;
class DataBlock;
struct tConnection;

/**
  * Device
  *   Interface for all devices producing audible stimuli.
  ******************************************************** */
class OutputDevice : public QObject, public device::IApexDevice
{
    Q_OBJECT
protected:
    /**
      * Constructor.
      * @param ac_sID a unique identifier
      * @param a_pParameters the parameters
      */
    OutputDevice(data::DeviceData *a_pParameters)
        : IApexDevice(a_pParameters), m_bContinuousModeEnabled(true)
    {
    }

public:
    /**
     * Destructor.
     */
    virtual ~OutputDevice()
    {
    }

    /**
     * Add a filter.
     * The first filter should come between the input and the output,
     * the rest after the first one.
     * Can be overruled if connections are used.
     * [ stijn ] removed const to allow interaction with
     * filters after they're added
     * @param a_Filter the filter
     */
    virtual void AddFilter(Filter &a_Filter) = 0;

    /**
     * Add a datablock for playing.
     * the datablock must not be played until PlayAll() is called.
     * All datablocks added through this function must be played simultaneously.
     * @param ac_DataBlock the datablock
     */
    virtual void AddInput(const DataBlock &ac_DataBlock) = 0;

    /**
     * Remove all datablocks added.
     */
    virtual void RemoveAll() = 0;

    /**
     * Start playing.
     * Must return immedeately.
     */
    virtual void PlayAll() = 0;

    /**
     * Stop playing.
     * Must return immedeately.
     */
    virtual void StopAll() = 0;

    /**
     * Stop playing and any background processing
     */
    virtual void Finish()
    {
        StopAll();
    };

    /**
     * Wait until all datablocks are played.
     * This must be a blocking call, returning true when all datablocks are
     * played.
     * The device is supposed, but not required, to call it's StopAll before
     * returning.
     */
    virtual bool AllDone() = 0;

    /**
     * Get connectable.
     * @return true: the device allows connecting it's elements
     * @return false: there is only one signal path possible for the device
     */
    virtual bool CanConnect() const
    {
        return false;
    }

    /**
     * Get sequence.
     * StimulusOutput requires this so Devices better override this.
     * @return true if the device supports loading a sequence of datablocks.
     * All datablocks in the rows of the sequence have to be played right after
     * each other,
     * unrelated to what happens in other rows
     */
    virtual bool CanSequence() const
    {
        return false;
    }

    /**
     * Get offline capabilities.
     * @return true if the device implements the CreateOffLine() method
     */
    virtual bool CanOffLine() const
    {
        return false;
    }

    /**
     * Get offline status.
     * @return true if the device is set to work in offline mode in the
     * experiment file
     */
    virtual bool IsOffLine() const
    {
        return false;
    }

    /**
     * Setup connection between elements.
     * Elements are: datablocks, filters and the device itself.
     * @param ac_pConnection a connection
     */
    virtual bool AddConnection(const tConnection & /*ac_pConnection*/)
    {
        return false;
    }

    /**
     * Load any number of simultaneous/sequential datablocks.
     * @param the matrix; must be deleted by the device!
     */
    virtual void SetSequence(const DataBlockMatrix *ac_pSequence)
    {
        delete ac_pSequence;
        return;
    }

    /**
      * Create an offline DataBlock.
      * Given the currently loaded datablocks and filters, create a single
     * datablock
      * containing all the data that would normally be played; eg if normally 2
     * datablocks are mixed
      * and then processed through a filter, this method returns a datablock
     * pointing to the result
      * of the last filter. Use when the processing load is too heavy for the
     * cpu.
      * The datablock ID must be unique.
      * The device can safely assume that this function will not be called
     * during playing.
      * @return the offline DataBlock
      */
    //       virtual DataBlock* CreateOffLine()
    //       { return 0; }

    /**
      * Create a self-copy that does not open/contain the (hardware) device
     * itself.
      * @return the copy
      */
    virtual OutputDevice *CreateOffLineCopy()
    {
        return 0;
    }

    /**
    * Play silence of length time (s) before the next stimulus
    * only operates on the next stimulus
    * @param time time in seconds
    */
    virtual void SetSilenceBefore(double time) = 0;

    /**
    * If continuous mode is enabled, a device can decide to keep on playing
    * after the stop command was given. Enabling continuous does not mean
    * that the device WILL do this, but that is CAN do this.
    */
    virtual void EnableContinuousMode(bool e)
    {
        m_bContinuousModeEnabled = e;
    }

Q_SIGNALS:
    /**
    * is emitted by the device when the IO layer sends its stimulusStarted
    * signal
    */
    void stimulusStarted();

protected:
    bool m_bContinuousModeEnabled;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_OUTPUTDEVICE_H_
