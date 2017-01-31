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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_STATUS_SCREENSTATUSREPORTER_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_STATUS_SCREENSTATUSREPORTER_H_

#include "statusreporter.h"

#include <QObject>

namespace apex
{

class StatusWindow;

/**
 * @class ScreenStatusReporter
 * @author Job Noorman
 *
 * This class reports its status to the screen using a StatusWindow.
 */
class APEXTOOLS_EXPORT ScreenStatusReporter : public QObject, public StatusReporter
{
        Q_OBJECT

    public:

        ScreenStatusReporter();
        ~ScreenStatusReporter();

        void clear();

    public Q_SLOTS:

        void setReportLevels(StatusItem::Levels levels);
        void showWindow();
        void hideWindow();

    private:

        void report(const StatusItem& e);

        StatusWindow* window;
};

}

#endif
