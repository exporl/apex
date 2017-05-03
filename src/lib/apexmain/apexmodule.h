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

#ifndef _APEX_SRC_LIB_APEXMAIN_APEXMODULE_H_
#define _APEX_SRC_LIB_APEXMAIN_APEXMODULE_H_

#include <QObject>

#include "apextools/global.h"

namespace apex {

class ApexControl;
class ApexErrorHandler;
class ExperimentRunDelegate;

/**
Apex module interface, every apex module should inherit from this class
*/
class APEX_EXPORT ApexModule: public QObject {
    Q_OBJECT

    public:
        ApexModule(ExperimentRunDelegate& p_rd);
public:
  virtual QString GetResultXML() const;
  virtual QString GetEndXML() const;

    protected:
        ExperimentRunDelegate& m_rd;
};

}

#endif
