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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_L34STIMULUS_ASEQPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_L34STIMULUS_ASEQPARSER_H_

#include "l34data.h"

#include <QByteArray>
#include <QBuffer>
#include <QCoreApplication>
#include <QVector>

namespace apex
{

namespace stimulus
{

template <class T> class DataContainer: public QVector<T>
{
public:
    T targetvalue (int i) const {
        if (QVector<T>::size() == 0)
            return T(-1);
        if (QVector<T>::size() == 1)
            return QVector<T>::at(0);
        return QVector<T>::at(i);
    }
};

class APEX_EXPORT AseqParser
{
    Q_DECLARE_TR_FUNCTIONS(AseqParser)

public:
    AseqParser(const QByteArray &data);

    L34Data GetData();

    ~AseqParser();

private:
    QString readID();
    quint32 readLength();
    float readFloat32();
    void check(QString id, QString target);

    QBuffer buffer;
    const QByteArray data;

    template <class T>
            quint32 readData(QVector<T> &target, quint32 bytes)
    {
        Q_ASSERT( ! (bytes%sizeof(T)));
        quint32 frames=bytes/sizeof(T);
        target.resize(frames);
        //qCDebug(APEX_RS, "readData: %d bytes, %d frames", bytes, frames);
        buffer.read(reinterpret_cast<char*>(target.data()),
                    bytes);
        if (sizeof(T)==1 && bytes%2)
            buffer.read(1);             // padding
        return frames;
    }

    static void checkElectrodes(const DataContainer<qint8> &electrodes, quint32 readlen);
};

}

}

#endif
