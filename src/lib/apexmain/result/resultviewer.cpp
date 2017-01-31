/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "resultviewer.h"
#include "ui_resultviewer.h"

#include "../resultsink/apexresultsink.h"
#include "services/paths.h"
#include "services/mainconfigfileparser.h"

#include "xml/xercesinclude.h"
#include "xml/xalaninclude.h"

#ifdef USEXALANFORMATNUMBER
#include <xalanc/ICUBridge/ICUFormatNumberFunctor.hpp>
#endif

XALAN_USING_XERCES(XMLPlatformUtils);
XALAN_USING_XALAN(XalanTransformer);
XALAN_USING_XALAN(XSLTInputSource);
XALAN_USING_XALAN(XSLTResultTarget);

//from libdata
#include "result/resultparameters.h"
#include "resultsink/rtresultsink.h"

#include <iostream>
#include <sstream>
#include <qfile.h>
#include <qmessagebox.h>
#include <qtextedit.h>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <QDebug>
#include <QWebView>
#include <QPrinter>
#include <QPrintDialog>


using namespace apex;

namespace
{
    const unsigned sc_nBufferSize = 4096;
}

ResultViewer::ResultViewer(data::ResultParameters* p_param,
        const QString& p_resultfile, const QString& p_xsltpath):
    m_pParam(p_param),
    m_sResultfile(p_resultfile),
    m_sXsltPath(p_xsltpath),
    m_xsltOK(false),
    m_javascriptOK(false),
    m_rtr(0),
    m_dialog(0),
    m_ui(0)
{
}

ResultViewer::~ResultViewer()
{
}

/**
 * Show current results
 */
bool ResultViewer::ProcessResult()
{
    if (!QFile::exists(m_sResultfile)) {
        qDebug("Results file not found %s", qPrintable (m_sResultfile));
        QMessageBox::warning(0, tr("Results file not found"),
                             tr("Processed results can only be shown if you save the "
                             "results, not processing."), QMessageBox::Abort,
                             QMessageBox::NoButton);
                             return false;
    }


    m_xsltOK = ProcessResultXslt();
    m_javascriptOK = ProcessResultJavascript();
    return m_xsltOK || m_javascriptOK;
}


