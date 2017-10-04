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

#include "apexdata/experimentdata.h"

#include "apexdata/interactive/parameterdialogresults.h"

#include "apexdata/mainconfigfiledata.h"

#include "apexdata/parameters/generalparameters.h"

#include "apexdata/result/resultparameters.h"

#include "apextools/apextools.h"
#include "apextools/version.h"

#include "common/xmlutils.h"

#ifdef Q_OS_ANDROID
#include "apexandroidnative.h"
#endif

#include "gui/mainwindow.h"

#include "runner/experimentrundelegate.h"

#include "apexcontrol.h"
#include "apexresultsink.h"
#include "trialresult.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHostInfo>
#include <QMessageBox>
#include <QRegExp>
#include <QStandardPaths>

using namespace cmn;

const QString apex::ApexResultSink::c_fileFooter("</apex:results>\n");
const QString apex::ApexResultSink::resultsExtension(".apr");

void apex::ApexResultSink::SaveAs(bool askFilename)
{
#ifdef SHOWSLOTS
    qCDebug(APEX_RS, "ApexResultSink::Finished( )");
#endif

    qCDebug(APEX_RS, "ApexResultSink::SaveAs(): m_filename=%s",
            qPrintable(m_filename));
    qCDebug(APEX_RS, "Current path = %s", qPrintable(QDir::current().path()));

    if (m_bSaved) {
        qCDebug(APEX_RS, "ApexResultSink::SaveAs: not saving, already saved");
        return;
    }

    MakeFilenameUnique();

    // save data if any
    if (m_Results.size() <= 0) {
        qCDebug(APEX_RS, "Nothing to save, exiting");
    }

    bool overwrite = false;

#ifndef NOSAVE

    QString saveFileName(m_filename);

    if (m_rd.GetData().generalParameters()->GetAutoSave()) {
        askFilename = false;
    }
    while (1) {
        if (askFilename) {
            QFileDialog dlg(
                ApexControl::Get().mainWindow(), QString(), m_filename,
                tr("APEX savedata (*%1);;XML Files (*.xml);;All files (*)")
                    .arg(resultsExtension));

            dlg.setAcceptMode(QFileDialog::AcceptSave);
            QStringList docLocations = QStandardPaths::standardLocations(
                QStandardPaths::DocumentsLocation);
#ifndef Q_OS_ANDROID
            if (m_filename.isEmpty() && !docLocations.isEmpty())
#else
            if ((m_filename.isEmpty() ||
                 !QFileInfo(QFileInfo(m_filename).dir().canonicalPath())
                      .isWritable()) &&
                !docLocations.isEmpty())
#endif
                dlg.setDirectory(docLocations.first());

            ApexTools::expandWidgetToWindow(&dlg);
            saveFileName.clear();
            if (dlg.exec() == QDialog::Accepted)
                saveFileName = dlg.selectedFiles().first();
        }

        if (!saveFileName.isEmpty()) {
            if (QFile::exists(saveFileName)) {
                int answer =
#if defined(Q_OS_ANDROID)
                    QMessageBox::question(
                        ApexControl::Get().mainWindow(), tr("File exists"),
                        tr("File already exists, overwrite?")) ==
                            QMessageBox::Yes
                        ? 1
                        : 2;
#else
                    QMessageBox::question(
                        ApexControl::Get().mainWindow(), tr("File exists"),
                        tr("File already exists, what should I do?"),
                        tr("Append"), tr("Overwrite"), tr("Cancel"));
#endif
                if (answer == 2) // cancel
                    continue;
                else if (answer == 0)
                    overwrite = false;
                else
                    overwrite = true;
            }

            if (Save(saveFileName, overwrite)) {
                m_filename = saveFileName;
                m_bSaved = true;
                break;
            } else {
                int answer = QMessageBox::question(
                    ApexControl::Get().mainWindow(), tr("Can't write to file"),
                    tr("I can't write to the requested file.\nCheck whether "
                       "you have access rights and the disk is not full."),
                    tr("Retry"), tr("Lose data"));
                if (answer == 0) // cancel
                    continue;
                else
                    break;
            }
        }
        if (QMessageBox::question(ApexControl::Get().mainWindow(),
                                  tr("Lose data?"),
                                  tr("Do you want to discard the results?"),
                                  QMessageBox::Save | QMessageBox::Discard,
                                  QMessageBox::Save) == QMessageBox::Discard)
            break;
    }
#endif
}

void apex::ApexResultSink::Finished(bool askFilename)
{
    m_endTime = QDateTime::currentDateTime();
    SaveAs(askFilename);

    Q_EMIT(Saved());
}

