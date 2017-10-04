/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "exceptions.h"

ApexStringException::ApexStringException(const QString &message)
    : message(message.toLocal8Bit())
{
}

const char *ApexStringException::what() const throw()
{
    return message;
}

const char *TrialDataNotFoundException::what() const throw()
{
    return "TrialDataNotFoundException";
}

ApexConnectionBetweenDifferentDevicesException::
    ApexConnectionBetweenDifferentDevicesException(const QString &message)
    : ApexStringException(message)
{
}

const char *ParseException::what() const throw()
{
    return "ParseException";
}
