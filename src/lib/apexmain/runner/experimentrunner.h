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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_EXPERIMENTRUNNER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_EXPERIMENTRUNNER_H_

#include <QMap>
#include <QObject>
#include <QStringList>

// FIXME: make ApexModule
namespace apex
{
class ExperimentParser;
namespace data
{
class ExperimentData;
}

class ExperimentRunner: public QObject
{
        Q_OBJECT
    public:
        virtual void makeVisible() {}
        virtual void makeInvisible() {}
    public slots:
        /**
         * Bring the Runner in a usable state. The provided path is the
         * project file/experiment/whatever to open. If called again, it will
         * reset the instance to a virgin internal state.
         *
         * Returns false if the experiment could not be loaded successfully.
         */
        virtual bool select(const QString& name) = 0;
        /**
         * Bring the Runner in a usable state. The provided path is a suggestion
         * for the runner to use. If called again, it will reset the instance to
         * a virgin internal state.
         */
        virtual void selectFromDir(const QString& path) = 0;

    signals:
        void selected(data::ExperimentData* data);
        void opened(const QString& fileName);

    protected:
        QMap<QString, QString> expressions;
        virtual data::ExperimentData* parseExperiment(const QString& fileName);
};
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_EXPERIMENTRUNNER_H_