bool ResultViewer::ProcessResultXslt()
{
    Q_ASSERT (m_pParam);

    QString document(m_sResultfile);

    // find xslt script
    QString script( Paths::findReadableFile (m_pParam->GetXsltScript(),
                        QStringList() <<m_sXsltPath,
                        QStringList() << ".xsl"    ));

    qDebug("Using script: %s", qPrintable(script));


    if (!QFile::exists(script)) {
        // try to get processing instruction from results file:
        qDebug("Fetching script name from results file");
        QFile resultsfile(document);
        if (!resultsfile.exists())
            qDebug("Resultsfile does not exist");
        if( resultsfile.open(QIODevice::ReadOnly) ) {
            QXmlStreamReader xsr( &resultsfile );
            if (! xsr.error() == QXmlStreamReader::NoError)
                qDebug("XMLStreamReader error: %s", qPrintable(xsr.errorString()));
            while (!xsr.atEnd()) {
                xsr.readNext();
                if (xsr.isProcessingInstruction() &&
                    xsr.processingInstructionTarget() ==
                            QLatin1String("xml-stylesheet")) {
                        qDebug("processingInstructionData: %s", qPrintable(xsr.processingInstructionData().toString()));
                        QRegExp rx("href=\"([^\"]*)\"");
                        int pos = rx.indexIn(
                                xsr.processingInstructionData().toString());
                        if (pos>-1) {
                            script=rx.cap(1);
                            qDebug("New script: %s", qPrintable(script));

                            QString xsltOnline=MainConfigFileParser::Get().
                                        GetXsltOnlinePath();
                            if (script.startsWith(xsltOnline)) {
                                 script=script.right(
                                         script.length()-xsltOnline.length()-1);
                                 qDebug("finding readable file named %s",
                                         qPrintable(script));
                                 qDebug("xsltpath= %s", qPrintable(m_sXsltPath));
                                script=Paths::findReadableFile (script,
                                    QStringList() <<m_sXsltPath,
                                    QStringList() );
                            }
                            break;
                        } else {
                             qDebug("Regexp did not match in %s",
                                qPrintable(xsr.processingInstructionData()
                                    .toString()));
                        }
                    }
                }
            }
        }


    if (script.isEmpty() )
        return false;

backtoscript:
        // ask user
    if (!QFile::exists(script)) {
        int result = QMessageBox::warning(0, tr("Script file not found"),
                tr("The xslt script you specified in the experiment file (%1)\n"
                    " could not be found, can't process results.\n"
                    "Would you like to specify another script?").arg(script),
                QMessageBox::Ok| QMessageBox::Abort, QMessageBox::Ok);

        if (result == QMessageBox::Ok) {
            script = QFileDialog::getOpenFileName(0, tr("Open XSLT script"),
                    m_sXsltPath, tr("XSLT scripts (*.xsl)"));
            goto backtoscript;
        } else {
            return false;
        }
    }


    XalanTransformer::initialize();


    XalanTransformer transformer;
#ifdef USEXALANFORMATNUMBER
    xalanc::ICUFormatNumberFunctor(transformer.getMemoryManager());
#endif

    XSLTInputSource xmlin(QFile::encodeName(document));
    XSLTInputSource sheet(QFile::encodeName(script));

    //make target stream with char buf
    std::ostringstream ostrTarget_html;
    std::ostringstream ostrTarget_text;

    XSLTResultTarget targ_html( ostrTarget_html );


    // Setup target for pure text output
    xalanc::XalanStdOutputStream xsos (ostrTarget_text);
    xalanc::XalanOutputStreamPrintWriter xospw(xsos);
    xalanc::FormatterToText textFormatter(xospw);
    XSLTResultTarget targ_text( textFormatter );

    transformer.setStylesheetParam("target", "'html'");
    transformer.transform(xmlin,sheet,targ_html);

    transformer.setStylesheetParam("target", "'parser'");
    transformer.transform(xmlin,sheet,targ_text);

    XalanTransformer::terminate();

    //make sure buffer has an endline and construct QString with it
    ostrTarget_text << std::ends;
    m_result_html = ostrTarget_html.str().c_str();
    m_result_text = ostrTarget_text.str().c_str();

//    qDebug( m_result_text.constData() );
    return true;
}


bool ResultViewer::ProcessResultJavascript()
{
    qDebug("Trying to find result page url");
    // find name of result page
    if (! m_pParam->resultPage().isEmpty()) {
        m_resultPagePath = m_pParam->resultPage();
        return true;
    }

    // read resultpage from XML file
    QFile resultsfile(m_sResultfile);
    if (!resultsfile.exists())
        qDebug("Resultsfile does not exist");
    if( resultsfile.open(QIODevice::ReadOnly) ) {
        QXmlStreamReader xsr( &resultsfile );
        if (! xsr.error() == QXmlStreamReader::NoError)
            qDebug("XMLStreamReader error: %s", qPrintable(xsr.errorString()));
        while (!xsr.atEnd()) {
            xsr.readNext();
            if (! xsr.error() == QXmlStreamReader::NoError)
                qDebug("XMLStreamReader error: %s", qPrintable(xsr.errorString()));
            //qDebug() << xsr.name();
            if (xsr.isStartElement() && xsr.name() == "jscript") {
                if (xsr.readNext() == QXmlStreamReader::Characters) {
                    m_resultPagePath = xsr.text().toString();
                    break;
                }
            }
        }
    }

    if ( m_resultPagePath.isEmpty()) {
        qDebug("Empty result page path");
        return false;
    } else {
        qDebug()<< "Found result page " << m_resultPagePath.toString();
        return true;
    }
}

