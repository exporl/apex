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
 
#include "correctordata.h"

#include <QString>
#include <QDebug>

namespace apex
{

namespace data
{

struct CorrectorDataPrivate
{
    CorrectorData::Type      type;
    CorrectorData::Language  language;
    CorrectorData::AnswerMap answers;
    bool                     answersDefined;
};

}//ns data

}//ns apex

using namespace apex::data;


CorrectorData::CorrectorData() : d(new CorrectorDataPrivate())
{
    d->language = DUTCH;
}


CorrectorData::CorrectorData(const CorrectorData& other) :
                                    d(new CorrectorDataPrivate(*other.d))
{
}


CorrectorData::~CorrectorData()
{
    delete d;
}

void CorrectorData::setType(const Type type)
{
    d->type = type;
}

CorrectorData::Type CorrectorData::type() const
{
    return d->type;
}

bool CorrectorData::answersDefined() const
{
    return d->answersDefined;
}

void CorrectorData::setAnswersDefined(const bool answersDefined)
{
    d->answersDefined = answersDefined;
}

CorrectorData::Language CorrectorData::language() const
{
    return d->language;
}

void CorrectorData::setLanguage(const CorrectorData::Language language)
{
    qDebug() << "setting lang to" << language;
    d->language = language;
}

void apex::data::CorrectorData::addAnswer(int number, const QString& value)
{
    d->answers[number] = value;
}

const CorrectorData::AnswerMap& CorrectorData::answers() const
{
    return d->answers;
}

// CorrectorData::AnswerMap& CorrectorData::answers()
// {
//     return d->answers;
// }


CorrectorData& CorrectorData::operator=(const CorrectorData& other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}

bool CorrectorData::operator==(const CorrectorData& other)
{
    qDebug() << d->type << other.d->type;
    qDebug() << d->language << other.d->language;
    qDebug() << d->answersDefined << other.d->answersDefined;
    qDebug() << d->answers << other.d->answers;
    return  d->type == other.d->type &&
            d->language == other.d->language &&
            d->answersDefined == other.d->answersDefined &&
            d->answers == other.d->answers;
}
