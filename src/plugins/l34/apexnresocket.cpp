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


#include "apexnresocket.h"

#include <interface/NreSocketInterface.h>

#include <qfile.h>
#include <qtextstream.h>
#include <string>
#include <QStringList>

Q_EXPORT_PLUGIN2 (l34plugin, ApexNreSocketCreator)

namespace apex {
    namespace stimulus{

     ApexNreSocket::ApexNreSocket( const QString& host, int p_deviceNr):
    socket(INVALID_NRE_SOCKET)
    {
        socket = nreSocketCreate( "0.0.0.0" );
        if (socket==INVALID_NRE_SOCKET)
            throw(0);

        m_file = new QFile( QString("output-%1.dump").arg(p_deviceNr));
        m_file->open( QIODevice::WriteOnly);
        m_stream=new QTextStream(m_file);
    }

    ApexNreSocket::~ApexNreSocket(void)
    {
        nreSocketClose( socket );
        m_file->close();
        delete m_file;
        delete m_stream;
    }

    QString ApexNreSocket::connect( const QString& device ) {
        const char* response = nreSocketConnect( socket, device.toAscii() );
        return QString(response);
    }

    QString  ApexNreSocket::receive() {
        const char* response = nreSocketReceive(socket);
        return QString(response);
    }

    void  ApexNreSocket::disconnect() {
        nreSocketDisconnect( socket );
    }


    QString ApexNreSocket::send( const QString& message ) {
         const char* response = nreSocketSend(socket, message.toAscii());
         *m_stream<<message;
         return QString(response);
    }


    }
}



QStringList ApexNreSocketCreator::availablePlugins() const {
    return QStringList("l34plugin");
}


apex::stimulus::ApexNreSocketInterface *ApexNreSocketCreator::create (const QString& host, int p_deviceNr ) const{
    return new apex::stimulus::ApexNreSocket(host,p_deviceNr);
}
