/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "xmlutils.h"

namespace cmn
{

QString xmlIndent(unsigned depth, unsigned indentSize)
{
    return QString(indentSize * depth, QL1C(' '));
}

QString xmlEscapedText(const QString &text)
{
    QString result = text;
    result.replace(QL1S("&"), QL1S("&amp;"));
    result.replace(QL1S("<"), QL1S("&lt;"));
    result.replace(QL1S(">"), QL1S("&gt;"));
    return result;
}

QString xmlEscapedAttribute(const QString &text)
{
    QString result = xmlEscapedText(text);
    result.replace(QL1S("\""), QL1S("&quot;"));
    result.prepend(QL1S("\""));
    result.append(QL1S("\""));
    return result;
}

} // namespace cmn
