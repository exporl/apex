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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_PARAMETERS_PARAMETERMANAGER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_PARAMETERS_PARAMETERMANAGER_H_

#include "apexdata/parameters/parameter.h"
#include "apexdata/parameters/parametermanagerdata.h"

/**
 * A parameter has or can have:
 *      - type: what it does, e.g. gain
 *      - id: user defined and unique over an experiment
 *      - value
 *      - channel: if defined the paramter only has effect on this channel
 *
 */

namespace apex
{

class ParameterManager : public QObject
{
    Q_OBJECT

public:
    ParameterManager(const data::ParameterManagerData &d);

    /**
     * Set parameter to value. If canReset is false, the parameter will never be
     * reset to the default value.
     */
    void setParameter(const QString &id, const QVariant &value, bool canReset);

    void registerParameter(const QString &id, const data::Parameter &name);

    /**
     * Reset parameter to its default value, if available.
     */
    void resetParameter(const QString &id);

    QVariant parameterValue(const QString &id) const;

    /**
     * This function in conjunction with @ref setInternalState can be used
     * to store and restore the state of the parametermanager
     */
    data::PMRuntimeSettings internalState() const;

    /**
     * This function in conjunction with @ref internalState can be used
     * to store and restore the state of the parametermanager
     */
    void setInternalState(data::PMRuntimeSettings n);

    data::ParameterValueMap parametersForOwner(const QString &owner) const;

    /**
     * Return the parameter associated with an ID
     * returns a default constructed parameter if it is not found
     */
    data::Parameter parameter(const QString &id) const;

    /**
     * Reset parameters to their initial values, unless they are permanently
     * set by SetParameter
     */
    void reset();

    /**
     * Reset parameters to their initial values, no matter what
     */
    void forceReset();

    /**
     * @brief Explicitly set all parameters that have ever been set to their
     * default value.
     * This is different from reset in that reset will remove a parameter from
     * the list of parameters,
     * while this function will keep in in the list and set it to the default
     * value.
     * @param force if true, ignore the "cannot reset" flag
     */
    void setAllToDefaultValue(bool force);

    void showContents() const;

Q_SIGNALS:

    void parameterChanged(QString id, QVariant value);

private:
    data::ParameterManagerData data;

    /// keeps current parameter values
    data::PMRuntimeSettings paramValues;
};
}

#endif
