/******************************************************************************
 * Copyright (C) 2016 Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>   *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_FLOWRUNNER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_FLOWRUNNER_H_

#include "experimentrunner.h"

#include "apextools/global.h"

#include "commongui/webview.h"
#include "common/websocketserver.h"

namespace apex
{

class FlowRunnerPrivate;

class APEX_EXPORT FlowRunner : public ExperimentRunner
{
    Q_OBJECT
public:
    FlowRunner();
    ~FlowRunner();

    bool select(const QString &path) Q_DECL_OVERRIDE;
    void selectFromDir(const QString &path) Q_DECL_OVERRIDE;

    void makeVisible() Q_DECL_OVERRIDE;
    void makeInvisible() Q_DECL_OVERRIDE;

public Q_SLOTS:

    void select(data::ExperimentData *data);
    void cleanup();

Q_SIGNALS:
    void errorMessage(const QString &source, const QString &message);
    void setResultsFilePath(const QString &filepath);

private:
    FlowRunnerPrivate *d;
};
}

#endif
