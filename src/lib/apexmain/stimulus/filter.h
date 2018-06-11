/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_FILTER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_FILTER_H_

#include "apexdata/filter/filterdata.h"

#include "apexdata/parameters/parameter.h"

#include "streamapp/defines.h"

namespace apex
{

class ParameterManager;

namespace stimulus
{

/**
 * Filter
 *   general Filter InterFace. A filter normally is something
 *   that takes a datablock as input, processes it, and outputs the result.
 *   All classes deriving from ths interface should check in their
 *   ctor whether the parameters supplied are the correct ones
 *   eg an amplifier should throw error when param "amplitude" is not there.
 *   @note the parameter system works the same way as for IApexDevice, so maybe
 *   we should have a common interface for that.
 ******************************************************************************
 */
class Filter
{
public:
    /**
     * Constructor.
     * @param ac_sID a unique id
     * @param ac_sInput ?? no idea if this is used
     * @param ac_sOutput identifier of the device this filter is made for
     * @param ac_pParams pointer to parameters, won't be deleted
     */
    Filter(const QString &ac_sID, data::FilterData *const ac_pParams)
        : mc_sID(ac_sID),
          m_Params(ac_pParams) //[ stijn ] removed copy, just keep pointer,
                               // deletion is managed by factory
    {
    }

    /**
     * Destructor.
     */
    virtual ~Filter()
    {
    }

    /**
     * Get the ID.
     * @return string reference
     */
    INLINE const QString &GetID() const
    {
        return mc_sID;
    }

    /**
     * Get the device this filter is made for.
     * @return string reference
     */
    INLINE const QString GetDevice() const
    { // return mc_sOutput;
        return m_Params->device();
    }

    /** Set parameters by querying the given parametermanager object
    */
    virtual void SetParameters(ParameterManager *pm);

    virtual bool SetParameter(data::Parameter name, QVariant value);

    /**
     * Check if this filter has the given parameter.
     * @return true if so.
     */
    INLINE virtual bool HasParameter(const QString &ac_sParamID)
    {
        return m_Params->hasParameter(ac_sParamID);
    }

    /**
     * Restore all parameters to their default value.
     */
    virtual void RestoreParameters()
    {
        Q_FOREACH (data::Parameter param, m_Params->parameters())
            SetParameter(param.type(), param.channel(), param.defaultValue());

        //                     for ( data::FilterData::const_iterator it=
        //                     m_Params->begin(); it!=m_Params->end();++it )
        //                     {
        //                         SetParameter ( it.key().type(),
        //                         it.key().channel(), it.value() );
        //                     }
    }

    /**
     * Reset the filter to its initial state and set all internal parameters to
     * built in
     * default values
     * throw exception if problem
     */
    virtual void Reset() = 0;

    /**
     * Prepare filter for processing
     * throw exception if problem
     */
    virtual void Prepare() = 0;

    /**
     * Get a reference to the parameters.
     * @return a const ApexFilterParameters reference
     */
    INLINE const data::FilterData &GetParameters() const
    {
        return *m_Params;
    }

protected:
    virtual bool SetParameter(const QString &type, const int channel,
                              const QVariant &value) = 0;

private:
    const QString mc_sID;
    /*        const QString mc_sInput;
              const QString mc_sOutput;*/

protected:
    data::FilterData *const m_Params;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_FILTER_H_
