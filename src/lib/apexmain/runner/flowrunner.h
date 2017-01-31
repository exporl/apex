/******************************************************************************
 * Copyright (C) 2016 Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>   *
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

#ifndef FLOWRUNNER_H
#define FLOWRUNNER_H

#include "experimentrunner.h"
#include <QWebView>

namespace apex {
    class FlowApi;

class FlowRunner : public ExperimentRunner {
    Q_OBJECT
public:
    virtual void select(const QString& path);
    virtual void selectFromDir(const QString& path);

    virtual void makeVisible();
    virtual void makeInvisible();
public slots:
    virtual void select(data::ExperimentData* data);
signals:
    void errorMessage(const QString& source, const QString& message);
    void setResultsFilePath(const QString& filepath);
    void foundExpressions(const QMap<QString, QString>& expressions);
    void savedFile(const QString& filePath);
private:
    QString path;
    QWebView *view;
    FlowApi* api;
};

} // ns apex

#endif // FLOWRUNNER_H
