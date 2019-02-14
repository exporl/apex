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

#include "apextools/xml/xmltools.h"

#include "common/exception.h"

#include "resultsink/apexresultsink.h"
#include "resultsink/rtresultsink.h"
#include "resultviewer.h"

#include <QDomElement>
#include <QFile>
#include <QMessageBox>

namespace apex
{

ResultViewer::ResultViewer(const QString &p_resultfile)
    : m_sResultfile(p_resultfile)
{
    if (!QFile(m_sResultfile).open(QIODevice::ReadOnly)) {
        throw cmn::Exception(
            tr("Results file not found: %1").arg(m_sResultfile));
    }

    m_rtResultSink.reset(new RTResultSink(findResultPage(m_sResultfile),
                                          findResultParameters(m_sResultfile),
                                          findExtraScript(m_sResultfile)));

    connect(m_rtResultSink.data(), SIGNAL(loadingFinished(bool)), this,
            SLOT(loadingFinished(bool)));
    connect(m_rtResultSink.data(), SIGNAL(viewClosed()), this,
            SIGNAL(viewClosed()));
}

ResultViewer::~ResultViewer()
{
}

RTResultSink *ResultViewer::getResultSink()
{
    return m_rtResultSink.data();
}

void ResultViewer::show(bool ask)
{
    if (ask &&
        QMessageBox::question(
            0, tr("ResultViewer"),
            tr("Would you like to see an analysis of the results?"),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
        Q_EMIT viewClosed();
        return;
    }

    m_rtResultSink->show();
}

bool apex::ResultViewer::addtofile(const QString &p_filename)
{
    QByteArray result_csv = RTResultSink::createCSVtext(m_sResultfile);

    QFile f(p_filename);
    if (!f.open(QFile::ReadWrite)) {
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

QString ResultViewer::findResultPage(const QString &resultFilePath)
{
    QDomElement document =
        XmlUtils::parseDocument(resultFilePath).documentElement();
    QString extraScript = document.firstChildElement("general")
                              .firstChildElement("jscript")
                              .text();
    return extraScript.isEmpty() ? "resultsviewer.html" : extraScript;
}

QMap<QString, QString>
ResultViewer::findResultParameters(const QString &resultFilePath)
{
    QMap<QString, QString> result;

    QDomElement document =
        XmlUtils::parseDocument(resultFilePath).documentElement();
    QDomElement parametersElement = document.firstChildElement("parameters");

    for (QDomElement currentNode =
             parametersElement.firstChildElement("parameter");
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement("parameter")) {

        QString name = currentNode.attribute("name");
        QString value = currentNode.text();

        result.insert(name, value);
    }

    return result;
}

QString ResultViewer::findExtraScript(const QString &resultFilePath)
{
    QDomElement document =
        XmlUtils::parseDocument(resultFilePath).documentElement();
    return document.firstChildElement("resultscript").text();
}

void ResultViewer::loadingFinished(bool ok)
{
    if (!ok) {
        qCWarning(APEX_RS, "ResultViewer: cannot load results page");
        m_rtResultSink->hide();
        return;
    }

    QFile resultsfile(m_sResultfile);
    resultsfile.open(QIODevice::ReadOnly);
    QTextStream stream(&resultsfile);
    stream.setCodec("UTF-8");
    m_rtResultSink->newResults(stream.readAll());
}
}
