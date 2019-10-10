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

#include "apexdata/experimentdata.h"
#include "apexdata/interactive/parameterdialogresults.h"
#include "apexdata/mainconfigfiledata.h"
#include "apexdata/parameters/generalparameters.h"
#include "apexdata/result/resultparameters.h"
#include "apexmain/resultsink/resultfilepathcreator.h"
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

using namespace cmn;

namespace apex
{

const QString ApexResultSink::resultfileXmlClosingTag("</apex:results>\n");
const QString ApexResultSink::resultfileExtension("apr");

ApexResultSink::ApexResultSink(ExperimentRunDelegate &runDelegate)
    : ApexModule(runDelegate)
{
}

ApexResultSink::~ApexResultSink()
{
    // delete all trials in trialResults
    for (std::vector<TrialResult *>::const_iterator p = trialResults.begin();
         p != trialResults.end(); ++p) {
        delete *p;
    }
}

const QString ApexResultSink::saveResultfile()
{
    QString suggestedPath;
    if (resultfilePath.isEmpty()) {
        suggestedPath = ResultfilePathCreator().createDefaultAbsolutePath(
            m_rd.GetData().fileName(),
            m_rd.GetData().resultParameters()->subject());
    } else {
        suggestedPath = resultfilePath;
    }

    bool allowDeviation = !m_rd.GetData().generalParameters()->GetAutoSave();

    return tryToSave(suggestedPath, allowDeviation);
}

QString ApexResultSink::tryToSave(const QString &suggestedPath,
                                  const bool allowDeviationFromSuggestion)
{
    const QString resultfilePath = allowDeviationFromSuggestion
                                       ? askSaveLocation(suggestedPath)
                                       : suggestedPath;

    if (resultfilePath.isEmpty()) {
        return askShouldDiscardResults() ? QString() : tryToSave(suggestedPath);
    }

    if (!save(resultfilePath)) {
        informSavingFailed(resultfilePath);
        return tryToSave(suggestedPath);
    }

    return resultfilePath;
}

const QString ApexResultSink::askSaveLocation(const QString &suggestedPath)
{
    QFileDialog dialog(
        ApexControl::Get().mainWindow(), QString(), suggestedPath,
        QSL("APEX savedata (*.%1);;XML Files (*.xml);;All files (*)")
            .arg(resultfileExtension));

    dialog.setAcceptMode(QFileDialog::AcceptSave);
    // allow the filedialog to warn when overwriting
    dialog.setOption(QFileDialog::DontConfirmOverwrite, false);

    ApexTools::expandWidgetToWindow(&dialog);

    return dialog.exec() == QDialog::Accepted ? dialog.selectedFiles().first()
                                              : QString();
}

bool ApexResultSink::askShouldDiscardResults()
{
    QString title = tr("Discard results?");
    QString text = tr("Do you want to discard the results?");
    return QMessageBox::question(ApexControl::Get().mainWindow(), title, text,
                                 QMessageBox::Save | QMessageBox::Discard,
                                 QMessageBox::Save) == QMessageBox::Discard;
}

void ApexResultSink::informSavingFailed(const QString &path)
{
    QString title = tr("Can't write to file");
    QString text = tr("Can't write to \"%1\".\nCheck whether you "
                      "have access rights and the disk is not full.")
                       .arg(path);
    QMessageBox::critical(ApexControl::Get().mainWindow(), title, text);
}

bool ApexResultSink::save(const QString &path)
{
    QString content;
    QTextStream out(&content);
    out.setCodec("UTF-8");

    printXMLHeader(out);

    printIntro(out);

    // results per trial

    for (std::vector<TrialResult *>::const_iterator p = trialResults.begin();
         p != trialResults.end(); ++p) {
        TrialResult *c = *p; // simplify notation
        out << c->toXML();
    }

    out << collectEndResults() << endl;
    printXMLFooter(out);

    QDir(QFileInfo(path).dir()).mkpath(".");
    bool result = XmlUtils::writeDocument(XmlUtils::parseString(content), path);

    if (!result)
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: Error while writing to file %2")
                                  .arg(metaObject()->className())
                                  .arg(path)));

    return result;
}

