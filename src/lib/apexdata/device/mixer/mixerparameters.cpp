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

#include "mixerparameters.h"

#include "xml/xmlkeys.h"
#include "xml/apexxmltools.h"

#include <QVariant>

#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;

using namespace apex;
using namespace apex::data;

MixerParameters::MixerParameters( ) : DeviceData(TYPE_MIXER)
//always get extended parameters (gain with id and channel), channel gets checked later on
{
    isControlDev=true;

    QString sModule = typeString();
    qDebug("Mixer: Type found: %s", qPrintable (sModule));
}

MixerParameters::~MixerParameters()
{
}

MixerType MixerParameters::type() const
{
  MixerType ret = DEFAULT_MIXER;
  QString type = typeString();

  if( type == "rme" )
    ret = RME_MIXER;
  else if( type == XMLKeys::sc_sPA5 || type== "pa5")
    ret = PA5_MIXER;
  else if ( xsiType() == XMLKeys::sc_sPA5)
      ret = PA5_MIXER;

  return ret;
}

QString MixerParameters::typeString() const
{
    return valueByType("type").toString();
}

QString MixerParameters::name() const
{
    QString name = valueByType("name").toString();

    if (name.isEmpty())
        return "default";

    return name;
}
