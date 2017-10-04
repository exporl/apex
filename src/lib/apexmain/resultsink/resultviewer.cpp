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

#include "../accessmanager.h"
#include "../apexcontrol.h"
#include "../gui/mainwindow.h"
#include "../resultsink/apexresultsink.h"
#include "../resultsink/rtresultsink.h"

#include "apexdata/result/resultparameters.h"

#include "apextools/apextools.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>
#include <QXmlQuery>
#include <QXmlStreamReader>

namespace apex
{

ResultViewer::ResultViewer(const data::ResultParameters *p_param,
                           const QString &p_resultfile)
    : m_pParam(p_param), m_sResultfile(p_resultfile)
{
    if (!QFile::exists(m_sResultfile)) {
        qCWarning(APEX_RS, "Results file not found %s",
                  qPrintable(m_sResultfile));
        return;
    }
    findResultPage();
    rtResultSink.reset(new RTResultSink(m_resultPagePath,
                                        m_pParam->resultParameters(),
                                        m_pParam->extraScript()));
    connect(rtResultSink.data(), SIGNAL(loadingFinished(bool)), this,
            SLOT(loadingFinished(bool)));
    connect(rtResultSink.data(), SIGNAL(viewClosed()), this,
            SIGNAL(viewClosed()));
}

ResultViewer::~ResultViewer()
{
}

void ResultViewer::show(bool ask)
{
    if (ask) {
        if (QMessageBox::question(
                0, tr("ResultViewer"),
                tr("Would you like to see an analysis of the results?"),
                QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
            return;
    }

    if (!QFile::exists(m_sResultfile)) {
        qCDebug(APEX_RS, "Results file not found %s",
                qPrintable(m_sResultfile));
        QMessageBox::warning(
            0, tr("Results file not found"),
            tr("Processed results can only be shown if you save the "
               "results, not processing."),
            QMessageBox::Abort, QMessageBox::NoButton);
        return;
    }
    rtResultSink->show();
}

bool apex::ResultViewer::addtofile(const QString &p_filename)
{
    QByteArray result_csv = RTResultSink::createCSVtext(m_sResultfile);

    QFile f(p_filename);
    if (!f.open(QFile::ReadWrite)) {
        qCDebug(APEX_RS, "File %s not found", qPrintable(p_filename));
        QMessageBox::warning(
            0, tr("Can't open file for append"),
            tr("Can't "
               "open resultfile for appending processed results, discarding.\n"
               "You can reprocess them using an XSLT processor"),
            QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }

    QTextStream out(&f);
    out.setCodec("UTF-8");

    // see if the "processed" tags are allready present.
    QByteArray textData = f.readAll();
    QString fileText(textData);

    int processedIndex = fileText.lastIndexOf("<processed>");

    if (processedIndex < 0) {
        // overwrite the </apex:results> thing
        // -1 to account for windows cr/lf
        f.seek(f.size() - ApexResultSink::c_fileFooter.length() - 1);
        out << "<processed>" << endl << endl;
        out << result_csv;
    } else {
        f.seek(processedIndex + 11);
        out << result_csv;
    }

    out << endl << "</processed>" << endl;
    out << ApexResultSink::c_fileFooter;

    f.close();

    return true;
}

bool ResultViewer::findResultPage()
{
    // read resultpage from XML file
    QFile resultsfile(m_sResultfile);
    if (!resultsfile.exists())
        qCDebug(APEX_RS, "Resultsfile does not exist");
    if (resultsfile.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xsr(&resultsfile);
        if (xsr.error() != QXmlStreamReader::NoError)
            qCDebug(APEX_RS, "XMLStreamReader error: %s",
                    qPrintable(xsr.errorString()));
        while (!xsr.atEnd()) {
            xsr.readNext();
            if (xsr.error() != QXmlStreamReader::NoError)
                qCDebug(APEX_RS, "XMLStreamReader error: %s",
                        qPrintable(xsr.errorString()));
            if (xsr.isStartElement() && xsr.name() == "jscript") {
                if (xsr.readNext() == QXmlStreamReader::Characters) {
                    m_resultPagePath = xsr.text().toString();
                    qCDebug(APEX_RS) << "Found results page: "
                                     << m_resultPagePath;
                    return true;
                }
            }
        }
    }

    // find name of result page
    if (!m_pParam->resultPage().isEmpty()) {
        m_resultPagePath = m_pParam->resultPage();
        return true;
    }

    return false;
}

void ResultViewer::loadingFinished(bool ok)
{
    if (!ok) {
        qCWarning(APEX_RS, "ResultViewer: cannot load results page");
        rtResultSink->hide();
        return;
    }

    QFile resultsfile(m_sResultfile);
    if (resultsfile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&resultsfile);
        stream.setCodec("UTF-8");
        rtResultSink->newResults(stream.readAll());
        rtResultSink->plot();
    } else {
        qCWarning(APEX_RS, "Couldn't open resultsfile %s",
                  qPrintable(resultsfile.fileName()));
    }
}
}