void ApexResultSink::printXMLHeader(QTextStream &out) const
{
    QString cfn = m_rd.GetData().fileName();

#ifdef WIN32
    cfn = "/" + cfn;
#endif

    out << "<?xml version='1.0' encoding='UTF-8'?>" << endl;

    out << "<apex:results experiment_file="
        << xmlEscapedAttribute(QSL("file:") + cfn);
    out << " xmlns:apex=\"http://med.kuleuven.be/exporl/apex/result\">" << endl;
}

void ApexResultSink::printXMLFooter(QTextStream &out) const
{
    out << resultfileXmlClosingTag;
}

void ApexResultSink::printIntro(QTextStream &out) const
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

    QString resultscript = m_rd.GetData().resultParameters()->extraScript();
    if (!resultscript.isEmpty()) {
        out << "<resultscript>" << resultscript << "</resultscript>" << endl;
    }

    out << "<general>" << endl;

    out << "\t<apex_version>" << APEX_SCHEMA_VERSION << "</apex_version>"
        << endl;
    out << "\t<apex_version_git>" << ApexTools::fetchVersion()
        << "</apex_version_git>" << endl;

#ifdef Q_OS_ANDROID
    out << "\t<serial>"
        << apex::android::ApexAndroidBridge::getDeviceSerialNumber()
        << "</serial>" << endl;
#else
    out << "\t<hostname>" << QHostInfo::localHostName() << "</hostname>"
        << endl;
#endif
    out << "\t<device_id>" << ApexTools::getApexGUID().toString()
        << "</device_id>" << endl;

    QString description = m_rd.GetData().experimentDescription();
    if (!description.isEmpty()) {
        out << "\t<description>" << description << "</description>" << endl;
    }

    out << "\t<startdate>"
        << xmlEscapedText(experimentStartTime.toString(Qt::ISODate))
        << "</startdate>" << endl;
    out << "\t<enddate>"
        << xmlEscapedText(experimentEndTime.toString(Qt::ISODate))
        << "</enddate>" << endl;
    out << "\t<duration unit=\"seconds\">"
        << experimentStartTime.secsTo(experimentEndTime) << "</duration>"
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

    out << "\t<stopped_by>" << stopCondition << "</stopped_by>" << endl;

    out << "</general>" << endl;
}

/**
 * Query all modules for extra XML data
 */
void ApexResultSink::collectResults(const QString &trial,
                                    const QString &extraXml)
{
    // TODO is this method still needed?
    // create new TrialData
    Q_ASSERT(!trial.isEmpty());
    TrialResult *currentTrial = new TrialResult;
    currentTrial->name = trial;

    trialResults.push_back(currentTrial);

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

QString ApexResultSink::GetResultXML() const
{
    return QString();
}

/**
 * Query all modules for extra XML data
 */
const QString ApexResultSink::collectEndResults() const
{
    QString result;

    const ModuleList *modules = m_rd.modules();

    if (!extraXml.isEmpty())
        result += extraXml;

    for (ModuleList::const_iterator it = modules->begin(); it != modules->end();
         ++it) {
        result += (*it)->GetEndXML();
    }

    return result;
}

/**
 * Set filename of results filename
 * resultsExtension will be added if necessary
 * @param resultfilePath
 */
void ApexResultSink::setFilename(const QString &resultfilePath)
{
    this->resultfilePath = resultfilePath;

    if (QRegExp("\\.\\w{2,4}$").indexIn(resultfilePath) == -1) {
        this->resultfilePath =
            QSL("%1.%2").arg(resultfilePath).arg(resultfileExtension);
    }
}

void ApexResultSink::setExtraXml(const QString &extraXml)
{
    this->extraXml = extraXml;
}

void ApexResultSink::setStopCondition(const bool &stoppedByUser)
{
    stopCondition = QString(stoppedByUser ? "user" : "procedure");
}

void ApexResultSink::setExperimentStartTime(
    const QDateTime &experimentStartTime)
{
    this->experimentStartTime = experimentStartTime;
}

void ApexResultSink::setExperimentEndTime(const QDateTime &experimentEndTime)
{
    this->experimentEndTime = experimentEndTime;
}
}