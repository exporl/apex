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

#ifndef __EXPERIMENTRUNNER_H__
#define __EXPERIMENTRUNNER_H__

#include "apexmodule.h"
#include <QStringList>

// FIXME: make ApexModule

namespace apex
{
class ExperimentParser;
namespace data
{
class ExperimentData;
}

class ExperimentRunner: public QObject /*: public ApexModule */
{
        Q_OBJECT
    public:
        //    ExperimentRunner(ExperimentRunDelegate& p_rd): ApexModule(p_rd) {};



    public slots:
        /**
         * Bring the Runner in a usable state. The provided path is the
         * project file/experiment/whatever to open. If called again, it will
         * reset the instance to a virgin internal state.
         */
        virtual void Select(const QString& name) = 0;
        /**
         * Bring the Runner in a usable state. The provided path is a suggestion
         * for the runner to use. If called again, it will reset the instance to
         * a virgin internal state.
         */
        virtual void SelectFromDir(const QString& path) = 0;

        virtual void Run() = 0;
        virtual void Finished() = 0;

    signals:
        void Selected(data::ExperimentData* data);
        
    protected:
        
        virtual data::ExperimentData* parseExperiment(QString fileName);
};
}

#endif //#ifndef __EXPERIMENTRUNNER_H__
