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

#ifndef _EXPORL_SRC_LIB_APEXDATA_INTERACTIVE_PARAMETERDIALOGRESULTS_H_
#define _EXPORL_SRC_LIB_APEXDATA_INTERACTIVE_PARAMETERDIALOGRESULTS_H_

#include "apextools/global.h"

#include <QList>

namespace apex
{
namespace data
{

struct ParameterDialogResultPrivate;

/**
 * @author Tom Francart,,, <tom.francart@med.kuleuven.be>
 */
class APEXDATA_EXPORT ParameterDialogResult
{
public:
    ParameterDialogResult();
    ParameterDialogResult(const ParameterDialogResult &other);
    ParameterDialogResult(const QString &xpath, const QString &description,
                          const QString &newValue, bool succeeded);

    // getters

    QString xpath() const;
    QString description() const;
    QString newValue() const;
    bool succeeded() const;

    ParameterDialogResult &operator=(const ParameterDialogResult &other);
    bool operator==(const ParameterDialogResult &other) const;
    bool operator<(const ParameterDialogResult &other) const;

private:
    ParameterDialogResultPrivate *d;
};

class APEXDATA_EXPORT ParameterDialogResults
    : public QList<ParameterDialogResult>
{
public:
    ParameterDialogResults();
    ~ParameterDialogResults();

    bool operator==(const ParameterDialogResults &other) const;
};
}
}

DUMMY_QHASH_FUNCTION(apex::data::ParameterDialogResult);

#endif
