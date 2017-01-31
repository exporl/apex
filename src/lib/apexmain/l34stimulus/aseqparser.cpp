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

#include "aseqparser.h"
#include <QVector>
#include <QtEndian>

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#error Byte order not implemented
#endif

namespace apex
{

namespace stimulus
{

AseqParser::AseqParser(const QByteArray &data) :
    data(data)
{
}

AseqParser::~AseqParser()
{
}

void AseqParser::checkElectrodes(const DataContainer<qint8> &electrodes, quint32 readlen)
{
    for (unsigned i = 0; i < readlen; ++i) {
        if (electrodes.targetvalue(i) < -3 || electrodes.targetvalue(i) > 22)
            throw ApexStringException(tr("Invalid electrode"));
    }
}

L34Data AseqParser::GetData()
{
    // Read header and info block

    // use a buffer, makes it easy to refactor to reading a QIODevice
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);
    //QDataStream d(&buffer);

    check(readID(),"RIFF");
    quint32 length=readLength();
    if ((int)length!=data.size())
        throw ApexStringException(tr("Internal parser error 1: %1!=%2")
                                 .arg(length).arg(data.size()));
    check(readID(),"ASEQ");

    // Setup buffers
    DataContainer<qint8> activeElectrodes;
    DataContainer<qint8> returnElectrodes;
    DataContainer<quint8> currentLevels;
    DataContainer<float> phaseWidths;
    DataContainer<float> phaseGaps;
    DataContainer<float> periods;
    DataContainer<qint8> channels;
    DataContainer<float> magnitudes;

    // Read INFO chunk
    check(readID(),"INFO");
    qint32 infolen=readLength();
    float version=readFloat32();
    if (version!=1.0)
        throw ApexStringException(tr("AQIC version %f not supported")
                .arg(version));

    bool channelsFound=false;
    bool magnitudesFound=false;
    bool activeElectrodesFound=false;
    bool currentLevelsFound=false;
    unsigned totallength=0;
    int nchunks=0;
    for (int i=0; i<infolen/8; ++i) {
        QString id=readID();
        totallength+=readLength();
        //qCDebug(APEX_RS, "AQIC: Found chunk %s with length %d", qPrintable(id), len);

        if (id=="AELE")
            activeElectrodesFound=true;
        else if (id=="MAGN")
            magnitudesFound=true;
        else if (id=="CHAN")
            channelsFound=true;
        else if (id=="CURL")
            currentLevelsFound=true;
        ++nchunks;
    }

    /*if (magnitudesFound ^ channelsFound)
        throw ApexStringException(
            tr("Magnitudes and channels must either not or both be defined"));*/

    if (!magnitudesFound && !currentLevelsFound)
        throw ApexStringException(
            tr("No stimulation levels defined"));

    if (!activeElectrodesFound && !channelsFound)
        throw ApexStringException(
            tr("No stimulation electrodes defined"));

    if (magnitudesFound && currentLevelsFound)
        throw ApexStringException(
            tr("Both magnitudes and current levels defined"));

    if (activeElectrodesFound && channelsFound)
        throw ApexStringException(
            tr("Both electrodes and channels defined"));

    // Read chunks
    qint64 maxLen=-1;
    while (1) {
        QString id=readID();
        if (buffer.atEnd())
            break;
        qint32 len=readLength();
        //qCDebug(APEX_RS, "AQIC: Reading chunk %s with length %d", qPrintable(id), len);

        quint32 readlen=0;
        if (id=="AELE") {
            readlen=readData<qint8>(activeElectrodes, len);
            checkElectrodes(activeElectrodes, readlen);
        } else if (id=="RELE") {
            readlen=readData<qint8>(returnElectrodes, len);
            checkElectrodes(returnElectrodes, readlen);
        } else if (id=="CURL") {
            readlen=readData<quint8>(currentLevels, len);
        } else if (id=="PHWI") {
            readlen=readData<float>(phaseWidths, len);
        } else if (id=="PHGA") {
            readlen=readData<float>(phaseGaps, len);
        } else if (id=="PERI") {
            readlen=readData<float>(periods, len);
        } else if (id=="CHAN") {
            readlen=readData<qint8>(channels, len);
            for (unsigned i=0; i<readlen; ++i) {
                if ( channels.targetvalue(i) < 0 )
                    throw ApexStringException(tr("Invalid channel"));
            }
        } else if (id=="MAGN") {
            readlen=readData<float>(magnitudes, len);
        } else {
            if (len%2)
                ++len;      // padding
            buffer.seek(buffer.pos()+len);
            qCDebug(APEX_RS, "Skipping unknown block with id %s", qPrintable(id));
        }

        if (maxLen>1 && readlen>maxLen)
            throw ApexStringException(tr("Deviant chunk size length")
                    .arg(version));
        if ((qint64)readlen>maxLen)
            maxLen=readlen;

    }

    buffer.close();

    // Convert vectors into L34Data structure
    L34Data ldata(maxLen);
    for (long i=0; i<maxLen; ++i) {
        // FIXME: check range of values

        ldata[i]=L34Stimulus(channels.targetvalue(i),
                             magnitudes.targetvalue(i),
                             periods.targetvalue(i),
                             phaseWidths.targetvalue(i),
                             phaseGaps.targetvalue(i),
                             activeElectrodes.targetvalue(i),
                             returnElectrodes.size()?
                                 returnElectrodes.targetvalue(i):0,
                             currentLevels.size()?
                                currentLevels.targetvalue(i):-1 );
        // currentLevels is unsigned char, so cannot return -1
    }

    return ldata;
}

QString AseqParser::readID()
{
    // FIXME (?)
    return buffer.read(4);
}

quint32 AseqParser::readLength()
{
    return qFromLittleEndian<quint32>((uchar*)buffer.read(4).data());
}

float AseqParser::readFloat32()
{
    return *reinterpret_cast<float*>(buffer.read(4).data());
}



void AseqParser::check(QString id, QString target)
{
    if (id!=target) {
        throw ApexStringException(
                tr("AseqParser: invalid chunk ID %1, expected %2")
                .arg(id).arg(target));
    }
}


}

}
