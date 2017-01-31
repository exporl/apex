/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#ifndef _RBA_SRC_LIB_PLUMBING_OPTIONUTILS_H_
#define _RBA_SRC_LIB_PLUMBING_OPTIONUTILS_H_

#include "exceptions.h"
#include "global.h"

//#include <boost/any.hpp>

#include <QCoreApplication>
#include <QMap>
#include <QStringList>

#include <vector>

// To be found when used with and without "namespace rba", these functions must
// be declared outside of a namespace
//
// These functions can't be in the cpp file, otherwise gcc will instantiate
// *hidden* rtti info for the boost::any template instantiations and when used,
// a dynamic cast deep inside boost::po will fail(compiled with visibility
// hidden)
/*void validate(boost::any &v,
        const std::vector<std::string> &values, QString *, int)
{
    if (!v.empty() || values.size() != 1)
        throw ApexStringException(QCoreApplication::translate("boost::po",
                    "Unable to set a parameter more than once"));

    v = boost::any(QString::fromStdString(values[0]));
}

void validate(boost::any &v,
        const std::vector<std::string> &values, QStringList *, int)
{
    if (v.empty())
            v = boost::any(QStringList());
    QStringList *tv = boost::any_cast<QStringList> (&v);
    Q_ASSERT(tv);
    for (unsigned i = 0; i < values.size(); ++i)
        tv->append(QString::fromStdString(values[i]));
}

void validate(boost::any &v,
        const std::vector<std::string> &values, QMap<QString, QString> *, int)
{
    if (v.empty())
            v = boost::any(QMap<QString, QString>());
    QMap<QString, QString> *tv = boost::any_cast<QMap<QString, QString> > (&v);
    Q_ASSERT(tv);
    for (unsigned i = 0; i < values.size(); ++i) {
        QString param = QString::fromStdString(values[i]);
        if (!param.contains(QLatin1Char('=')))
            param = param + QLatin1String("=1");
        tv->insert(param.section(QLatin1Char('='), 0, 0),
                param.section(QLatin1Char('='), 1));
    }
}*/

namespace apex
{

APEXTOOLS_EXPORT std::vector<std::string> arguments();

} // namespace rba

#endif

