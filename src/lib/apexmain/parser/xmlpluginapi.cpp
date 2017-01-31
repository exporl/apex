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
 
#include "xmlpluginapi.h"
#include "services/errorhandler.h"

#include <QDir>
#include <QDebug>

namespace apex {

QString XMLPluginAPI::version()
{
    return "This is a version string";
}

QStringList XMLPluginAPI::files(QString path)
{
    QString p(path);

    QDir d(p);
    if (d.exists())            // there is no wildcard specified
        return d.entryList();
    else {
        // assume the last part of the path is the wildcard
        QStringList parts( QDir::fromNativeSeparators(p).split("/") );
        QString wildcard( parts.at(parts.size()-1) );

        if ( p.length()==wildcard.length() )
            d.setPath(d.currentPath());
        else
            d.setPath( p.left( p.length()-wildcard.length()));
        d.setNameFilters( QStringList()<< wildcard );

        qDebug() << d.entryList();

        return d.entryList();
    }
    
    /*QDir dir(path.toString(), wildcard.toString());

    qDebug("found %d files in path %s:",
        dir.entryList().size(),
                      qPrintable(path.toString()));
    return dir.entryList();*/
}




QString XMLPluginAPI::stripPath(QString s)
{
    QDir d(s);
    if (d.exists())
        return s;
    
    QStringList parts( QDir::fromNativeSeparators(s).split("/") );
    QString wildcard( parts.at(parts.size()-1) );

    return s.left( s.length()-wildcard.length());

}



void XMLPluginAPI::addWarning(const QString & warning)
{
    ErrorHandler::Get().addWarning( "XML Plugin script",
                      warning);
}

void XMLPluginAPI::addError(const QString & warning)
{
    ErrorHandler::Get().addError( "XML Plugin script",
                      warning);
}

}

