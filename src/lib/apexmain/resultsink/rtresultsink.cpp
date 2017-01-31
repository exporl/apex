//
// C++ Implementation: rtresultsink
//
// Description: 
//
//
// Author: Tom Francart,,, <tom.francart@med.kuleuven.be>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rtresultsink.h"
//#include "result/resultparameters.h"
#include "runner/experimentrundelegate.h"
#include "services/paths.h"
#include "services/errorhandler.h"
#include "experimentdata.h"
#include "apextools.h"


#include <QWebView>
#include <QWebFrame>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkReply>
#include <QXmlStreamReader>

namespace apex {

    class RTRWebPage: public QWebPage
    {
        public:
        RTRWebPage( QObject * parent = 0 );

        virtual void javaScriptConsoleMessage ( const QString & message,
                        int lineNumber, const QString & sourceID );
    };

    RTRWebPage::RTRWebPage( QObject * parent ):
            QWebPage(parent)
    {

    }

    void RTRWebPage::javaScriptConsoleMessage
            ( const QString & message,
              int lineNumber,
              const QString & sourceID )
    {
        QString m (
            tr("Javascript error at line %1 of source %2: %3"));
        m=message.arg(lineNumber).arg(sourceID).arg(message);
                         
        qDebug() << m;
        ErrorHandler::Get().addWarning("RealtimeResultSink", m );
        QWebPage::javaScriptConsoleMessage( message, lineNumber, sourceID );
    }

    class RTRNetworkAccessManager:
            public QNetworkAccessManager
    {
        public:
            RTRNetworkAccessManager(QObject * parent = 0 );

        protected:
            virtual QNetworkReply * createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
    };

    RTRNetworkAccessManager::RTRNetworkAccessManager(QObject * parent ):
            QNetworkAccessManager(parent)
    {

    }


    QNetworkReply * RTRNetworkAccessManager::createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData )
    {
        QNetworkRequest newreq(req);
        
        // if scheme "apex" is used, try to find the file in some apex data paths
        if ( req.url().scheme() == "apex") {
            QStringList dirs;
            dirs << "";
            dirs << Paths::Get().GetScriptsPath();
            dirs << Paths::Get().GetScriptsPath() + "/external"; 
            dirs << Paths::Get().GetNonBinaryPluginPath();

            bool found=false;
            QUrl newUrl(req.url());
            if (! QFile::exists(req.url().path())) {
                QFileInfo fi(req.url().path());
                Q_FOREACH(const QString &prefix, dirs) {
                    QString newpath(prefix + "/" + fi.fileName());
                    if ( QFile::exists( newpath )) {
                        newUrl.setPath( newpath );
                        found=true;
                        break;
                    }
                }
            } else {
                found=true;
            }

            if (!found)
                ErrorHandler::Get().addWarning("RealtimeResultSink",
                        tr("Resource not found: %1").arg(req.url().toString()));
                
            newUrl.setScheme("file");
            newreq.setUrl( newUrl );
        }

        qDebug() << "createRequest " << newreq.url();

        /*QNetworkReply* reply = QNetworkAccessManager::createRequest( op, newreq, outgoingData );
        qDebug() << "network error: " << reply->error();
        return reply;*/
        return QNetworkAccessManager::createRequest( op, newreq, outgoingData );
    }


    class RTResultSinkPrivate {
        public:
            RTResultSinkPrivate(QWebView* wv);
            ~RTResultSinkPrivate();            
            QWebView* webView;
            bool weOwnWebView;
            //data::ResultParameters const* params;
    };


    RTResultSinkPrivate::RTResultSinkPrivate(QWebView* wv)
    {
        if (wv) {
            webView = wv;
            weOwnWebView=false;
        } else {
            webView=new QWebView();
            weOwnWebView=true;
        }
    }

    RTResultSinkPrivate::~RTResultSinkPrivate()
    {
        if (weOwnWebView)
            delete webView;
    }



/*RTResultSink::RTResultSink(const data::ResultParameters* const p_rp)
    
{
    RTResultSink( p_rp->resultPage());
}*/

RTResultSink::RTResultSink(const QUrl& p_page, QWebView* webview):
        d(new RTResultSinkPrivate(webview))
{
    //d->webView->load( m_rd.GetData().resultParameters()->resultPage());

    
    /*QNetworkDiskCache* cache = new QNetworkDiskCache(nam);
    cache->setCacheDirectory( Paths::Get().GetScriptsPath() + "/cache");
    nam->setCache(cache);*/
    d->webView->setWindowTitle(tr("Real-time results - APEX"));
    
    QWebPage* page = new RTRWebPage(d->webView);  // webView is parent, so the page will be deleted when webView is deleted
    QNetworkAccessManager* nam = new RTRNetworkAccessManager(page);
    page->setNetworkAccessManager(nam);
    qDebug() << "loading page " << p_page;
    page->mainFrame()->load( p_page );
    d->webView->setPage(page);
    Q_ASSERT(d->webView!=0);
}


RTResultSink::~RTResultSink()
{
    delete d;
}

void RTResultSink::show(bool visible)
{
    d->webView->setVisible(visible);
}

void RTResultSink::newAnswer (QString xml)
{
    qDebug("RTResultSink::newAnswer");

    QString code( "newAnswer(\"" + ApexTools::escapeJavascriptString(xml)
            + "\"); plot();");
    
    QVariant jr = d->webView->page()->mainFrame()->evaluateJavaScript(
                                   code );

    qDebug() << code;
    qDebug() << "Javascript result: " << jr;

}

void RTResultSink::newResults ( QString xml )
{
    qDebug("RTResultSink::newResults");

    /*QDomDocument doc("results");
    doc.setContent(xml);
    QDomElement docElem = doc.documentElement();
    
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull() && e.tagName()=="trial") {
            part = e.to
        }
        n = n.nextSibling();
    }*/



    QXmlStreamReader xsr( xml );
    if (! xsr.error() == QXmlStreamReader::NoError)
        qDebug("XMLStreamReader error: %s", qPrintable(xsr.errorString()));
    while (!xsr.atEnd()) {
        qint64 start = xsr.characterOffset();
        xsr.readNext();
        if (! xsr.error() == QXmlStreamReader::NoError)
            qDebug("XMLStreamReader error: %s", qPrintable(xsr.errorString()));
        //qDebug() << xsr.name();
        if (xsr.isStartElement() && xsr.name() == "trial") {
            while (! (xsr.isEndElement() && xsr.name() == "trial")) {
                xsr.readNext();
            }
            xsr.readNext();
            qint64 end = xsr.characterOffset();
            
            QString text(xml.mid(start-1, end-start-1));
            //qDebug("* sending");
            //qDebug() << text;
            newAnswer( text );
        }
    }

}


}
