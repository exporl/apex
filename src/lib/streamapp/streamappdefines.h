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

#ifndef _APEX_SRC_LIB_STREAMAPP_STREAMAPPDEFINES_H__
#define _APEX_SRC_LIB_STREAMAPP_STREAMAPPDEFINES_H__

#include "containers/matrix.h"

namespace streamapp
{
  typedef double                            StreamType;
  typedef MatrixAccess<StreamType>          Stream;
  typedef MatrixStorage<StreamType>         StreamBuf;
  typedef ExternalMatrixAccess<StreamType>  StreamPtrBuf;
  typedef ArrayAccess<StreamType>           StreamChannel;
  typedef ArrayStorage<StreamType>          StreamChannelBuf;
  typedef ArrayStorage<StreamType*>         StreamChanPtr;

  typedef double                            GainType; //!< for mixers etc
}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_STREAMAPPDEFINES_H__
