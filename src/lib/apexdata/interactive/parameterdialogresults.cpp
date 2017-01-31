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

#include "apextools/apextools.h"

#include "parameterdialogresults.h"

namespace apex
{
namespace data
{

struct ParameterDialogResultPrivate
{
        QString xpath;
        QString description;
        QString newValue;
        bool    succeeded;
};

ParameterDialogResult::ParameterDialogResult() :
                                    d(new ParameterDialogResultPrivate())
{
}

ParameterDialogResult::ParameterDialogResult(const ParameterDialogResult& other) :
                                d(new ParameterDialogResultPrivate(*other.d))
{
}

ParameterDialogResult::ParameterDialogResult(const QString& xpath,
                                             const QString& description,
                                             const QString& newValue,
                                             const bool succeeded) :
                                        d(new ParameterDialogResultPrivate())
{
    d->xpath = xpath;
    d->description = description;
    d->newValue = newValue;
    d->succeeded = succeeded;
}


QString ParameterDialogResult::xpath() const
{
    return d->xpath;
}


QString ParameterDialogResult::description() const
{
    return d->description;
}


QString ParameterDialogResult::newValue() const
{
    return d->newValue;
}


bool ParameterDialogResult::succeeded() const
{
    return d->succeeded;
}

ParameterDialogResult& ParameterDialogResult::operator=(
                                            const ParameterDialogResult& other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}


bool ParameterDialogResult::operator<(const apex::data::ParameterDialogResult&) const
{
    Q_ASSERT(false);
    return false;
}

bool ParameterDialogResult::operator==(const ParameterDialogResult& other) const
{
    return  d->xpath == other.d->xpath &&
            d->description == other.d->description &&
            d->newValue == other.d->newValue &&
            d->succeeded == other.d->succeeded;
}

//class ParameterDialogResults

ParameterDialogResults::ParameterDialogResults()
{
}


ParameterDialogResults::~ParameterDialogResults()
{
}

bool ParameterDialogResults::operator==(const ParameterDialogResults& other) const
{
    return ApexTools::haveSameContents(*this, other);
}

}
}
