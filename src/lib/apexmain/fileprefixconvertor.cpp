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

#include "apexdata/fileprefix.h"

#include "apextools/apextools.h"

#include "parser/prefixparser.h"

#include "fileprefixconvertor.h"
#include "mainconfigfileparser.h"

#include <QFileInfo>
#include <QUrl>

using apex::data::FilePrefix;

QString apex::FilePrefixConvertor::convert(data::FilePrefix p)
{
    if (p.value().isEmpty())
        return QString();

    if (p.type() == FilePrefix::PREFIX_INLINE)
        return p.value().trimmed();

    if (p.type() == FilePrefix::PREFIX_MAINCONFIG) {
        QString result(MainConfigFileParser::Get().data().prefix(p.value()));
        if (result.isEmpty()) {
            qCWarning(APEX_RS, "%s",
                      qPrintable(QSL("%1: %2").arg(
                          "PrefixParser", tr("Prefix "
                                             "with ID %1 not found in main "
                                             "config file, trying with empty "
                                             "prefix")
                                              .arg(p.value()))));
        }
        return result;
    }

    return QString();
}

QString apex::FilePrefixConvertor::addPrefix(data::FilePrefix p,
                                             const QString &file)
{
    return ApexTools::addPrefix(file, FilePrefixConvertor::convert(p));
}
