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
 
#include "fileprefixconvertor.h"
#include "parser/prefixparser.h"
#include "services/mainconfigfileparser.h"
#include "services/errorhandler.h"
#include "fileprefix.h"
#include "apextools.h"

using apex::data::FilePrefix;

QString apex::FilePrefixConvertor::convert(data::FilePrefix p)
{
    QString prefix;
    
    if (p.value().isEmpty())
        return QString();
    
    if (p.type()==FilePrefix::PREFIX_INLINE) {
        prefix = p.value().trimmed();
    } else if (p.type()==FilePrefix::PREFIX_MAINCONFIG) {
        const QString result (MainConfigFileParser::Get().
                GetPrefix (p.value()));
        if (result.isEmpty())
            ErrorHandler::Get().addItem( StatusItem(StatusItem::Warning, "PrefixParser",
                              QString(tr("Prefix with ID %1 not found in main config file, trying with empty prefix")).arg(p.value())));
        prefix = result;
    } else {
        Q_ASSERT (0 && "invalid attribute value");
    }

    ApexTools::MakeDirEnd(prefix);
    return prefix;
}


QUrl apex::FilePrefixConvertor::addPrefix(data::FilePrefix p,
                                          const QUrl& file)
{
    if (file.isRelative()) {
        QString modprefix = FilePrefixConvertor::convert( p );
        if (!modprefix.isEmpty() && !modprefix.endsWith('/'))
            modprefix += '/';
        return( QUrl(modprefix + file) );
    }
    else
        return ( file );

}

