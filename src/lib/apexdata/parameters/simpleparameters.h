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

#ifndef _EXPORL_SRC_LIB_APEXDATA_PARAMETERS_SIMPLEPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXDATA_PARAMETERS_SIMPLEPARAMETERS_H_

// from libtools
#include "apextools/global.h"

#include "parameter.h"

namespace apex
{

namespace data
{

struct SimpleParametersPrivate;
class Parameter;

typedef QList<Parameter> ParameterList;

/**
 * @class SimpleParameters
 *
 * @brief Container for data for all apex things that have parameters.
 *
 * Can be parsed from XML using @ref SimpleParametersParser.
 * There are generally 2 types of parameters: those that can be changed during
 * the experiment and those that cannot.
 * Parameters that can be changed usually have default values set in the
 * device/filter/... itself. Parameters that cannot be changed, can have
 * default values that should be set by calling @ref setValueByType in the
 * constructor of the class inheriting from this class.
 * Generally you should set a default value for every parameter that is optional
 * in the experiment file.
 *
 * @note This class does not actually store the values of the parameters, just
 *       their default values. @ref ParameterManager takes care of the values.
 */
class APEXDATA_EXPORT SimpleParameters
{
public:
    SimpleParameters();
    SimpleParameters(const SimpleParameters &other);
    virtual ~SimpleParameters();

    // getters

    /**
     * Returns the value for the given type.
     *
     * @throws ApexStringException If multiple parameters with the given
     *                             type exist.
     *
     * @return  A default constructed value if there is no
     *          parameter with the given type.
     */
    QVariant valueByType(const QString &type) const;

    /**
     * Returns the parameter corresponding to the given type.
     *
     * @throws ApexStringException If multiple parameters with the given
     *                             type exist.
     *
     * @return  A default constructed ParameterName if there is no
     *          parameter with the given type.
     */
    Parameter parameterByType(const QString &type) const;
    virtual bool hasParameter(const QString &type) const;
    ParameterList parameters() const;
    virtual QString id() const;
    QString xsiType() const;

    // setters

    /**
     * Sets the parameter corresponding to \param type to \param value.
     * If there is no such parameter, a default one will be constructed.
     *
     * @throws ApexStringException If multiple parameters with the given
     *                             type exist.
     */
    virtual void setValueByType(const QString &type, const QVariant &value);
    virtual void setId(const QString &id);
    void setXsiType(const QString &type);
    void addParameter(const Parameter &param);
    void removeParameter(const Parameter &param);

    SimpleParameters &operator=(const SimpleParameters &other);
    virtual bool operator==(const SimpleParameters &other) const;

private:
    SimpleParametersPrivate *d;
};

} // ns data

} // ns apex

#endif
