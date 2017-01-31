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

#ifndef L34DATABLOCK_H
#define L34DATABLOCK_H

#include "stimulus/datablock.h"
#include "l34data.h"

#include <QUrl>
#include <QStringList>
#include <QVector>

//FIXME [job refactory] move implementation to cpp

namespace apex
{

class ExperimentRunDelegate;

namespace data
{
class ApexMap;
}

namespace stimulus
{

const char * const NIC_SEQUENCE_OPEN = "<nic:sequence "
    "xmlns:nic='http://www.cochlear.com/nic' "
    "xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' "
    "xsi:schemaLocation='http://www.cochlear.com/nic nic-stimuli.xsd' >";
const char * const NIC_SEQUENCE_END = "</nic:sequence>";
const char * const NIC_STREAM_OPEN =
        "<nre:stream xmlns:nre='http://www.cochlear.com/nre'>";
const char * const NIC_STREAM_END =
        "</nre:stream>";

class APEX_EXPORT L34XMLData: public QStringList
{
public:
    L34XMLData():
        m_length(0) {}
        void SetLength(long mus) { m_length=mus; }        //! Set length to mus microseconds
        long GetLength() const { return m_length; }       //! Add mus microseconds to length
        void AddToLength(long mus) { m_length+=mus;}

private:
    long m_length;        // length in microseconds

};

//typedef std::vector<QString> L34XMLData;              // vector is hier vreemd genoeg sneller dan list
class APEX_EXPORT L34DataBlock : public DataBlock
{
    Q_DECLARE_TR_FUNCTIONS (L34DataBlock)
public:
    L34DataBlock(const data::DatablockData& data, const QUrl& filename,
                 const ExperimentRunDelegate* experiment);
    ~L34DataBlock();

    //const L34XMLData& GetData() const;
    const L34XMLData& GetMappedData(data::ApexMap* pMap, float volume) const;
    bool canMap(data::ApexMap* pMap) const;

private:

    // Matthias: [use file-extension for discriminating between qic and qicext]
    QString file_extension;

    bool ParseFile(QString pFilename);
    //void ConvertToXMLData() const;

    void ConvertToMappedXMLData(data::ApexMap* pMap, float volume) const;

    virtual const QString& GetModule() const
    {
        return sc_sL34Device;
    }

    //std::string m_sData;
    L34Data m_Data;
    mutable L34XMLData m_XMLData;
    mutable bool bIsXMLData;              // determines whether the stimulus is XML data and thus doesn't have to be mapped
    mutable float mappedForVolume;            //! the volume parameter for which the mapping is done, is -1 if no xml data is available
    mutable double m_length;                          //! datablock length in microseconds
};
}
}

#endif //L34DATABLOCK_H
