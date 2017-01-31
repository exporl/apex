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

#include "apexresultsink.h"
#include "trialresult.h"
#include "apexcontrol.h"
#include "services/filedialog.h"
#include "runner/experimentrundelegate.h"
#include "xml/apexxmltools.h"

#include "apextools.h"
#include "services/mainconfigfileparser.h"
#include "apexcontrol.h"        // get main window
#include "gui/mainwindow.h"

#include "parameters/generalparameters.h"

//from libdata
#include "result/resultparameters.h"
#include "interactive/parameterdialogresults.h"
#include "experimentdata.h"

#include <qmessagebox.h>
#include <qdatetime.h>
#include <qregexp.h>

using apex::ApexXMLTools::XMLutils;

const QString apex::ApexResultSink::c_fileFooter( "</apex:results>\n");
const QString apex::ApexResultSink::resultsExtension( ".apr");

void apex::ApexResultSink::SaveAs( ) {
#ifdef SHOWSLOTS
    qDebug("ApexResultSink::Finished( )");
#endif

    qDebug("ApexResultSink::SaveAs(): m_filename=%s", qPrintable(m_filename));
        qDebug("Current path = %s",
                qPrintable(QDir::current().path()));


    if (m_bSaved) {
        qDebug("ApexResultSink::SaveAs: not saving, already saved");
        return;
    }


    MakeFilenameUnique();
//    qDebug("m_filename=%s", qPrintable (m_filename));

    // save data if any
    if (m_Results.size() <= 0) {
        qDebug("Nothing to save, exiting");
    }


    bool overwrite=false;



#ifndef NOSAVE

    bool askFilename=true;
    QString saveFileName;
    while (1) {

        if (m_rd.GetData().generalParameters()->GetAutoSave()) {
            askFilename=false;
            int answer = QMessageBox::question(0, tr("Save?"), QString(tr("Do you want to save the results in file %1?")).arg(m_filename), tr("Save"), tr("Change filename"), tr("Discard"));
            if (answer==0)
                saveFileName=m_filename;
            else if (answer==1)
                askFilename=true;
            else if (answer==2)
                return;
        }

        if (askFilename)
            saveFileName = FileDialog::Get().mf_sGetAnyFile(m_filename, tr("APEX savedata (*%1);;XML Files (*.xml);;All files (*)").arg(resultsExtension) );

        if (saveFileName.isEmpty()) {                    // user pressed cancel
            Q_ASSERT("Can't save");
        } else {

            if (QFile::exists(saveFileName)) {
                /*if ( QMessageBox::question(0, tr("File exists"), tr("File already exists, overwrite?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::No) continue;*/
                int answer = QMessageBox::question(ApexControl::Get().GetMainWnd(), tr("File exists"), tr("File already exists, what should I do?"), tr("Append"), tr("Overwrite"), tr("Cancel"));
                if (answer==2)          // cancel
                    continue;
                else if (answer==0)
                    overwrite=false;
                else
                    overwrite=true;
            }

            if (Save(saveFileName,overwrite)) {
                m_filename=saveFileName;
                m_bSaved=true;
                break;
            } else {
                int answer = QMessageBox::question(ApexControl::Get().GetMainWnd(), tr("Can't write to file"), tr("I can't write to the requested file.\nCheck whether you have access rights and the disk is not full."), tr("Retry"),  tr("Lose data"));
                if (answer==0)          // cancel
                    continue;
                else
                    break;
            }
        }

        if ( QMessageBox::question(ApexControl::Get().GetMainWnd(), tr("Lose data?"), tr("Do you want to discard the results?"), QMessageBox::Save | QMessageBox::Discard, QMessageBox::Save) == QMessageBox::Discard)
            break;

    }
#endif
}


void apex::ApexResultSink::Finished( ) {
    SaveAs();

    emit(Saved());

}

