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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_PLUGINFILTERPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_PLUGINFILTERPARAMETERS_H_

//#include "wavparameters.h"
#include <filter/filterdata.h>

namespace apex
{

namespace stimulus
{

/**
 * Parameter class for plugin filter
 *
 * @author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
class PluginFilterParameters : public data::FilterData
{
public:
    PluginFilterParameters();
    ~PluginFilterParameters();

    virtual void setParameter(const QString &type, const QString &value,
                              const QString &optionalId,
                              const QString &optionalChannel) = 0;

private:
    QString m_plugin; //! filename of plugin
};
}
}

#endif
