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
#include "apexdata/experimentdata.h"
#include "apextools/apextools.h"
#include "psignifit/psignifitwrapper.h"
#include "runner/experimentrundelegate.h"

#include "services/accessmanager.h"
#include "services/errorhandler.h"

#include "rtresultsink.h"

#include <QDebug>
#include <QUrl>
#include <QWebFrame>
#include <QWebView>
#include <QXmlStreamReader>

namespace apex {

    class RTRWebPage: public QWebPage
    {
        Q_OBJECT
    public:
        RTRWebPage( QObject * parent = 0 );

        virtual void javaScriptConsoleMessage ( const QString & message,
                                                int lineNumber, const QString & sourceID );

    public slots:
        void enablePsignifit();

    private:
        QScopedPointer<PsignifitWrapper> psignifitWrapper;
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

        qCDebug(APEX_RS) << m;
        ErrorHandler::Get().addWarning("RealtimeResultSink", m );
        QWebPage::javaScriptConsoleMessage( message, lineNumber, sourceID );
    }

    void RTRWebPage::enablePsignifit()
    {
        psignifitWrapper.reset( new PsignifitWrapper() );
        this->mainFrame()->addToJavaScriptWindowObject( "psignifit", psignifitWrapper.data() );
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



    RTResultSink::RTResultSink(QUrl p_page, QMap<QString,QString> resultParameters, QString extraScript, QWebView* webview):
            d(new RTResultSinkPrivate(webview))
    {
        QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        d->webView->setWindowTitle(tr("Real-time results - APEX"));

        QWebPage* page = new RTRWebPage(d->webView);  // webView is parent, so the page will be deleted when webView is deleted
        Q_ASSERT(page != 0);
        AccessManager* am = new AccessManager(page);
        Q_ASSERT(am != 0);
        page->setNetworkAccessManager(am);
        connect( page->mainFrame(),  SIGNAL(javaScriptWindowObjectCleared()),
                 page, SLOT(enablePsignifit()));

        // The QUrl for the QWebFrame expects the correct number of slashes after the scheme. p_page doesn't have this but QUrl::fromUserInput can add them (important for Windows).
        qCDebug(APEX_RS) << "RTResultSink::RTResultSink" << am->prepare(p_page);
        page->mainFrame()->load( am->prepare(p_page) );
        d->webView->setPage(page);
        d->webView->setVisible(true);
        Q_ASSERT(d->webView!=0);

        setJavascriptParameters(resultParameters);
        executeJavaScript(extraScript);

    }


    RTResultSink::~RTResultSink()
    {
        delete d;
    }

    void RTResultSink::show(bool visible)
    {
        d->webView->setVisible(visible);
    }

    void RTResultSink::setJavascriptParameters (QMap<QString,QString> resultParameters) const
    {
            if (!resultParameters.isEmpty()) {
                QString rpstring = "params = {\"";
                QMapIterator<QString,QString> it(resultParameters);
                while (it.hasNext()) {
                    it.next();

                    bool isNumber;
                    QString valuestr;
                    (it.value()).toFloat(&isNumber); // if toFloat() fails because it.value() is not a number, isNumber is set to false

                    if (!isNumber){  valuestr =  "\"" + it.value() + "\""; }
                    else{ valuestr = it.value(); }

                    rpstring += it.key() + "\": " + valuestr;

                    if(it.hasNext()){ rpstring.append(", \""); }
                    else { rpstring.append("};"); }
                }
                d->webView->page()->mainFrame()->evaluateJavaScript(rpstring);
            }
    }

    void RTResultSink::executeJavaScript(QString JScode) const
    {
        d->webView->page()->mainFrame()->evaluateJavaScript(JScode);
    }

    void RTResultSink::newAnswer (QString xml, bool doPlot)
    {
        qCDebug(APEX_RS, "RTResultSink::newAnswer");

        QString code( "newAnswer(\"" + ApexTools::escapeJavascriptString(xml)
                      + "\");");
        if (doPlot)
            code += "plot();";

        QVariant jr = d->webView->page()->mainFrame()->evaluateJavaScript(
                code );

        qCDebug(APEX_RS) << code;
        qCDebug(APEX_RS) << "Javascript result: " << jr;

    }

    void RTResultSink::plot()
    {
        d->webView->page()->mainFrame()->evaluateJavaScript(
                    "plot();" );
    }

    void RTResultSink::newResults ( QString xml )
    {
        qCDebug(APEX_RS, "RTResultSink::newResults");


        QXmlStreamReader xsr( xml );
        if (! xsr.error() == QXmlStreamReader::NoError)
            qCDebug(APEX_RS, "XMLStreamReader error: %s", qPrintable(xsr.errorString()));
        while (!xsr.atEnd()) {
            qint64 start = xsr.characterOffset();
            xsr.readNext();
            if (! xsr.error() == QXmlStreamReader::NoError)
                qCDebug(APEX_RS, "XMLStreamReader error: %s", qPrintable(xsr.errorString()));
            //qCDebug(APEX_RS) << xsr.name();
            if (xsr.isStartElement() && xsr.name() == "trial") {
                while (! (xsr.isEndElement() && xsr.name() == "trial")) {
                    xsr.readNext();
                }
                xsr.readNext();
                qint64 end = xsr.characterOffset();

                QString text(xml.mid(start-1, end-start-1));
                //qCDebug(APEX_RS, "* sending");
                //qCDebug(APEX_RS) << text;
                newAnswer( text, false );
            }
        }

    }

    QString RTResultSink::currentHtml() const
    {
        return d->webView->page()->mainFrame()->toHtml();
    }

    QVariant RTResultSink::evaluateJavascript(QString script)
    {
        return d->webView->page()->mainFrame()->evaluateJavaScript(script);
    }

    QWebFrame* RTResultSink::mainWebFrame() const
    {
        return d->webView->page()->mainFrame();
    }


}

#include "rtresultsink.moc"