namespace apex
{

ApexResultSink::ApexResultSink(ExperimentRunDelegate &p_rd)
    : ApexModule(p_rd), m_filename(), m_bSaved(false)
{
    // currentTrial=new TrialResult;
    //      currentTrial=NULL;
    //      m_bSaturation=false;

    // construct filename
    if (m_filename.isEmpty()) {
        QString expfile = m_rd.GetData().fileName();
        QFileInfo file(expfile);

        // get subject name
        QString subject = p_rd.GetData().resultParameters()->subject();

        if (!subject.isEmpty())
            m_filename = file.path() + "/" + file.baseName() + "-" + subject +
                         resultsExtension;
        else
            m_filename = file.path() + "/" + file.baseName() + "-results" +
                         resultsExtension;

        /*        qCDebug(APEX_RS, "ApexResultSink::ApexResultSink -
           m_filename=%s",
                        qPrintable(m_filename));
                qCDebug(APEX_RS, "Current path = %s",
                        qPrintable(QDir::current().path()));*/
    }
}

ApexResultSink::~ApexResultSink()
{
    // delete all trials in m_Results
    for (std::vector<TrialResult *>::const_iterator p = m_Results.begin();
         p != m_Results.end(); ++p) {
        delete *p;
    }
}
}

/**
 *
 * @param p_filename
 * @param p_overwrite overwrite the file if already exists or append to it?
 * @return
 */
bool apex::ApexResultSink::Save(const QString &p_filename,
                                const bool p_overwrite)
{
#ifdef SHOWSLOTS
    qCDebug(APEX_RS, "SLOT ApexResultSink::Save( const QString & p_filename )");
#endif

    QString content;
    QTextStream out(&content);
    out.setCodec("UTF-8");

    PrintXMLHeader(out);

    PrintIntro(out);

    // results per trial

    for (std::vector<TrialResult *>::const_iterator p = m_Results.begin();
         p != m_Results.end(); ++p) {
        TrialResult *c = *p; // simplify notation
        out << c->toXML();
    }

    out << CollectEndResults() << endl;
    PrintXMLFooter(out);

    bool result = p_overwrite
                      ? XmlUtils::writeDocument(XmlUtils::parseString(content),
                                                p_filename)
                      : XmlUtils::appendDocument(XmlUtils::parseString(content),
                                                 p_filename);
    if (!result)
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(metaObject()->className(),
                                                "Error while writing to file " +
                                                    p_filename)));

    return result;
}

void apex::ApexResultSink::PrintXMLHeader(QTextStream &out)
{
    QString cfn = ApexControl::Get().GetCurrentExperiment().fileName();

#ifdef WIN32
    cfn = "/" + cfn;
#endif

    out << "<?xml version='1.0' encoding='UTF-8'?>" << endl;

    out << "<apex:results experiment_file="
        << xmlEscapedAttribute(QSL("file:") + cfn);
    out << " xmlns:apex=\"http://med.kuleuven.be/exporl/apex/result\">" << endl;
}

void apex::ApexResultSink::PrintXMLFooter(QTextStream &out)
{
    out << c_fileFooter;
}

