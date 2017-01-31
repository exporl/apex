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

#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "datablock/datablockdata.h"

#include "xml/xmlkeys.h"

#include "exceptions.h"
#include "idevicefactoryelement.h"

#include <QDateTime>
#include <QString>
#include <QUrl>

using namespace apex::XMLKeys;

namespace apex
{

class ExperimentRunDelegate;

namespace stimulus
{

/** Base class representing a single stimulus file on disk */
class APEX_EXPORT DataBlock : public IDeviceFactoryElement
{
    //FIXME [job refactory] move all implementations to cpp
public:
    /**
     * Constructor.
     * @param ac_Url the file location
     * @param ac_sID a unique id
     * @param ac_sDesc a description
     * @param ac_sDevice the device ID this datablock should be played on
     * @param ac_sBirt ??
     * @param ac_Params the parameters
     */
    DataBlock(const apex::data::DatablockData& data, const QUrl& filename,
              const ExperimentRunDelegate* experiment);

    /**
     * Copy Constructor with ID.
     * Default copy ctor is ok but we must not have duplicate id's.
     * @param other the one to copy
     * @param newId the new ID
     */
    DataBlock( const DataBlock& other, const QString& newId ) :
        filename(other.filename),
        data(other.data),
        experiment(other.experiment)
    {
        data.setId(newId);
    }

    /**
     * Destructor.
     */
    virtual ~DataBlock()
    {
    }

    /**
     * Get the module (= factory name) used to create this datablock.
     * @return string reference
     */
    virtual const QString& GetModule  () const
    { return sc_sDummyDevice; }

    /**
     * Get the id.
     * @return string reference
     */
    const QString&    GetID           () const
    { return data.id(); }

    /**
     * Get the description.
     * @return string reference
     */
    const QString&    GetDescription  () const
    { return data.description(); }

    /**
     * Get the device for this datablock.
     * @return string reference
     */
    const QString&    GetDevice       () const
    { return data.device(); }


    /**
     * Get the url.
     * @return string reference
     */
    const QUrl&       GetUrl          () const
    { return filename; }

    /**
     * Get the parameters.
     * @return const parameters
     */
    const apex::data::DatablockData& GetParameters() const
    { return data; }

    const QString GetMD5Sum() const;

    const QDateTime GetBirth() const;

    /**
     * Set a parameter.
     * Needed since some parameters (eg loop) are not known at the time
     * of constructing the datablock.
     */
    virtual void SetParameter( const QString& ac_sID, const QString& ac_sVal )
    { throw ApexStringException( "Can't set " + ac_sID + "to value" + ac_sVal + " on datablock" ); }

    /**
     * Create identical datablock with new id
     */
    virtual DataBlock* GetCopy(QString id);

protected:
    const QUrl filename;
    data::DatablockData data;
    const ExperimentRunDelegate* const experiment;
    /*DataBlock( const DataBlock& ac_Rh );
    DataBlock& operator= ( const DataBlock& ac_Rh );*/
};

} // namespace stimulus

} // namespace apex

#endif //#ifndef DATABLOCK_H
