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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_PARAMETERS_APEXFILTERPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXMAIN_PARAMETERS_APEXFILTERPARAMETERS_H_

#include "stimulus/idevicefactoryelement.h"
//#include "apexextendedparameters.h"

namespace apex {
namespace stimulus {

  class ApexFilterParameters : public ApexExtendedParameters, public IDeviceFactoryElement {
  public:
    ApexFilterParameters();
    virtual ~ApexFilterParameters();

    virtual void SetParameter( const QString& ac_sType, const QString& ac_sValue );
    virtual void SetParameter( const QString& ac_sType, const QString& ac_sValue, const QString& ac_sOptionalID, const QString& ac_sOptionalChannel );

    unsigned mf_nChannels() const
    {
      return m_nChannels;
    }

  protected:
    unsigned m_nChannels;
  };

  typedef ApexFilterParameters FilterParameters;
  typedef ApexFilterParameters::const_iterator FilterParametersCIt;
  typedef ApexFilterParameters::iterator FilterParametersIt;

}
}

#endif
