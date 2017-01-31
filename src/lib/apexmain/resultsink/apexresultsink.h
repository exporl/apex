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

#ifndef APEXAPEXRESULTSINK_H
#define APEXAPEXRESULTSINK_H

#include "apexmodule.h"
#include "procedure/adaptiveprocedure.h"

#include <vector>
#include <qtextstream.h>

namespace apex {

class TrialResult;
class ScreenResult;

/**
Apex result collector module

@author Tom Francart,,,
*/
class ApexResultSink : public ApexModule
{
        Q_OBJECT
public:
    ApexResultSink(ExperimentRunDelegate& p_rd);

    ~ApexResultSink();

        virtual QString GetResultXML( ) const;

        public:         // slot replacements
                //void NewTrial(const QString& p_name);
                void SetFilename(const QString& p_filename);
                const QString& GetFilename() const { return m_filename; };
                bool IsSaved() const { return m_bSaved; };

                static const QString c_fileFooter;
                static const QString resultsExtension;          // .apx

        public slots:
                /*void NewStimulus(const QString& p_name);
                void Answer(const ScreenResult& p_answer);
                void NewAnswerCorrect(const bool p_correct);
                void AnswerTime(const int p_time);*/
                void Finished(bool askFilename=true);
                void SaveAs(bool askFilename=true);
                /*void SetCorrectAnswer(const unsigned p_answer);
                void SetSaturation();
                void SetTargetParam(const t_adaptParam p_param);*/
                void CollectResults();

        signals:
                void Saved();                   // is emitted after Finished() did it's job
                void collected(QString xml);


        private:
                bool Save(const QString& p_filename, const bool p_overwrite );
                bool MakeFilenameUnique( );

                void PrintXMLHeader(QTextStream& out);
                void PrintXMLFooter(QTextStream& out);
                void PrintIntro(QTextStream& out);                      // print general stuff about the experiment

                const QString CollectEndResults( );

        private:
//              TrialResult* currentTrial;
                std::vector<TrialResult*> m_Results;

                QString m_filename;
                QDateTime m_endTime;
                bool m_bSaved;


        /*      bool m_bSaturation;
                int p_reversals;
                bool p_defReversals;*/
};

}

#endif
