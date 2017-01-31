/*****************************************************************************
* Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "screenstatusreporter.h"
#include "statuswindow.h"

namespace apex
{

ScreenStatusReporter::ScreenStatusReporter() :
                                window(new StatusWindow(reportLevels()))
{
    connect(window, SIGNAL(levelsChanged(StatusItem::Levels)),
            this, SLOT(setReportLevels(StatusItem::Levels)));
}

ScreenStatusReporter::~ScreenStatusReporter()
{
    delete window;
}

void ScreenStatusReporter::clear()
{
    StatusReporter::clear();
    window->clearList();
}

void ScreenStatusReporter::report(const apex::StatusItem& e)
{
    window->addItem(&e);

    if (e.level() >= StatusItem::Error)
        window->raise();
}

void ScreenStatusReporter::setReportLevels(StatusItem::Levels levels)
{
    window->clearList();
    StatusReporter::setReportLevels(levels);
    rereport();
}

void ScreenStatusReporter::showWindow()
{
    window->show();
}

void ScreenStatusReporter::hideWindow()
{
    window->hide();
}

}

