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

#ifndef CORRECTORDATA_H
#define CORRECTORDATA_H

#include "global.h"

#include <QMap>

namespace apex
{

namespace data
{

struct CorrectorDataPrivate;

class APEXDATA_EXPORT CorrectorData
{
    public:

        enum Type
        {
            EQUAL,
            ALTERNATIVES,
            CVC
        };

        enum Language
        {
            DUTCH,
            ENGLISH
        };

        //[job] should this be inside CorrectorData?
        typedef QMap<int, QString> AnswerMap;

        CorrectorData();
        CorrectorData(const CorrectorData& other);
        ~CorrectorData();

        //getters
        Type type() const;
        bool answersDefined() const;
        Language language() const;
        const AnswerMap& answers() const;
        //AnswerMap& answers();

        //setters
        void setType(const Type type);
        void setAnswersDefined(const bool answersDefined);
        void setLanguage(const Language language);
        void addAnswer(int number, const QString& value);

        CorrectorData& operator=(const CorrectorData& other);
        bool operator==(const CorrectorData& other);

    private:

        CorrectorDataPrivate* d;
};
}
}

#endif
