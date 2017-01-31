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
 
#error

this file uses the c++ interface, which gives libc compatibility problems


#include "apexnresocket.h"

#include <interface/NreSocketInterface.hpp>
using cochlear::nre::sockets::NreSocket;

#include <qfile.h>
#include <qtextstream.h>
#include <string>
#include <QStringList>

Q_EXPORT_PLUGIN2 (l34plugin, ApexNreSocketCreator)

namespace apex {
    namespace stimulus{

     ApexNreSocket::ApexNreSocket( const std::string& host, int p_deviceNr):
    ApexNreSocketInterface(host,p_deviceNr),
    socket(new NreSocket(host))
    {
        m_file = new QFile( QString("output-%1.dump").arg(p_deviceNr));
        m_file->open( QIODevice::WriteOnly);
        m_stream=new QTextStream(m_file);
    }

    ApexNreSocket::~ApexNreSocket(void)
    {
        m_file->close();
        delete m_file;
        delete m_stream;
    }

    const std::string& ApexNreSocket::connect( const std::string& device ) {
        return socket->connect(device);
    }

    const std::string&  ApexNreSocket::receive() {
        return socket->receive();
    }

    void  ApexNreSocket::disconnect() {
        socket->disconnect();
    }


    const std::string& ApexNreSocket::send( const std::string& message ) {
        (*m_stream)<<message.c_str();
        (*m_stream)<<"\n";
        //      try {
        return socket->send(message);
        /*      } catch (...) {
        return std::string();
        }*/
    }


    }
}



QStringList ApexNreSocketCreator::availablePlugins() const {
    return QStringList("l34plugin");
}


apex::stimulus::ApexNreSocketInterface *ApexNreSocketCreator::create (const std::string& host, int p_deviceNr ) const{
    return new apex::stimulus::ApexNreSocket(host,p_deviceNr);
}
