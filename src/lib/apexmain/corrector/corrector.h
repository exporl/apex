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

#ifndef CORRECTOR_H
#define CORRECTOR_H

#include "apexmodule.h"
#include "comparator.h"

#include <memory>

namespace apex {
    namespace data
    {
    class CorrectorData;
    }
    class ScreenResult;

    class Corrector : public ApexModule {
        Q_OBJECT
    public:
        Corrector (ExperimentRunDelegate& p_rd, const data::CorrectorData* data );
        virtual QString GetResultXML() const;

    public:
        virtual bool Correct (const ScreenResult& p_answer);
        virtual bool GetLastResult() const { return m_currentResult; };

        virtual const QString GetCorrectAnswer (unsigned p_position = 0) const;

    public slots:

        virtual void SetCorrectAnswer (const unsigned p_answer);
        virtual void SetComparator (Comparator* comparator);

    protected:
        bool Compare (const QString& m1, const QString& m2);
        const data::CorrectorData* const data;
        bool m_currentResult;

        QString m_currentAnswer;
        QString m_correctAnswer;
    private:
        std::auto_ptr<Comparator> comparator;
    };
}

#endif
