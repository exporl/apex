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
 
#ifndef __CONNECTION_H_
#define __CONNECTION_H_

#include "connection/connectiondata.h"
#include <appcore/qt_utils.h>

#include <QMap>
#include <QString>

#include <vector>

namespace apex
{
    namespace stimulus
    {

        /**
        * tConnection
        *   contains info on a connection between IDeviceFatoryElements.
        *   Not too efficient since we only keep an ID,
        *   that has to be searched for in 3 different maps
        *   (device, filter, datablock)
        *   FIXED a bit by making connections device specific
        **************************************************************** */
        struct tConnection
        {
            tConnection()
            {
            }

            tConnection (const data::ConnectionData& data) :
            m_sFromID (data.fromId()),
                m_sToID (data.toId()),
                m_nFromChannel (data.fromChannel()),
                m_nToChannel (data.toChannel()),
                m_sFromChannelID (data.fromChannelId()),
                m_sToChannelID (data.toChannelId())
            {
            }

            QString   m_sFromID;        //!< the element where the connection originates
            QString   m_sToID;          //!< the element where the connection ends
            unsigned  m_nFromChannel;   //!< the originating channel
            unsigned  m_nToChannel;     //!< the ending channel
            QString   m_sFromChannelID; //!< optional ID, used when changing connections as a variable parameter
            QString   m_sToChannelID;   //!< @see m_sFromChannelID

            /**
            * Print the connection info to a string.
            * @return the string
            */
            QString mf_sPrint() const
            {
                const QString sFrom( m_sFromChannelID + " : from " + m_sFromID + " channel " + qn( m_nFromChannel ) );
                const QString sTo( m_sToChannelID + " : to " + m_sToID + " channel " + qn( m_nToChannel ) );
                return QString( sFrom + " " + sTo );
            }
        };




        class tConnections:
            public std::vector<tConnection> {
        public:
            void SetID(QString id) { m_id=id; };
            QString GetID() const { return m_id; };
        private:
            QString m_id;
            };


        class tConnectionsMap:
            public QMap<QString, tConnections> {

            };

        typedef tConnectionsMap::iterator         tConnectionsMapIt;
        typedef tConnectionsMap::const_iterator   tConnectionsMapCIt;


    }
}

#endif //#ifndef __CONNECTION_H_
