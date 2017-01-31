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

#ifndef NRESOCKETDEBUGI__
#define NRESOCKETDEBUGI__

#include <QString>
#include <QtPlugin>


/**
* wrapper for NreSocket
* allows L34 to be used as a plugin
*/
namespace apex
{

namespace stimulus
{

class ApexNreSocketInterface
{
public:
    virtual ~ApexNreSocketInterface()
    {
    }

    virtual QString send( const QString& message )=0;
    virtual QString connect( const QString& device )=0;
    virtual QString receive()=0;
    virtual void disconnect()=0;
};

}

}

class ApexNreSocketCreatorInterface
{
public:
    virtual ~ApexNreSocketCreatorInterface()
    {
    }

    virtual QStringList availablePlugins() const = 0;

    // empty name creates default plugin, caller must free returned class,
    // returns NULL if impossible to create

    virtual apex::stimulus::ApexNreSocketInterface *create (const QString& host = "", int p_deviceNr=0 ) const = 0;
};

Q_DECLARE_INTERFACE (ApexNreSocketCreatorInterface, "be.exporl.apex.apexnresocket/1.0")

#endif
