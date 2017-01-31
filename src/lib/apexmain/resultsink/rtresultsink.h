//
// C++ Interface: rtresultsink
//
// Description:
//
//
// Author: Tom Francart,,, <tom.francart@med.kuleuven.be>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RTRESULTSINK_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RTRESULTSINK_H_

#include "apextools/global.h"

#include <QString>
#include <QObject>
#include <QUrl>
#include <QWidget>
#include <QWebFrame>

class QWebView;

namespace apex {

    class ApexScreenResult;
    //class ExperimentRunDelegate;
    class RTResultSinkPrivate;


/**
        @author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
class APEX_EXPORT RTResultSink:
    public QObject{

    Q_OBJECT
public:
    /**
     * If webview is 0, RTResultSink will create a webview and take ownership
     * of it. Otherwise it will not take ownership
     */
    RTResultSink(QUrl page, QMap<QString,QString> resultParameters = QMap<QString, QString>(), QString extraScript =  0, QWebView* webview=0);

    ~RTResultSink();

    void show(bool visible=true);

    /**
     * @brief
     * @return HTML code for current state of page
     */
    QString currentHtml() const;

    QVariant evaluateJavascript(QString script);

    QWebFrame* mainWebFrame() const;     // needed for unit test

    //void newAnswer ( const ApexScreenResult* r, bool correct);

    void setJavascriptParameters (QMap<QString,QString> resultParameters) const;
    void executeJavaScript (QString JScode) const;

public slots:
    //! Add a single <trial> block to the results
    void newAnswer( QString xml, bool doPlot=true);
    void plot();

    //! Add a whole results file to the results <apex:results>
    void newResults ( QString xml );

protected:
    //ExperimentRunDelegate& m_rd;
    RTResultSinkPrivate* d;

};

}

#endif
