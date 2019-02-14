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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_APEXRESULTSINK_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_APEXRESULTSINK_H_

#include "apextools/global.h"

#include "apexmain/apexmodule.h"

#include <qtextstream.h>

#include <QDateTime>

#include <vector>

namespace apex
{

class TrialResult;
class ScreenResult;

/**
Apex result collector module

@author Tom Francart,,,
*/
class APEX_EXPORT ApexResultSink : public ApexModule
{
    Q_OBJECT
public:
    ApexResultSink(ExperimentRunDelegate &p_rd, QDateTime experimentStartTime);

    ~ApexResultSink();

    virtual QString GetResultXML() const;

public: // slot replacements
    // void NewTrial(const QString& p_name);
    void SetFilename(const QString &p_filename);
    const QString &GetFilename() const
    {
        return m_filename;
    }
    bool IsSaved() const
    {
        return m_bSaved;
    }

    /**
      * Extra XML will be appended to the results file
      */
    void setExtraXml(const QString &x);

    /**
      * provide information about how the experiment was stopped
      */
    void setStopCondition(const bool &stoppedByUser);

    static const QString c_fileFooter;
    static const QString resultsExtension; // .apx

public slots:
    /*void NewStimulus(const QString& p_name);
    void Answer(const ScreenResult& p_answer);
    void NewAnswerCorrect(const bool p_correct);
    void AnswerTime(const int p_time);*/
    void Finished(bool askFilename = true);

    /*void SetCorrectAnswer(const unsigned p_answer);
    void SetSaturation();
    void SetTargetParam(const t_adaptParam p_param);*/
    void CollectResults(const QString &trial, const QString &extraXml);

signals:
    void Saved(); // is emitted after Finished() did it's job
    void collected(QString xml);

private:
    void SaveAs(bool askFilename = true);
    bool Save(const QString &p_filename, const bool p_overwrite);
    bool MakeFilenameUnique();

    void PrintXMLHeader(QTextStream &out);
    void PrintXMLFooter(QTextStream &out);
    void
    PrintIntro(QTextStream &out); // print general stuff about the experiment

    const QString CollectEndResults();

private:
    //              TrialResult* currentTrial;
    std::vector<TrialResult *> m_Results;

    QDateTime m_startTime;
    QDateTime m_endTime;
    QString m_stopCondition;
    QString m_filename;
    QString m_extraXml;
    bool m_bSaved;

    /*      bool m_bSaturation;
            int p_reversals;
            bool p_defReversals;*/
};
}

#endif
