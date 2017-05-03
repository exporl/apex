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

#include "apexdata/result/resultparameters.h"

#include "apextools/apextools.h"

#include "apextools/apexpaths.h"

#include "parser/xmlpluginapi.h"

#include "resultsink/apexresultsink.h"
#include "resultsink/rtresultsink.h"

#include "accessmanager.h"

#include "resultviewer.h"
#include "ui_resultviewer.h"
#include "webpageprocessor.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFile>
#include <QFileDialog>
#include <QHash>
#include <QIODevice>
#include <QMessageBox>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#include <QXmlQuery>
#include <QXmlStreamReader>

#include <iostream>
#include <sstream>

static const char* ERROR_SOURCE = "ResultViewer";

using namespace apex;

ResultViewer::ResultViewer(const data::ResultParameters* p_param,
        const QString& p_resultfile):
    m_pParam(p_param),
    m_sResultfile(p_resultfile),
    m_rtr(0),
    m_dialog(0),
    m_ui(0)
{
}

ResultViewer::~ResultViewer()
{
}

QByteArray ResultViewer::createCSVtext()
{
    //This function should run the javascript script and put
    //the output in a variable so it can be added to a file later.
    //Load the file with the results
    //Open the xml file and load the contents
    QFile resultsfile(m_sResultfile);
    resultsfile.open(QIODevice::ReadOnly);
    if(!resultsfile.isOpen()){
        Q_EMIT errorMessage(ERROR_SOURCE, "resultsfile could not be loaded");
        return QByteArray();
    }
    QString xmlString = resultsfile.readAll();

    //Load the html page with accessmanager:
    QUrl p_page("apex:apr2csv.html");
    QPointer<QWebView> webView = new QWebView();
    QPointer<WebPageProcessor> page = new WebPageProcessor(webView);
    QPointer<AccessManager> am = new AccessManager(page);
    page->setNetworkAccessManager(am);
    page->mainFrame()->load( am->prepare(p_page) );

    //Wait for the page to load the javascript files:
    QEventLoop loop;
    QTimer *timeoutTimer = new QTimer();

    connect(page,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
    //Set timeout in case page fails to load...
    //Should an error message be givven here?
    connect(timeoutTimer,SIGNAL(timeout()), &loop,SLOT(quit()));
    timeoutTimer->start(5000); //5 second timeout

    loop.exec();

    QVariant jsResult = page->mainFrame()->evaluateJavaScript("process(\"" +
                                                              ApexTools::escapeJavascriptString(xmlString)
                                                              + "\")");
    return jsResult.toByteArray();
}

bool ResultViewer::findResultPage()
{

    // read resultpage from XML file
    QFile resultsfile(m_sResultfile);
    if (!resultsfile.exists())
        qCDebug(APEX_RS, "Resultsfile does not exist");
    if( resultsfile.open(QIODevice::ReadOnly) ) {
        QXmlStreamReader xsr( &resultsfile );
        if (xsr.error() != QXmlStreamReader::NoError)
            qCDebug(APEX_RS, "XMLStreamReader error: %s", qPrintable(xsr.errorString()));
        while (!xsr.atEnd()) {
            xsr.readNext();
            if (xsr.error() != QXmlStreamReader::NoError)
                qCDebug(APEX_RS, "XMLStreamReader error: %s", qPrintable(xsr.errorString()));
            //qCDebug(APEX_RS) << xsr.name();
            if (xsr.isStartElement() && xsr.name() == "jscript") {
                if (xsr.readNext() == QXmlStreamReader::Characters) {
                    m_resultPagePath = xsr.text().toString();
                    qCDebug(APEX_RS) << "Found results page: " << m_resultPagePath;
                    return true;
                }
            }
        }
    }

    // find name of result page
    if (! m_pParam->resultPage().isEmpty()) {
        m_resultPagePath = m_pParam->resultPage();
        return true;
    }

    return false;
}


void ResultViewer::show(bool ask)
{
    if (ask) {
        if (QMessageBox::question(0, tr("ResultViewer"),
                tr("Would you like to see an analysis of the results?"),
                QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        return;
    }

    if (!QFile::exists(m_sResultfile)) {
        qCDebug(APEX_RS, "Results file not found %s", qPrintable (m_sResultfile));
        QMessageBox::warning(0, tr("Results file not found"),
                             tr("Processed results can only be shown if you save the "
                             "results, not processing."), QMessageBox::Abort,
                             QMessageBox::NoButton);
        return;
    }

    findResultPage();
    showJavascript();

    return;
}

void ResultViewer::setupDialog()
{
    m_dialog = new QDialog;
    m_ui = new Ui_ResultViewer;
    m_ui->setupUi(m_dialog);
    connect(m_ui->buttonExport, SIGNAL(clicked(bool)),
            this, SLOT(exportToPdf()));
    connect(m_ui->buttonPrint, SIGNAL(clicked(bool)),
            this, SLOT(print()));
}

void ResultViewer::showDialog()
{
    m_dialog->exec();
    delete(m_ui);
    delete(m_dialog);
}

void ResultViewer::showJavascript()
{
    setupDialog();
    connect(m_ui->webView, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));

    m_dialog->setWindowTitle(tr("APEX Results - %1").
        arg(QFileInfo(m_sResultfile).fileName()) );

    m_rtr = new RTResultSink( m_resultPagePath, m_pParam->resultParameters(), m_pParam->extraScript(), m_ui->webView);
   // m_rtr->setJavascriptParameters(m_pParam->resultParameters());   // input is a QMap found as member mResultParameters in structure Resultparameters
   // m_rtr->executeJavaScript(m_pParam->extraScript());         // input is a Qstring found as member m_extraScript in structure ResultParameters
    showDialog();
    delete(m_rtr);
}

void ResultViewer::loadFinished(bool ok)
{
    if (!ok) {
        m_dialog->close();
        Q_EMIT errorMessage(ERROR_SOURCE, tr("ResultViewer: cannot load results page"));
        return;
    }

    QFile resultsfile(m_sResultfile);
    if (!resultsfile.exists())
        qCDebug(APEX_RS, "Resultsfile does not exist");
    if( resultsfile.open(QIODevice::ReadOnly) ) {

        QTextStream stream(&resultsfile);
        stream.setCodec("UTF-8");

        m_rtr->newResults( stream.readAll() );
        m_rtr->plot();
        qCDebug(APEX_RS, "Showing rtresultsink");
    }
}

void ResultViewer::exportToPdf()
{
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
    QByteArray result_csv = createCSVtext();

    QFile f(p_filename);
    if (!f.open(QFile::ReadWrite)) {
        QMessageBox::warning(0, tr("Can't open file for append"), tr("Can't "
                "open resultfile for appending processed results, discarding.\n"
                "You can reprocess them using an XSLT processor"),
                QMessageBox::Ok, QMessageBox::NoButton);
        qCDebug(APEX_RS, "File %s not found", qPrintable (p_filename));
        return false;
    }

    QTextStream out(&f);
    out.setCodec("UTF-8");

    //see if the "processed" tags are allready present.
    QByteArray textData = f.readAll();
    QString fileText(textData);

    int processedIndex = fileText.lastIndexOf("<processed>");

    if(processedIndex < 0)
    {
        // overwrite the </apex:results> thing
        // -1 to account for windows cr/lf
        f.seek(f.size() - ApexResultSink::c_fileFooter.length() - 1);
        out << "<processed>" << endl << endl;
        out << result_csv;
    }
    else
    {
        f.seek(processedIndex+11);
        out << result_csv;
    }

    out << endl << "</processed>" << endl;
    out << ApexResultSink::c_fileFooter;

    f.close();

    return true;
}



