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

#include "services/mainconfigfileparser.h"
#include "services/pluginloader.h"

#include "experimentparser.h"
#include "experimentrunner.h"

#include <QDebug>
#include <QMessageBox>

using namespace apex;
using namespace apex::data;

ExperimentData* ExperimentRunner::parseExperiment(const QString& fileName)
{
    qCDebug(APEX_RS, "Parsing Experiment");

    data::ExperimentData* data;
    StatusReporter& error = ErrorHandler::Get();

//     experiment.reset();

    error.clearCounters();
//     m_Wnd->Clear();
//     m_state.mp_Reset();

#ifdef PERL
    const QString& experimentFile = fileName;
    const QString perlPath = MainConfigFileParser::Get().GetPerlPath();
    // do Apex 2 compatibility if possible
    if (! experimentFile.endsWith(".xml") && !experimentFile.endsWith(".apx"))
    {
        error.addMessage("newExperiment", ".xml extension not found in filename, trying to convert from Apex 2");
        if (!perlPath.isEmpty() && QFile::exists(perlPath))
        {
            QRegExp re("(\\.\\w{3})$");
            QString newfilename = experimentFile;
            newfilename.replace(re, ".xml");

            if (QFile::exists(newfilename))
            {
                int answer = QMessageBox::question(0, tr("File exists"), tr("Trying to convert to Apex 3 format, but the file %1 already exists, what should I do?").arg(newfilename), tr("Overwrite"), tr("Cancel"));
                if (answer == 1)    // cancel
                    goto EndConvert;
            }


            QString cmdline = perlPath + " " +
                              Paths::Get().GetScriptsPath() + "/convert/a2to3.pl " +
                              experimentFile + " " +
                              newfilename;
            system(cmdline.toAscii());
            qCDebug(APEX_RS, "%s", qPrintable(cmdline));
            if (! QFile::exists(newfilename))
            {
                error.addWarning("newExperiment", "Couldn't convert apex 2 file, trying to interpret it as apex 3 file");
            }
            else
            {
                fileName = newfilename;
            }
        }
        else
        {
            error.addWarning("newExperiment", "Perl interpreter not found");
        }
    }

EndConvert:
#endif

    if (!QFile(fileName).exists())
    {
        error.addError("ApexControl" , tr("Experiment file not found"));
#ifndef NODIALOG
        QMessageBox::critical(NULL, tr("Error"), tr("Experiment file not found"));
#endif
//         m_state.mp_SetError();
        return 0;
    }
//     deleteModules();

    const QString& sConfigFileName(fileName);
    QDir dConfigFileDir(fileName);
    dConfigFileDir.cdUp();
    error.addMessage("ApexControl" , "Parsing " + sConfigFileName);
    try
    {
        ExperimentParser parser(fileName, expressions);
        parser.LogErrorsToApex (&error);
        data = parser.parse(true);
//         experiment.reset(new data::ExperimentData(*parser));
        //experimentRunDelegate.reset(new ExperimentRunDelegate(*experiment, m_Wnd->centralWidget()));
        //m_Wnd->setExperimentRunDelegate(experimentRunDelegate.get());
        //error.AddItem(experimentRunDelegate->GetError());
        //m_Wnd->ApplyConfig(&(experiment->GetScreens())); //make sure to keep this before makeModules
        //m_Wnd->EnableSkip(experiment->GetGeneralParameters()->GetAllowSkip());
        //makeModules();
        //m_Wnd->MruFile(sConfigFileName);
        //m_state.mp_SetState(tState::mc_eLoaded);
    }
    catch (std::exception &e)
    {
        error.addError("ApexControl" , e.what());
        //m_state.mp_SetError();
    }
    catch (...)
    {
        error.addError("ApexControl" , "FAILED");
        //m_state.mp_SetError();
    }
    //m_Wnd->SetOpenDir(f_sDirectoryFromFile(sConfigFileName));

    int nWarnings = error.numberOfWarnings();
    error.addWarning("", QString::number(error.numberOfErrors()) + tr(" Error"));
    if (error.numberOfErrors())
        error.addWarning("", tr("Warning: errors after the first one could be generated by earlier errors. Try to fix the first error first."));
    error.addWarning("", QString::number(nWarnings) + " Warning(s)");

//     delete parser;

    if (error.numberOfErrors() == 0)
    {
        error.addWarning("ApexControl" , "DONE");
        //PrepareExperiment();
        return data;
    }
    else
    {
        return 0;
    }
}
