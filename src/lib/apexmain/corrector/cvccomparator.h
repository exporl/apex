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
 
#ifndef APEXCVCCOMPARATOR_H
#define APEXCVCCOMPARATOR_H

#include "comparator.h"

#include <vector>

#include <qstring.h>

namespace apex {

/**
Corrects Consonant-Vowel-Consonant type text inputs

@author Tom Francart,,,
*/
class CvcComparator : public Comparator
{
public:
    CvcComparator(int language);

    ~CvcComparator();

    virtual bool Compare(const QString& m1, const QString& m2);
    virtual bool TestAnswer(const QString& p_answer) const;

    virtual QString GetResultXML() const;

    private:
        void SetDutch();
        void SetEnglish();

        std::vector<int> getphonemes (const QString& word) const;

        struct tPhonCode {
                QString phoneme; int code;
                tPhonCode(QString p, int c): phoneme(p), code(c) {};
        };

    std::vector<tPhonCode> m_phonCode;

    int m_lastScore;


};

}

#endif
