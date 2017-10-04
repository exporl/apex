/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
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

#include "resultapi.h"

#include "psignifit/psignifitwrapper.h"

namespace apex
{

class ResultApiPrivate
{
public:
    QScopedPointer<PsignifitWrapper> psignifitWrapper;
};

ResultApi::ResultApi() : d(new ResultApiPrivate)
{
    d->psignifitWrapper.reset(new PsignifitWrapper);
}

ResultApi::~ResultApi()
{
    delete d;
}

QVariant ResultApi::psignifit(const QString &data)
{
    return QVariant(d->psignifitWrapper->psignifit(data));
}
}
