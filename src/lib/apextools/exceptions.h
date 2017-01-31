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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_EXCEPTIONS_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_EXCEPTIONS_H_

#include "global.h"

#include <exception>

#include <QString>

class APEXTOOLS_EXPORT ApexException : public std::exception {
};

class APEXTOOLS_EXPORT ApexStringException: public ApexException {
public:
    ApexStringException (const QString &message);

    //QString what() const;

    const char *what() const throw();

    ~ApexStringException() throw ();

private:
    const QByteArray message;
};

class APEXTOOLS_EXPORT TrialDataNotFoundException: public ApexException
{
public:
    const char *what() const throw();
};

class APEXTOOLS_EXPORT ApexConnectionBetweenDifferentDevicesException:
    public ApexStringException
{
public:
    ApexConnectionBetweenDifferentDevicesException (const QString& message);
};

class APEXTOOLS_EXPORT XercesException: public ApexException
{
public:
    const char *what() const throw();
};

class APEXTOOLS_EXPORT ParseException: public ApexException
{
public:
    const char *what() const throw();
};

#endif

