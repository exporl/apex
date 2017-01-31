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
#ifndef APEXRTRESULTSINK_H
#define APEXRTRESULTSINK_H

#include <QString>
#include <QObject>
#include <QUrl>
#include <QWidget>

class QWebView;

namespace apex {

    class ApexScreenResult;
    //class ExperimentRunDelegate;
    class RTResultSinkPrivate;


/**
	@author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
class RTResultSink:
    public QObject{
    
    Q_OBJECT
public:
    //RTResultSink(ExperimentRunDelegate& p_rd);
    //RTResultSink(const data::ResultParameters* const p_rp);

    /**
     * If webview is 0, RTResultSink will create a webview and take ownership
     * of it. Otherwise it will not take ownership
     */
    RTResultSink(const QUrl& page, QWebView* webview=0);

    ~RTResultSink();

    void show(bool visible=true);

    //void newAnswer ( const ApexScreenResult* r, bool correct);

public slots:
    //! Add a single <trial> block to the results
    void newAnswer( QString xml );

    //! Add a whole results file to the results <apex:results>
    void newResults ( QString xml );

protected:
    //ExperimentRunDelegate& m_rd;
    RTResultSinkPrivate* d;

};

}

#endif
