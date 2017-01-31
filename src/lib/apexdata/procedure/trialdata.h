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

#ifndef _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_TRIALDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_TRIALDATA_H_

#include <QString>
#include <QObject>
#include <QList>
//#include <vector>

//from libtools
#include "apextools/global.h"
#include "apextools/random.h"

namespace apex {


    typedef QList<QString> tStimulusList;

    namespace data
    {

/**
Trial class

@author Tom Francart,,,
*/
class APEXDATA_EXPORT TrialData: public QObject{            // must be QObject for compatibility with QtScript
    Q_OBJECT

    Q_PROPERTY( QString id WRITE SetID READ GetID )
    Q_PROPERTY( QString screen WRITE SetScreen READ GetScreen )

    public:
        TrialData();
        ~TrialData();

        bool operator==(const TrialData& other) const;

public slots:           // have to be slots to be used in QtScript
        void SetAnswer(const QString& p_answer);
        void SetAnswerElement(const QString& p_element) {m_answer_element=p_element;};
        void SetScreen(const QString& p_answer);
        void AddStimulus(const QString& p_answer);
        //void SetStandard(const QString& p_name);        // TODO: obsolete
        void AddStandard(const QString& p_name);
        void SetID(const QString& p_id);

        const QString GetAnswer() const;
        const QString GetStimulus(QString& p_name) const;
        const QString GetStimulus() const;              // cannot return a reference for QtScript
        const QString GetRandomStimulus( ) const;
        const QString GetRandomStandard() const;
        const tStimulusList& GetStandards() const { return m_standards;};
        const tStimulusList::const_iterator GetStimulusIterator() const;
        const tStimulusList& GetStimuli() const {return m_stimuli;};

        const QString GetID() const {return m_ID;};
        const QString GetScreen() const {return m_screen;};
        const QString GetAnswerElement() const;



        private:
                QString m_answer;
                QString m_answer_element;                       // screen element containing the answer to this trial
                QString m_screen;
                tStimulusList m_stimuli;
                QString m_ID;
                //QString m_standard;
                tStimulusList m_standards;
                mutable Random mRandom;

};


typedef QList<TrialData*> tTrialList;

}
}
#endif