void apex::ApexResultSink::PrintIntro(QTextStream &out)
{
    QString ms = m_rd.GetData().resultParameters()->matlabScript();
    if (!ms.isEmpty())
        out << "<?matlab script=\"" << ms << "\"?>" << endl;

    data::tScriptParameters xp =
        m_rd.GetData().resultParameters()->resultParameters();
    if (!xp.isEmpty()) {
        out << "<parameters>" << endl;

        QMapIterator<QString, QString> it(xp);
        while (it.hasNext()) {
            it.next();
            out << "<parameter name=\"" << it.key() << "\">" << it.value()
                << "</parameter>" << endl;
        }
        out << "</parameters>" << endl;
    }

    out << "<general>" << endl;

    out << "\t<apex_version>" << APEX_SCHEMA_VERSION << "</apex_version>"
        << endl;
    out << "\t<apex_version_git>" << ApexTools::fetchVersion()
        << "</apex_version_git>" << endl;

#ifdef Q_OS_ANDROID
    out << "\t<serial>"
        << apex::android::ApexAndroidMethods::getDeviceSerialNumber()
        << "</serial>" << endl;
#else
    out << "\t<hostname>" << QHostInfo::localHostName() << "</hostname>"
        << endl;
#endif
    out << "\t<device_id>" << ApexTools::getApexGUID().toString()
        << "</device_id>" << endl;

    QString description =
        ApexControl::Get().GetCurrentExperiment().experimentDescription();
    if (!description.isEmpty()) {
        out << "\t<description>" << description << "</description>" << endl;
    }

    out << "\t<startdate>"
        << xmlEscapedText(
               ApexControl::Get().GetStartTime().toString(Qt::ISODate))
        << "</startdate>" << endl;
    out << "\t<enddate>" << xmlEscapedText(m_endTime.toString(Qt::ISODate))
        << "</enddate>" << endl;
    out << "\t<duration unit=\"seconds\">"
        << ApexControl::Get().GetStartTime().secsTo(m_endTime) << "</duration>"
        << endl;
    if (m_rd.GetData().resultParameters()) {
        QString rtscript(
            m_rd.GetData().resultParameters()->resultPage().toString());
        if (!rtscript.isEmpty())
            out << "\t<jscript>" << rtscript << "</jscript>" << endl;

        QString subject(m_rd.GetData().resultParameters()->subject());
        if (!subject.isEmpty())
            out << "\t<subject>" << subject << "</subject>" << endl;
    }

    const data::ParameterDialogResults *r =
        m_rd.GetData().parameterDialogResults();
    if (r) {
        out << "\t<interactive>" << endl;
        data::ParameterDialogResults::const_iterator it;
        for (it = r->begin(); it != r->end(); ++it) {
            out << "\t\t<entry>" << endl
                << "\t\t\t<expression>" << xmlEscapedText(it->xpath())
                << "</expression>" << endl
                << "\t\t\t<new_value>" << xmlEscapedText(it->newValue())
                << "</new_value>" << endl
                << "\t\t\t<succeeded>"
                << xmlEscapedText(it->succeeded() ? "true" : "false")
                << "</succeeded>" << endl
                << "\t\t\t<description>" << xmlEscapedText(it->description())
                << "</description>" << endl
                << "\t\t</entry>" << endl;
        }
        out << "\t</interactive>" << endl;
    }

    out << "</general>" << endl;
}

/**
 * Query all modules for extra XML data
 */
void apex::ApexResultSink::CollectResults(const QString &trial,
                                          const QString &extraXml)
{
    // TODO is this method still needed?
    // create new TrialData
    Q_ASSERT(!trial.isEmpty());
    TrialResult *currentTrial = new TrialResult;
    currentTrial->name = trial;

    m_Results.push_back(currentTrial);

    if (!extraXml.isEmpty())
        currentTrial->extra += extraXml;

    const ModuleList *modules = m_rd.modules();

    for (ModuleList::const_iterator it = modules->begin(); it != modules->end();
         ++it) {
        QString x((*it)->GetResultXML());

        if (!x.isEmpty())
            currentTrial->extra += x + "\n";
    }

    Q_EMIT(collected(currentTrial->toXML()));
}

QString apex::ApexResultSink::GetResultXML() const
{
    return QString();
}

/**
 * Query all modules for extra XML data
 */
const QString apex::ApexResultSink::CollectEndResults()
{
    QString result;

    const ModuleList *modules = m_rd.modules();

    if (!m_extraXml.isEmpty())
        result += m_extraXml;

    for (ModuleList::const_iterator it = modules->begin(); it != modules->end();
         ++it) {
        result += (*it)->GetEndXML();
    }

    return result;
}

/**
 * Set filename of results filename
 * resultsExtension will be added if necessary
 * @param p_filename
 */
void apex::ApexResultSink::SetFilename(const QString &p_filename)
{
    QRegExp re("\\.\\w{2,4}$");
    m_filename = p_filename;

    if (re.indexIn(p_filename) == -1) {
        qCDebug(APEX_RS, "ApexResultSink::SetFilename: No match");
        m_filename += resultsExtension;
    } else {
        qCDebug(APEX_RS, "ApexResultSink::SetFilename: match");
    }

    return;
}

void apex::ApexResultSink::setExtraXml(const QString &x)
{
    m_extraXml = x;
}

/**
 * Add -N after m_filename if it already exists
 */
bool apex::ApexResultSink::MakeFilenameUnique()
{
    QString tfilename;
    if (!m_filename.endsWith(resultsExtension))
        return false;

    if (QFile::exists(m_filename)) {
        tfilename =
            m_filename.left(m_filename.length() - 4); // remove extention

        int i = 1;
        while (QFile::exists(
            QString("%1-%2%3").arg(tfilename).arg(i).arg(resultsExtension)))
            ++i;
        m_filename =
            QString("%1-%2%3").arg(tfilename).arg(i).arg(resultsExtension);
    }

    return true;
}
