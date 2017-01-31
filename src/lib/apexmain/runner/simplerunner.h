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

#ifndef __SIMPLERUNNER_H__
#define __SIMPLERUNNER_H__

#include "experimentrunner.h"
//#include "experimentdata.h"

namespace apex
{

class ApexControl;

class SimpleRunner : public ExperimentRunner
{
        Q_OBJECT
    public:
//        SimpleRunner(ExperimentRunDelegate& p_rd): ExperimentRunner(p_rd) {};
        void Select(const QString& path);
        void SelectFromDir(const QString& path);
        void Run();
        void Finished();
    private:
        QString path;
    signals:
        void errorMessage(const QString& source, const QString& message);
};
}

#endif //#ifndef __SIMPLERUNNER_H__
