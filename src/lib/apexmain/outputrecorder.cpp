#include "outputrecorder.h"

#include <experimentdata.h>
#include <filter/pluginfilterdata.h>
#include <device/devicesdata.h>
#include <connection/connectiondata.h>

#include <QDebug>
#include <QFileInfo>

using namespace apex;

bool OutputRecorder::setupRecording(data::ExperimentData* data)
{
    qDebug("Enabling output recording");
    //get the output device (there should be exactly one)
    const data::DevicesData* devices = data->devicesData();

    if (devices->size() != 1)
    {
        warning(tr("Can only record an experiment if there is exactly "
                   "one output device"));
        return false;
    }

    data::DevicesData::const_iterator devicesIt = devices->begin();
    QString deviceId = devicesIt.key();
    const data::DeviceData* device = devicesIt.value();

    if (device->deviceType() != data::TYPE_WAVDEVICE)
    {
        warning(tr("Can only record an experiment if a wav device is used"));
        return false;
    }

    //create a filesink filter
    QString sinkId = "filesink";
    int nbChannels = device->numberOfChannels();
    data::PluginFilterData* filterData = new data::PluginFilterData();
    filterData->setId(sinkId);
    filterData->setXsiType("apex:pluginfilter");
    filterData->setValueByType("plugin", "filesink");
    filterData->setValueByType("channels", nbChannels);
    QString outFile = "%0-${count}.wav";
    outFile = outFile.arg(QFileInfo(data->fileName()).baseName());
    filterData->setValueByType("uri", outFile);
    filterData->setValueByType("device", deviceId);
    data->addFilter(filterData);

    //modify connections
    data::ConnectionsData* connections = data->connectionsData();

    //create the connections to the sink
    if (connections->isEmpty())
    {
        //if there are no connections, just create default connections: all
        //channels of all datablocks to the sink
        for (int channel = 0; channel < nbChannels; channel++)
        {
            data::ConnectionData* toSink = new data::ConnectionData();
            toSink->setFromId("_ALL_");
            toSink->setFromChannel(channel);
            toSink->setToId(sinkId);
            toSink->setToChannel(channel);
            connections->append(toSink);
        }
    }
    else
    {
        //there are connections so we insert the sink just before the device.
        //modify all connections that have the device as endpoint to have the
        //sink as endpoint
        for (data::ConnectionsData::iterator connection = connections->begin();
             connection != connections->end(); ++connection)
        {
            if ((*connection)->toId() == deviceId)
                (*connection)->setToId(sinkId);
        }
    }

    //create the connections from the sink to the device
    for (int channel = 0; channel < nbChannels; channel++)
    {
        data::ConnectionData* toDevice = new data::ConnectionData();
        toDevice->setFromId(sinkId);
        toDevice->setFromChannel(channel);
        toDevice->setToId(deviceId);
        toDevice->setToChannel(channel);
        connections->append(toDevice);
    }

    return true;
}

void OutputRecorder::warning(const QString& msg)
{
    log().addWarning("OutputRecorder", msg);
}