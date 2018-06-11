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

#ifndef __Controllers_H__
#define __Controllers_H__

#include "apextools/apextypedefs.h"

#include "apexmodule.h"

namespace apex
{

namespace stimulus
{
class StimulusParameters;
}

namespace device
{

/**
  * Controllers
  *   class responsible for parameter output
  *   to hardware devices of any kind, that do not produce audible stimuli.
  *   Examples are mixers and attenuators.
  ************************************************************************* */
class Controllers : public ApexModule
{
public:
    /**
      * Constructor.
      */
    Controllers(ExperimentRunDelegate &p_rd);

    /**
      * Destructor.
      */
    ~Controllers();

    /**
      * Implementation of the ApexModule method.
      */
    QString GetResultXML() const;

private:
    const tControllerMap &m_Devices;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_CONTROLLERS_H_
