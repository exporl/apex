/******************************************************************************
 * Copyright (C) 2008,2011 Michael Hofmann <mh21@piware.de>                   *
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

#ifndef _BERTHA_SRC_PROGRAMS_PTCFAST_EXCEPTION_H_
#define _BERTHA_SRC_PROGRAMS_PTCFAST_EXCEPTION_H_

#include <QByteArray>
#include <QString>

class Exception : public std::exception
{
public:
    Exception(const QString &message) throw() : message(message.toLocal8Bit())
    {
    }

    virtual ~Exception() throw()
    {
    }

    virtual const char *what() const throw()
    {
        return message;
    }

private:
    const QByteArray message;
};

#endif
