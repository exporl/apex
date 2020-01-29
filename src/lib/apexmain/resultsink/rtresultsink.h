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

#include <QObject>
#include <QString>
#include <QUrl>

namespace apex
{

class RTResultSinkPrivate;

class APEX_EXPORT RTResultSink : public QObject
{
    Q_OBJECT
public:
    RTResultSink(QUrl url, QMap<QString, QString> resultParameters =
                               QMap<QString, QString>(),
                 QString extraScript = 0);
    ~RTResultSink();

    cmn::WebView &getWebView() const;
    void newResults(QString xml);

public Q_SLOTS:
    void show();
    void hide();
    void newAnswer(QString xml, bool doPlot = true);
    void trialStarted();
    void stimulusStarted();
    void newStimulusParameters(const QVariantMap &params);

Q_SIGNALS:
    void loadingFinished(bool ok);
    void viewClosed();

private:
    RTResultSinkPrivate *d;
    void plot();
    void runJavaScript(const QString &script) const;
    void setJavascriptParameters(QMap<QString, QString> resultParameters) const;

private Q_SLOTS:
    void print();
};
}

#endif
