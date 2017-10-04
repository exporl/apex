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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULT_RESULTAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULT_RESULTAPI_H_

#include <QVariant>

namespace apex
{
class ResultApiPrivate;

class ResultApi : public QObject
{
    Q_OBJECT
public:
    ResultApi();
    ~ResultApi();

public Q_SLOTS:
    QVariant psignifit(const QString &data);

Q_SIGNALS:
    void exportToPdf();

private:
    ResultApiPrivate *d;
};
}

#endif
