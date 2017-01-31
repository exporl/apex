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

#include "apexprocedure.h"
#include "apexcontrol.h"
#include "corrector/corrector.h"
#include "screen/apexscreen.h"

#include "screen/apexscreenresult.h"

#include "stimulus/stimulus.h"
#include "stimulus/stimulusoutput.h"
#include "stimulus/stimulusparameters.h"

#include <appcore/threads/thread.h>     // sleep

#include "../randomgenerator/randomgenerators.h"

//from libdata
#include "experimentdata.h"

// for creating a silence stimulus:
#include "stimulus/datablock.h"
#include "runner/experimentrundelegate.h"
#include "stimulus/wav/wavdevice.h"

#include <qapplication.h>

#include <algorithm>            // random_shuffle

namespace apex
{

using namespace stimulus;

ApexProcedure::ApexProcedure(ExperimentRunDelegate& p_rd,
                             data::ApexProcedureConfig* config,
                             bool deterministic) :
    ApexModule(p_rd),
    m_procedureConfig(config),
    m_useTrialList(false),
    m_bFinished(false),
    m_bStarted( false ),
    m_bFirstPresentation(true),
    m_CurrentCorrectAnswer(-1),
    m_nTrials(0),
    m_id(config->GetID())
{
    if (deterministic)
        randomGenerator.setSeed(0);

#ifdef PRINTPROCEDURE
    qDebug("New Apexprocedure, id=" + m_id);
#endif
}

ApexProcedure::~ApexProcedure()
{
}

/**
 *  @param doSkip   do skipping according to the set number of trials to be skipped
 * Create a list of trials that will sequentially be presented to the user.
 * This is commonly done before the procedure commences. If a list of trials is used,
 * m_useTrialList is set to true. The trial list is built according to the m_order
 * parameter that determines whether the order of the trials is random or sequential.
 */
void ApexProcedure::CreateTrialList(const bool doSkip) {
    m_useTrialList=true;

    m_trialList.empty();

    if (m_procedureConfig->GetParameters()->m_order == data::ApexProcedureParameters::ORDER_RANDOM ) {
        GenerateOrderedTrialList(false);
        RandomizeTrialList(false);

        // add some random trials to be skipped
        if (doSkip) {
            std::vector<data::ApexTrial*> temp = m_procedureConfig->GetTrials();
            std::vector<data::ApexTrial*>::iterator i=temp.end();

            for (int count=0; count< m_procedureConfig->GetParameters()->m_skip; ++count) {
                if (i==temp.end()) {
                    std::random_shuffle( temp.begin(), temp.end(), randomGenerator);
                    i=temp.begin();
                }
                m_trialList.insert( m_trialList.begin(), *i);
                ++i;
            }

        }
    } else if (m_procedureConfig->GetParameters()->m_order == data::ApexProcedureParameters::ORDER_SEQUENTIAL ) {
        GenerateOrderedTrialList(doSkip);
    } else {
        Q_ASSERT ( 0 && "invalid order");
    }


    m_trialListIterator = m_trialList.begin();
    //--m_trialListIterator;
}

/**
 * Create a trial list that is not randomized: every trial is put m_presentations times
 * in the list in the order defined in the experiment file. In other words: the resulting
 * trial list is the m_presentations time concatenation of the experiment file trial list.
 */
void ApexProcedure::GenerateOrderedTrialList(const bool doSkip) {
    // initialize vector size
    int skip=0;
    if (doSkip)
        skip= m_procedureConfig->GetParameters()->m_skip;
    int presentations = m_procedureConfig->GetParameters()->m_presentations;

    int totalTrials = presentations*
                      (int)( m_procedureConfig->GetTrials().size() ) + skip;
    m_trialList.reserve(totalTrials);

    const std::vector<data::ApexTrial*> &temp = m_procedureConfig->GetTrials();
    std::vector<data::ApexTrial*>::const_iterator i;

    i=temp.begin();
    for (int count=0; count<skip; ++count) {
        m_trialList.push_back( *i );
        ++i;
    }

    for (int presentation=0; presentation<presentations; ++presentation) {
        for (i=temp.begin(); i!= temp.end(); ++i) {
            m_trialList.push_back( *i );
        }
    }
}

/**
 * Randomize the current trial list
 *
 * The first m_skip*nTrials elements are randomized separately
 */
void ApexProcedure::RandomizeTrialList(const bool doSkip) {
    int skip=0;
    if (doSkip)
        skip = m_procedureConfig->GetParameters()->m_skip;

    if (skip)
        std::random_shuffle( m_trialList.begin(), m_trialList.begin()+skip-1, randomGenerator);
    std::random_shuffle( m_trialList.begin()+skip, m_trialList.end(), randomGenerator);
}

/**
 * This method is called upon experiment completion
 */
void ApexProcedure::ExperimentFinished( ) {
    m_bFinished=true;
    m_bStarted = false;
#ifdef PRINTPROCEDURE

    qDebug("Experiment finished");
#endif

    emit (Finished());
}

/**
 * Select the next trial
 * Next StartOutput should be called
 * @param
 */
bool ApexProcedure::NextTrial( bool, const ApexScreenResult* ) {
#ifdef SHOWSLOTS
    qDebug("SLOT ApexProcedure::NextTrial( bool result )");
#endif

    m_rd.GetParameterManager()->reset();           // reset all parameters that can be reset to their correct values

    if (m_bFinished) {
        //              Q_ASSERT("ApexProcedure::NextTrial: m_bFinished==true, should not come here");
        return true;
    }
    ++m_nTrials;

    return true;
}

/**
 * This method is to be called upon stimulus completion. It will
 * - select the next stimulus to be output; or
 * - end stimulus output for this trial
 */
void ApexProcedure::StimulusDone( )
{
#ifdef SHOWSLOTS
    qDebug("SLOT ApexProcedure::StimulusDone");
#endif

    //[ controlthread ]
    //emit StimulusEnd();

    if (!m_outputList.empty()) {

        QString t = m_outputList.front();
        m_outputList.pop_front() ;
#ifdef PRINTPROCEDURE

        qDebug("ApexProcedure: emitting NewScreenElement("  + GetCurrentScreenElement(m_nCurrentOutputStimulus) + ")");
#endif

        emit (NewScreenElement( GetCurrentScreenElement(m_nCurrentOutputStimulus)));


        // pause if spcecified in the experiment file
        if (m_nCurrentOutputStimulus>1 && m_procedureConfig->GetParameters()->m_pause_between_stimuli) {
                 appcore::IThread::sf_Sleep( m_procedureConfig->GetParameters()->m_pause_between_stimuli );
        }


        m_rd.modRandomGenerators()->ApplyGenerators();
        ++m_nCurrentOutputStimulus;                     // we put this here because if NewStimulus returns quasi immediately it wouldn't be incremented

        //#define NO_OUTPUT
#ifndef NO_OUTPUT
        emit NewStimulus( t );
#endif


    } else {
        // if not already enabled, enable
        if ( !m_procedureConfig->GetParameters()->m_input_during_stimulus)
            emit(EnableScreen(true));
        ApexControl::Get().StartTimer();
    }

    m_bFirstPresentation=false;
}

/**
 * Create list of stimuli to be output according to p_trial
 * @param p_trial trial to get the stimuli from
 * @param p_stimulus stimulus to be used, if ==NULL this method will select a random stimulus from the trial
 */
void ApexProcedure::CreateOutputList( data::ApexTrial * p_trial, Stimulus* p_stimulus ) {
    int nChoices = m_procedureConfig->GetParameters()->m_choices.nChoices;               // if choices is undefined, it is set to -1, so the rest of this method is correct

    QString useStimulus;
    if (p_stimulus!=NULL) {
        useStimulus = p_stimulus->GetID();
    } else {
        //useStimulus = p_trial->GetStimulus();
            useStimulus = p_trial->GetRandomStimulus();
        // FIXME: if more than one stimulus present, use random stimulus
    }

    m_currentStimulus = useStimulus;
#ifdef PRINTPROCEDURE

    qDebug("CurrentStimulus = " + useStimulus);
#endif

    // Set parameters from randomgenerators
//    ApexControl::Get().GetRandomGenerators()->ApplyGenerators();

    // Set parameters from SpinBoxes
    const stimulusParamsT& newSP=m_rd.modScreen()->GetLastResult()->GetStimulusParameters();
    //StimulusParameters* stimp = ApexControl::Get().GetStimulus(useStimulus)->GetVarParameters();
    // overwrite existing parameters!! (std::copy doesn't do this)
    for ( stimulusParamsT::const_iterator it=newSP.begin(); it!=newSP.end(); ++it) {
        //(*stimp)[ it->first ] =  it->second;
        m_rd.GetParameterManager()->setParameter(it->first, it->second, true);
    }

    emit( NewStimulus( m_rd.GetStimulus(useStimulus) ) );

    if (nChoices==-1 || nChoices==1) {
        m_outputList.push_back( useStimulus );
        return;
    } else if (nChoices==0) {
        qDebug("Error: nChoices==0, should not happen");
    }

    // determine random stimulus position
    int pos;
    const data::Choices& c = m_procedureConfig->GetParameters()->GetChoices();
    while (1) {
        pos = randomGenerator.nextUInt(nChoices) + 1;
        //pos= ApexTools::RandomRange(1, nChoices );

        data::Choices::const_iterator it;

        it = std::find(c.begin(), c.end(), pos);
        if (it!=c.end()) break;
    }
#ifdef PRINTPROCEDURE

    qDebug("Putting stimulus in position " + QString::number(pos) + " of " + QString::number(nChoices) );
#endif
    emit( SetCorrectAnswer(pos));                               // [Tom] this is necessary for corrector!!

    m_outputList.clear();

    bool useDefaultStandard=false;
    QString defaultStandard;
    if ( p_trial->GetRandomStandard().isEmpty()) {
        defaultStandard = m_procedureConfig->GetParameters()->m_defaultstandard;
        useDefaultStandard=true;
    }

    bool useStandardList=false;
    tStimulusList standardList;
    tStimulusList::const_iterator standardListIterator;
    if ( m_procedureConfig->GetParameters()->GetUniqueStandard()) {
        // get standard list
        //standardList = p_trial->GetStandards();
        standardList = GetStandards(p_trial);
        if (standardList.size()+1 < nChoices) {
            qDebug("Error: can't create unique standardlist because the number of standards is to small in trial %s", qPrintable (p_trial->GetID()));
        } else {

            // randomize standard list
            // we need a vector instead
            std::random_shuffle( standardList.begin(), standardList.end(), randomGenerator);
            standardListIterator=standardList.begin();
            useStandardList=true;
        }
    }


    m_currentStandards.clear();
    if (useDefaultStandard)
        m_currentStandards.append( defaultStandard );

    int i;
    for (i=1; i<=nChoices; ++i) {
        if (i==pos) {
            m_outputList.push_back( useStimulus );

        } else {
            if (!useDefaultStandard) {
                QString std;
                if (useStandardList) {
                    std = *standardListIterator;
                    qDebug("Using standard %s", qPrintable (std));
                    ++standardListIterator;
                } else
                    std = GetRandomStandard(GetStandards(p_trial));
//                ApexControl::Get().GetRandomGenerators()->ApplyGenerators(std);
                m_outputList.push_back( std );
                m_currentStandards.append(std);
            } else {
                m_outputList.push_back( defaultStandard );
            }
        }
    }


    //if (nChoices>1)
    m_CurrentCorrectAnswer=pos;

    //emit( NewSignalStimulus(useStimulus));

}


const tStimulusList ApexProcedure::GetStandards( data::ApexTrial* trial) {
    return trial->GetStandards();
}

/**
 *
 * @return empty string if no standard is present
 */
const QString ApexProcedure::GetRandomStandard( const tStimulusList& stdList ) const
{

    if (stdList.size()>1) {
        int pos = randomGenerator.nextUInt(stdList.size());
        //int pos = ApexTools::RandomRange((int)stdList.size()-1) ;
        qDebug ("Returning random standard #%i", pos);
        tStimulusList::const_iterator it= stdList.begin();
        for (int i=0; i<pos; ++i)
            ++it;
        return (*it);
    } else if (stdList.size()==0) {
        return QString();
    } else {
        return( stdList.front() );
    }


        //return m_standard;
}

/**
 * Start output of stimuli in the output list
 */
void ApexProcedure::StartOutput( )
{
        //SCREEN m_parent->Repaint();

    if (!m_bStarted)
        return;

    m_lastOutputList = m_outputList;

//    Q_ASSERT( !m_outputList.empty());           // [FIXME?] can be empty sometimes
    if (m_outputList.empty()) {
        qDebug("Warning: outputlist empty");
        return;
    }


#ifdef PRINTPROCEDURE
    qDebug("Output list:");
    for (std::list<QString>::const_iterator it=m_outputList.begin(); it!=m_outputList.end(); ++it) {
        qDebug(*it);
    }
#endif


    m_nCurrentOutputStimulus=1;

    // enable screen if config file says so
    if ( m_procedureConfig->GetParameters()->m_input_during_stimulus)
        emit(EnableScreen(true));

    if ( m_bFirstPresentation &&  m_procedureConfig->GetParameters()->m_time_before_first_stimulus) {


        stimulus::tDeviceMap& devs = m_rd.GetDevices();
        for (stimulus::tDeviceMap::const_iterator it=devs.begin(); it!=devs.end(); ++it) {
//            if ( (*it).second->CanOffLine()) {       // it is a wav device
//                stimulus::WavDevice* d = (stimulus::WavDevice*) (*it).second;
                it.value()->SetSilenceBefore(m_procedureConfig->GetParameters()->m_time_before_first_stimulus);
//            }
        }
    }

    StimulusDone();


    return;

}

void ApexProcedure::NewTrial( const QString & p_name )
{
    ApexControl::Get().SetCurrentTrial(p_name);
}

QString ApexProcedure::XMLHeader() const
{
#ifdef PRINTPROCEDURE
    qDebug("ApexProcedure::XMLHeader(): id=" + m_id);
#endif

    QString temp("<procedure %1>\n");
    temp = temp.arg( XMLID());
    return temp;
}

QString ApexProcedure::XMLFooter() const
{
    return QString("</procedure>");
}

QString ApexProcedure::XMLBody()  const
{
    QString temp;

    if (m_CurrentCorrectAnswer!=-1) {
        temp+= "<correct_answer>%1</correct_answer>\n";
        temp=temp.arg(m_CurrentCorrectAnswer);
    }

    if ( m_nTrials<=m_procedureConfig->GetParameters()->m_skip)
        temp += "<skip/>\n";

    temp+="<stimulus>"+m_currentStimulus+"</stimulus>\n";

    for (int i=0; i<m_currentStandards.size(); ++i) {
        temp+="<standard>"+m_currentStandards.at(i)+"</standard>\n";
    }

    return temp;
}

QString ApexProcedure::GetResultXML( ) const
{
    //  qDebug("ApexProcedure::GetResultXML, id=" + m_id);
    QString temp = XMLHeader() + XMLBody() + XMLFooter();

    return temp;
}

void ApexProcedure::Stop()
{
    if( m_bStarted )
    {
        m_bStarted = false;
        emit Finished();
    }
}

void ApexProcedure::Start()
{
    if( !m_bStarted ) {
        m_bStarted = true;
        return StartOutput();
    }
    ErrorHandler::Get().addError( "ApexConstantProcedure::Start()", "Already Started" );
}

QString ApexProcedure::XMLID( ) const
{
    QString temp;
    if (!m_id.isEmpty()) {
        temp = "id=\"" + m_id + "\"";
    }
    return temp;
}

/**
 *
 * @param p_position current position in output list
 * @return the name of the screen element corresponding to the given trial
 */
QString ApexProcedure::GetCurrentScreenElement(int p_position ) {
#ifdef PRINTPROCEDURE
    qDebug("GetCurrentScreenElement: position=" + QString::number(p_position));
#endif

    return m_rd.modCorrector()->GetCorrectAnswer (p_position);
}

/**
 * Repeat the last (sound) output, including standards
 */
void ApexProcedure::RepeatOutput( )
{
    Q_ASSERT( m_lastOutputList.size());
    m_outputList = m_lastOutputList;
    StartOutput();
}

}


