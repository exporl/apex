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

#ifndef APEXAPEXPROCEDURE_H
#define APEXAPEXPROCEDURE_H

#include "apexmodule.h"
#include "stimulus/stimulus.h"

//from libdata
#include "procedure/apexprocedureconfig.h"

#include <random.h>

#include <list>
#include <QStringList>

class PluginProcedureAPI;

namespace apex
{

class ScreenResult;

using namespace stimulus;

/**
 * Base class for all procedures
 * Procedure has two modes:
 * - plain
 * - AFC
 * In AFC mode, multiple standards and one stimulus are presented to the user, right after each other.
 * Procedure sends the correct answer to the corrector
@author Tom Francart,,,
*/
class ApexProcedure : public ApexModule
{
    Q_OBJECT
    friend class PluginProcedureAPI;
public:
    ApexProcedure(ExperimentRunDelegate& p_rd,
                  data::ApexProcedureConfig* config,
                  bool deterministic = false);

    virtual ~ApexProcedure();

    //virtual QString GetXMLResults()=0;
    virtual QString GetResultXML() const;
    //      void SetID(const QString& p_id) { m_procedureConfig->SetID(p_id);};
    const QString& GetID() const {
        return m_id;
    };
    bool isStarted() const {
        return m_bStarted;
    };
    virtual void setStarted(bool p) {
        m_bStarted=p;
    }
    bool isFinished() const {
        return m_bFinished;
    };

    //      const QString& GetCurrentCorrectAnswer() const { return m_CurrentCorrectAnswer; };

    virtual unsigned GetNumTrials() const=0;
    virtual unsigned GetProgress() const=0;

    virtual bool CanPause() const {
        return true;
    }
    ;            // does pausing make sense for this type of procedure?

    /**
      * Query outputlist.
      * @return true if there are items in the list
      */
    virtual bool mf_bHasOutputLeft() const {
        return m_outputList.size() != 0;
    }

    /**
      * Clear outputlist.
      */
    virtual void mp_ClearOutputList() {
        m_outputList.clear();
    }

public slots:
    virtual void Start();
    virtual void Stop();
    virtual void RepeatOutput();
    //virtual void SetResult()=0;
    virtual bool NextTrial(const bool result, const ScreenResult* screenresult);
    virtual void StimulusDone();                            // react on stimulusdone from stimulusoutput

    virtual void FirstTrial()=0; //[ stijn ] prepare first trial

    const std::vector<data::ApexTrial*>& GetTrialList() const //[ stijn ] temporary, used to get # trials for progressbar
    {
        return m_trialList;
    }
    unsigned GetCurTrial() const {
        return m_nTrials;
    }


public:         // signal replacements
    void NewTrial(const QString& p_name);
    virtual void StartOutput();
    virtual void doDeterministicGeneration();

signals:
    //[ stijn ] added SendMessage and removed the virtual keyword
    void Finished();
    void Started();

    void SendMessage( const QString& );

    //[ controlthread ]
    void NewStimulus(const QString& p_stimulus);

    void NewStimulus(stimulus::Stimulus* p_stimulus);
    //virtual void NewSignalStimulus(const QString& p_stimulus); // the stimulus containing the target signal [ cleanup ]
    void NewScreenElement(const QString& p_element);        // returns the screen element corresponding to the currently playing stimulus
    void NewScreen(const QString& p_screen);
    void EnableScreen(const bool);
    void SetCorrectAnswer(const unsigned p_answer);
    void OutputDone();
    void StimulusEnd();                     // indicated end of current stimulus output

    void SetProgress(const unsigned value);
    void SetNumTrials(const unsigned value);

protected:
    virtual void CreateTrialList(const bool doSkip=true);
    virtual void ExperimentFinished();
    virtual const tStimulusList GetStandards( data::ApexTrial* trial);
    const QString GetRandomStandard( const tStimulusList& stdList ) const;

    virtual void CreateOutputList(data::ApexTrial* p_trial, Stimulus* p_stimulus=0);

protected:
    data::ApexProcedureConfig* m_procedureConfig;

    bool m_useTrialList;            // are we using a predefined trial list?
    std::vector<data::ApexTrial*> m_trialList;
    std::vector<data::ApexTrial*>::const_iterator m_trialListIterator;

    std::list<QString> m_outputList;                // list of stimuli to be output this trial
    std::list<QString> m_lastOutputList;            // output list that was the last created, useful for repeating the output

    bool m_bFinished;
    bool m_bStarted;
    bool m_bFirstPresentation;                      // is this the first presentation of the first stimulus?

private:
    virtual void RandomizeTrialList(const bool doSkip);
    virtual void GenerateOrderedTrialList(const bool doSkip);

    virtual QString GetCurrentScreenElement(int p_position);

protected:
    virtual QString XMLHeader() const;
    virtual QString XMLFooter() const;
    virtual QString XMLBody() const;
    virtual QString XMLID() const;

    // current status
    int m_CurrentCorrectAnswer;     // if procedure determines the correct answer (ie AFC procedure) this variable holds it
    int m_nTrials;                  // number of current trial
    QString m_currentStimulus;      // currently playing stimulus
    QDateTime m_trialStartTime;// Starting time of current stimulus
    QStringList m_currentStandards; // played standards (for results)
    QString m_id;                   // id of this procedure

    Random randomGenerator;
    Random choicesRandomGenerator;

private:
    int m_nCurrentOutputStimulus;           // number of the current output stimulus (useful for afc)


};

}

#endif
