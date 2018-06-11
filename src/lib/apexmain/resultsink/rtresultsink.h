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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RTRESULTSINK_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RTRESULTSINK_H_

#include "apextools/global.h"

#include "commongui/webview.h"
#include "common/paths.h"

#include <QObject>
#include <QString>
#include <QUrl>
#include <QWidget>

namespace apex
{

class ApexScreenResult;
class RTResultSinkPrivate;

class APEX_EXPORT RTResultSink : public QObject
{

    Q_OBJECT
public:
    RTResultSink(QUrl page, QMap<QString, QString> resultParameters =
                                QMap<QString, QString>(),
                 QString extraScript = 0);

    ~RTResultSink();

    QVariant evaluateJavascript(QString script);
    void setJavascriptParameters(QMap<QString, QString> resultParameters) const;
    void executeJavaScript(QString JScode) const;

    static QByteArray createCSVtext(const QString &resultFilePath);

public Q_SLOTS:
    void show();
    void hide();

    void newAnswer(QString xml, bool doPlot = true);
    void plot();

    void newResults(QString xml);

    void trialStarted();
    void stimulusStarted();
    void newStimulusParameters(const QVariantMap &params);

Q_SIGNALS:
    void loadingFinished(bool ok);
    void viewClosed();

protected:
    RTResultSinkPrivate *d;

private Q_SLOTS:
    void setup();
    void exportToPdf();
};
}

#endif