void ResultViewer::show(bool ask)
{
    if (ask) {
        if (QMessageBox::question(0, tr("ResultViewer"),
                tr("Would you like to see an analysis of the results?"),
                QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        return;
    }

    if (m_javascriptOK)
        showJavascript();

    if (m_xsltOK)
        showXslt();

    return;
}


void ResultViewer::showXslt()
{
    QDialog Window;
    Ui_ResultViewer Ui_Window;
    Ui_Window.setupUi(&Window);
   // Ui_Window.textEdit->append(m_result);
   // Ui_Window.webView->setHtml(m_result_html);
    QString contentType;
    if ( m_result_html.contains("html")) {
        if ( m_result_html.contains("xml")  )
            contentType="application/xhtml+xml";
        else
            contentType="text/html";
    } else
        contentType="text/plain";


    qDebug("HTML\n----------------");
    qDebug()<<m_result_html;
    qDebug("Text\n----------------");
    qDebug()<<m_result_text;

    Ui_Window.webView->setContent(m_result_html, contentType);
    Window.exec();



// #define WEBKIT
// #ifdef WEBKIT
//     QWebView webview;
//     webview.setHtml(m_result);
//     webview.show();
// #endif
}

void ResultViewer::showJavascript()
{
    //QDialog Window;
    m_dialog = new QDialog;
    m_ui = new Ui_ResultViewer;
    m_ui->setupUi(m_dialog);
    connect(m_ui->webView, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
    connect(m_ui->buttonExport, SIGNAL(clicked(bool)),
            this, SLOT(exportToPdf()));
    connect(m_ui->buttonPrint, SIGNAL(clicked(bool)),
            this, SLOT(print()));

    m_dialog->setWindowTitle(tr("APEX Results - %1").
        arg(QFileInfo(m_sResultfile).fileName()) );

    m_rtr = new RTResultSink( m_resultPagePath, m_ui->webView );
    m_dialog->exec();
    delete(m_rtr);
    delete(m_ui);
    delete(m_dialog);
}

void ResultViewer::loadFinished(bool ok)
{
    if (!ok)
        throw ApexStringException(tr("ResultViewer: cannot load results page"));
    // TODO: set script name

    //    rtr.show();
    QFile resultsfile(m_sResultfile);
    if (!resultsfile.exists())
        qDebug("Resultsfile does not exist");
    if( resultsfile.open(QIODevice::ReadOnly) ) {

        QTextStream stream(&resultsfile);

        m_rtr->newResults( stream.readAll() );
        qDebug("Showing rtresultsink");
        //        rtr.show();
    }
}

void ResultViewer::exportToPdf()
{
    //m_ui->webView
    QString filename( QFileDialog::getSaveFileName(0,
                tr("Export results page to PDF"),
                "", "*.pdf"));

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    m_ui->webView->print(&printer);
}

void ResultViewer::print()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, 0);
    if (printDialog.exec() != QDialog::Accepted) {
        return;
    }

    m_ui->webView->print(&printer);
}

bool apex::ResultViewer::addtofile( const QString & p_filename )
{
    QFile f(p_filename);
    //if (!f.open(IO_Append | IO_WriteOnly)) {
    if (!f.open(QFile::ReadWrite)) {
        QMessageBox::warning(0, tr("Can't open file for append"), tr("Can't "
                "open resultfile for appending processed results, discarding.\n"
                "You can reprocess them using an XSLT processor"),
                QMessageBox::Ok, QMessageBox::NoButton);
        qDebug("File %s not found", qPrintable (p_filename));
        return false;
    }

    // overwrite the </apex:results> thing
    // -1 to account for windows cr/lf
    f.seek(f.size()- ApexResultSink::c_fileFooter.length() - 1);

    QTextStream out(&f);
    out.setCodec("UTF-8");

    out << "<processed>" << endl << endl;
    out << m_result_text;
    out << endl << "</processed>" << endl;

    out << ApexResultSink::c_fileFooter;

    f.close();

    return true;
}


const QString apex::ResultViewer::GetResultHtml() const
{
    return m_result_html;
};

