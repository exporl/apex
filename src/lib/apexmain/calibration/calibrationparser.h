/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_CALIBRATIONPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_CALIBRATIONPARSER_H_

#include "apextools/xml/xercesinclude.h"

#include "parser/apexparser.h"

namespace XERCES_CPP_NAMESPACE
{

class DOMElement;

}; // namespace XERCES_CPP_NAMESPACE

namespace apex
{

namespace data
{

class CalibrationData;

} // namespace data

namespace parser
{

class CalibrationParser: public Parser
{
public:
    data::CalibrationData *Parse (XERCES_CPP_NAMESPACE::DOMElement *a_pBase);
};

} // namespace parser

} // namespace apex

#endif
