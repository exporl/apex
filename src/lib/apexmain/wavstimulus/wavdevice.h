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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_WAVDEVICE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_WAVDEVICE_H_

#include "apextools/apextypedefs.h"

#include "apextools/xml/xmlkeys.h"

#include "connection/connection.h"

#include "stimulus/outputdevice.h"

#include "wavdeviceio.h"

#include "bertha/experimentdata.h"

namespace apex
{

namespace data
{

class WavDeviceData;
}

namespace stimulus
{

class WavFilter;

class ApexSeqStream;

typedef std::map<QString, ApexSeqStream *> tSeqMap;
typedef std::map<QString, StreamGenerator *> tGenMap;
typedef std::vector<WavFilter *> tWavFilters;

/**
 * WavDevice
 *   Device implementation for playing wavefiles,
 *   or generated stimuli.
 ************************************************ */

class WavDevice : public OutputDevice
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    WavDevice(data::WavDeviceData *p_data,
              const bertha::ExperimentData &berthaData, bool useBertha);

    /**
     * Destructor.
     */
    virtual ~WavDevice();

    /**
     * Implementation of the Device method.
     */
    virtual void AddFilter(Filter &a_Filter) Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    virtual void AddInput(const DataBlock &ac_DataBlock) Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    virtual void RemoveAll() Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    virtual bool
    AddConnection(const tConnection &ac_pConnection) Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    virtual void
    SetSequence(const DataBlockMatrix *ac_pSequence) Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    //         DataBlock* CreateOffLine();

    /**
     * Implementation of the Device method.
     */
    // OutputDevice* CreateOffLineCopy();

    /**
     * Implementation of the Device method.
     */
    void PlayAll() Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    virtual void StopAll() Q_DECL_OVERRIDE;

    virtual void Finish() Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    virtual bool AllDone() Q_DECL_OVERRIDE;

    virtual bool SetParameter(const QString &type, int channel,
                              const QVariant &value) Q_DECL_OVERRIDE;

    /**
     * Reset the filter to its initial state and set all internal parameters to
     * built in
     * default values
     * throw exception if problem
     */
    virtual void Reset() Q_DECL_OVERRIDE;

    /**
     * Prepare filter for processing
     * throw exception if problem
     */
    virtual void Prepare() Q_DECL_OVERRIDE;

    // FIXME: [job refactory] move implementations to cpp

    /**
     * Implementation of the Device method.
     */
    virtual bool HasParameter(const QString &ac_ParamID) const Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     */
    virtual bool CanConnect() const Q_DECL_OVERRIDE
    {
        return true;
    }

    /**
     * Implementation of the Device method.
     */
    virtual bool CanSequence() const Q_DECL_OVERRIDE
    {
        return true;
    }

    /**
     * Implementation of the Device method.
     */
    virtual bool CanOffLine() const Q_DECL_OVERRIDE
    {
        return true;
    }

    /**
     * Implementation of the Device method.
     */
    virtual bool IsOffLine() const Q_DECL_OVERRIDE
    {
        return false;
    }

    /**
     * Implementation of the Device method.
     */
    virtual QString GetResultXML() const Q_DECL_OVERRIDE;

    /**
     * Implementation of the Device method.
     * Gets clipping info.
     */
    virtual bool GetInfo(const unsigned ac_nType,
                         void *a_pInfo) const Q_DECL_OVERRIDE;

    int GetBlockSize() const;

    virtual void SetSilenceBefore(double time) Q_DECL_OVERRIDE
    {
        m_SilenceBefore = time;
    }

    WavDeviceIO *getWavDeviceIo();

private:
    void SetConnectionParams();
    void CheckClipping();

private:
    bool mf_bIsNamedConnection(const QString &ac_sID) const;
    void mp_RemoveNamedConnection(const QString &ac_sFromID);
    void mp_AddSeqStream(ApexSeqStream *pStream, const QString &ac_sID);

    data::WavDeviceData *data; // FIXME make const
    QString mv_sOffLine;       // temp
    float m_SilenceBefore;
    tSeqMap m_InputStreams;
    tGenMap m_Generators;
    tWavFilters m_InformFilters;
    tConnections m_NamedConnections;
    /**
     * Cache for parameters that refer to connections
     * They have to be cached until the Prepare call because
     * connections cannot be set before they are made (when
     * a stimulus is loaded
     */
    QMap<QString, QVariant> m_connection_param_cache;

    WavDeviceIO m_IO;

    // QString m_driver;       // real driver and card name, as looked
    // QString m_card;         // up in mainconfig

    mutable QStringList xmlresults; // cache for GetResultXML()
    bertha::ExperimentData berthaData;
    bool useBertha;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_WAVDEVICE_H_
