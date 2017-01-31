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

#include "datablock.h"
#include "md5/md5.h"

#include <qstring.h>
#include <QFileInfo>

namespace apex
{
namespace stimulus
{

DataBlock::DataBlock(const apex::data::DatablockData& data, const QUrl& filename,
                      const ExperimentRunDelegate* experiment) :
                        filename(filename),
                        data(data),
                        experiment(experiment)
{}

const QString DataBlock::GetMD5Sum() const
{
    std::ifstream file(GetUrl().path().toLatin1());
    if (!file)
        throw ApexStringException("Can't open file for MD5 sum");
    MD5 context(file);
    char* hd = context.hex_digest();
    QString ret( hd );
    // avoid memory leak..
    delete[] hd;
    return ret;
}

const QDateTime DataBlock::GetBirth() const
{
    QFileInfo info (GetUrl().path().toLatin1());
    if (!info.exists())
        throw ApexStringException("Cannot check timestamp of file");
    return info.lastModified();
}


DataBlock* DataBlock::GetCopy(QString id)
{
    //qDebug("Copying datablock");
    return new DataBlock(*this, id);
}



}
}

