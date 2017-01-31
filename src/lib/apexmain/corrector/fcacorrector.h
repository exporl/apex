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
 
#ifndef FCACORRECTOR_H
#define FCACORRECTOR_H

#include "corrector.h"

namespace apex {

    /**
    Corrector for the Forced Choice n Alternatives procedure
    Expects <standards> in <procedure> and <answers> in <corrector>

    @author Tom Francart,,,
    */
    class FCACorrector : public Corrector
    {
        Q_OBJECT
    public:
        FCACorrector (ExperimentRunDelegate& p_rd, const data::CorrectorData* data);

        virtual bool Correct (const ScreenResult& p_answer);

        virtual const QString GetCorrectAnswer (unsigned position = 0) const;
        
    public slots:
        virtual void SetCorrectAnswer (const unsigned p_answer);
    
    private:
        unsigned m_fcaCorrectAnswer;
    };
}

#endif
