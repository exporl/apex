/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_SIMPLERUNNER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_SIMPLERUNNER_H_

#include "experimentrunner.h"

#include "../resultsink/resultviewer.h"
#include "flowrunner.h"

namespace apex
{

class SimpleRunner : public ExperimentRunner
{
    Q_OBJECT
public:
    SimpleRunner();
    virtual bool select(const QString &path) Q_DECL_OVERRIDE;
    void selectFromDir(const QString &path) Q_DECL_OVERRIDE;
    virtual void makeVisible() Q_DECL_OVERRIDE;

public slots:
    void setExpressions(const QMap<QString, QString> &expressions);

private:
    QString path;
    QScopedPointer<FlowRunner> flowRunner;
    QScopedPointer<ResultViewer> resultViewer;

signals:
    void errorMessage(const QString &source, const QString &message);
    void setResultsFilePath(const QString &filePath);
    void savedFile(const QString &filePath);
    void experimentClosed();
};
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_SIMPLERUNNER_H_