namespace apex {

ApexResultSink::ApexResultSink(ExperimentRunDelegate& p_rd) :
        ApexModule(p_rd),
        m_filename(),
m_bSaved(false) {
    //currentTrial=new TrialResult;
    //      currentTrial=NULL;
    //      m_bSaturation=false;

    // construct filename
    if (m_filename.isEmpty()) {
        QString expfile= m_rd.GetData().fileName();
        QFileInfo file(expfile);

        // get subject name
        QString subject= p_rd.GetData().
                resultParameters()->GetSubject();

        if (! subject.isEmpty())
            m_filename = file.path() + "/" + file.baseName()
                + "-" + subject + resultsExtension;
        else
            m_filename = file.path() + "/" + file.baseName()
                + "-results" + resultsExtension;


/*        qDebug("ApexResultSink::ApexResultSink - m_filename=%s",
                qPrintable(m_filename));
        qDebug("Current path = %s",
                qPrintable(QDir::current().path()));*/

    }
}


ApexResultSink::~ApexResultSink() {
    // delete all trials in m_Results
    for (std::vector<TrialResult*>::const_iterator p=m_Results.begin(); p!=m_Results.end(); ++p) {
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
bool apex::ApexResultSink::Save( const QString & p_filename, const bool p_overwrite ) {
#ifdef SHOWSLOTS
    qDebug("SLOT ApexResultSink::Save( const QString & p_filename )");
#endif


    QFile::OpenMode flags;
    if (!p_overwrite)
        flags=QFile::Append;
    else
        flags=0;

    QFile file(p_filename);
    if (!file.open(flags | QFile::WriteOnly | QFile::Text) ) {
        ErrorHandler::Get().addError(metaObject()->className(),
                          "Can't write output to file " + p_filename);
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    PrintXMLHeader(out);

    PrintIntro(out);

    // results per trial

    for (std::vector<TrialResult*>::const_iterator p=m_Results.begin(); p!=m_Results.end(); ++p) {
        TrialResult* c = *p;                    // simplify notation
        out << c->toXML();
    }

    out << CollectEndResults() <<endl;
    PrintXMLFooter(out);
    
    if (file.error() != QFile::NoError) {
        ErrorHandler::Get().addError(metaObject()->className(),"Error while writing to file " + p_filename);
        return false;
    }

    return true;

}

void apex::ApexResultSink::PrintXMLHeader( QTextStream & out ) {
    QString cfn = ApexControl::Get().GetCurrentExperiment().fileName();

	// make cfn relative
	/*QDir curDir(QDir::currentDirPath());
	cfn = curDir.relativeFilePath(cfn);*/

#ifdef WIN32

    cfn = "/" + cfn;
#endif

    out << "<?xml version='1.0' encoding='UTF-8'?>" << endl;

    QString xsltscript = m_rd. GetData().
            resultParameters()->GetXsltScript();
    if (!xsltscript.isEmpty())
        out << "<?xml-stylesheet  href=\"" <<
                ApexTools::MakeDirEnd(
                MainConfigFileParser::Get().GetXsltOnlinePath()) <<
                xsltscript <<
                "\" type=\"text/xsl\"?>" << endl;


    out << "<apex:results experiment_file=\"file:" <<  XMLutils::xmlEscapedText(cfn) << "\" ";
    out << "xmlns:apex=\"http://med.kuleuven.be/exporl/apex/result\">" << endl;
}

void apex::ApexResultSink::PrintXMLFooter( QTextStream & out ) {
    //out << "</apex:results>" << endl;
    out << c_fileFooter;
}



void apex::ApexResultSink::PrintIntro(QTextStream& out) {
    QDateTime end;
    end=QDateTime::currentDateTime();

    QString xsltscript = m_rd. GetData().
            resultParameters()->GetXsltScript();

    QString ms=m_rd.GetData().resultParameters()->GetMatlabScript();
    if (!ms.isEmpty())
        out << "<?matlab script=\"" << ms << "\"?>" << endl;


    data::tXsltParameters xp =
        m_rd.GetData().resultParameters()->GetXsltParameters();
    if (!xp.isEmpty()) {
        out << "<parameters>" << endl;
        for (data::tXsltParameters::const_iterator it=xp.begin();
                it!=xp.end(); ++it) {
            out << "<parameter name=\"" << it->first << "\">" << it->second
                <<"</parameter>" << endl;
        }
        out << "</parameters>" << endl;
    }

    out << "<general>" << endl;

    QString description = ApexControl::Get().GetCurrentExperiment().experimentDescription();
    if (!description.isEmpty()) {
        out << "\t<description>" << description << "</description>" << endl;
    }

    out << "\t<startdate>" << XMLutils::xmlEscapedText(ApexControl::Get().GetStartTime().toString()) << "</startdate>" << endl;
    out << "\t<enddate>" << XMLutils::xmlEscapedText(end.toString()) << "</enddate>" << endl;
    out << "\t<duration unit=\"seconds\">" << ApexControl::Get().GetStartTime().secsTo(end) << "</duration>" << endl;
    if ( m_rd.GetData().resultParameters()) {
        if (!xsltscript.isEmpty())
            out << "\t<xsltscript>" << xsltscript << "</xsltscript>" << endl;

        QString rtscript( m_rd.GetData().resultParameters()->resultPage().toString());
        if (!rtscript.isEmpty())
            out << "\t<jscript>" << rtscript << "</jscript>" << endl;

        QString subject(m_rd.GetData().resultParameters()->GetSubject());
        if ( !subject.isEmpty())
            out <<"\t<subject>" << subject << "</subject>" << endl;
    }

    
    const data::ParameterDialogResults* r = m_rd.GetData().parameterDialogResults();
    if (r) {
        out << "\t<interactive>" << endl;
        data::ParameterDialogResults::const_iterator it;
        for (it = r->begin(); it!=r->end(); ++it)
        {

                    if (! (*it).xpath().isEmpty())
                         out << "\t\t<entry expression=" << XMLutils::xmlEscapedAttribute(it->xpath())
                                << " new_value=" << XMLutils::xmlEscapedAttribute(it->newValue())
                                << " succeeded=" << XMLutils::xmlEscapedAttribute(it->succeeded() ? "true" : "false")
                             << " />" <<endl;
                    else
                        out << "\t\t<entry description=" << XMLutils::xmlEscapedAttribute(it->description())
                                << " new_value=" << XMLutils::xmlEscapedAttribute(it->newValue())
                             << " />" <<endl;
        }
        out << "\t</interactive>" << endl;
    }

    out << "</general>" << endl;

}




/**
 * Query all modules for extra XML data
 */
void apex::ApexResultSink::CollectResults( ) {
    // create new ApexTrial
    TrialResult* currentTrial = new TrialResult;
    currentTrial->name = ApexControl::Get().GetCurrentTrial();
    currentTrial->timestamp=QDateTime::currentDateTime(); //[ stijn ] FIXME has to be set when trial starts
    //      currentTrial->correctAnswer=-1;

    m_Results.push_back(currentTrial);


    const std::vector<ApexModule*>* modules = m_rd.modules();

    for (std::vector<ApexModule*>::const_iterator it = modules->
            begin();
            it!=modules->end();
            ++it) {
        QString x( (*it)->GetResultXML() );
        if (!x.isEmpty())
            currentTrial->extra += x + "\n";
    }

    emit (collected(currentTrial->toXML()));
}

QString apex::ApexResultSink::GetResultXML( ) const
{
    return QString();
}

/**
 * Query all modules for extra XML data
 */
const QString apex::ApexResultSink::CollectEndResults( ) {
    QString result;

    const std::vector<ApexModule*>* modules = m_rd.modules();

    for (std::vector<ApexModule*>::const_iterator it = modules->
            begin();
            it!=modules->end();
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
void apex::ApexResultSink::SetFilename( const QString & p_filename ) {
    QRegExp re("\\.\\w{2,4}$");
    m_filename = p_filename;

    if (re.indexIn(p_filename) == -1) {
        qDebug("ApexResultSink::SetFilename: No match");
        m_filename += resultsExtension;
    } else {
        qDebug("ApexResultSink::SetFilename: match");
    }

    return;
}


/**
 * Add -N after m_filename if it already exists
 */
bool apex::ApexResultSink::MakeFilenameUnique( ) {
    QString tfilename;
    if (! m_filename.endsWith( resultsExtension ) )
        return false;

    if ( QFile::exists(m_filename)) {
        tfilename = m_filename.left(m_filename.length()-4);      // remove extention

        int i=1;
        while ( QFile::exists( QString( "%1-%2%3").arg(tfilename).arg(i).arg(resultsExtension)) )
            ++i;
        m_filename=QString( "%1-%2%3").arg(tfilename).arg(i).arg(resultsExtension) ;
    }

    return true;
}

