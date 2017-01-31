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

#ifndef APEXNRESOCKET
#define APEXNRESOCKET

#include "apexmain/l34stimulus/apexnresocketinterface.h"

#include <QtPlugin>

#include <memory>
#include <string>

class QFile;
class QTextStream;


typedef void* NreSocketHandle;

/**
* wrapper for NreSocket
* allows L34 to be used as a plugin
*/
namespace apex
{

    namespace stimulus
    {

        class ApexNreSocket: public ApexNreSocketInterface
        {

        public:
            ApexNreSocket( const QString& host = "", int p_deviceNr=0 );
            ~ApexNreSocket(void);

            QString send( const QString& message );
            QString connect( const QString& device );
            QString receive();
            void disconnect();

        private:
            QFile* m_file;
            QTextStream* m_stream;

            NreSocketHandle socket;
        };


    }
}


class ApexNreSocketCreator:
    public QObject,
    public ApexNreSocketCreatorInterface
{
            Q_OBJECT
            Q_INTERFACES (ApexNreSocketCreatorInterface)
            Q_PLUGIN_METADATA (IID "apex.apexNreSocketCreatorInterface")
public:
    virtual ~ApexNreSocketCreator()
    {
    }

    virtual QStringList availablePlugins() const;

    // empty name creates default plugin, caller must free returned class,
    // returns NULL if impossible to create
    /**
     *
     * @param name          name of filter
     * @param channels      number of channels
     * @param blocksize     size of one block to be processed, in frames
     *                      (1 frame = 1 sample per channel)
     * @param fs            sample rate (samples/s)
     * @return
     */
    virtual apex::stimulus::ApexNreSocketInterface *
            create (const QString& host = "", int p_deviceNr=0 ) const ;
};


#endif // NRESOCKETDEBUG__
