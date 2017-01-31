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
 
#include "cvccomparator.h"

//from libdata
#include "corrector/correctordata.h"

namespace apex {

CvcComparator::CvcComparator (int language)
        : Comparator(),
        m_lastScore(-1) {

    if (language==data::CorrectorData::DUTCH) {
        SetDutch();
    } else if (language==data::CorrectorData::ENGLISH) {
        SetEnglish();
    }


}


CvcComparator::~CvcComparator() {}



bool apex::CvcComparator::Compare( const QString & m1, const QString & m2 ) {
    qDebug( "CvcComparator::Compare");

    std::vector<int> tph = getphonemes(m1);             // target = gold standard
    std::vector<int> inph = getphonemes(m2);    // input = user input

    unsigned score=0;
    unsigned j=0;
    int maxerr = qAbs( (int) tph.size() - (int) inph.size());
    int found=0;
    int lastfound=0;

    for (unsigned i=0; i!=tph.size(); ++i) {
                found=0;
                for (j=lastfound; j<=i+maxerr; ++j) {
                        if ( inph.at(j) == tph.at(i)) {
                                ++score;
                                ++j;
                                found=1;
                                lastfound=j;
                                break;
                        }
                }
                if (found) {
                        j=i+1;
                }
    }

        m_lastScore = score;
        return (score==tph.size());

}



bool apex::CvcComparator::TestAnswer( const QString & /*p_answer*/ ) const { return false; }


}

/**
 * set internal structures to dutch materials
 */
void apex::CvcComparator::SetDutch( ) {
        int count=1;
        m_phonCode.reserve(30);

        m_phonCode.push_back(tPhonCode(QString("a"),count++));
        m_phonCode.push_back(tPhonCode(QString("b"),count++));
        m_phonCode.push_back(tPhonCode(QString("c"),count++));
        m_phonCode.push_back(tPhonCode(QString("d"),count++));
        m_phonCode.push_back(tPhonCode(QString("e"),count++));
        m_phonCode.push_back(tPhonCode(QString("f"),count++));
        m_phonCode.push_back(tPhonCode(QString("h"),count++));
        m_phonCode.push_back(tPhonCode(QString("i"),count++));
        m_phonCode.push_back(tPhonCode(QString("j"),count++));
        m_phonCode.push_back(tPhonCode(QString("k"),count++));
        m_phonCode.push_back(tPhonCode(QString("l"),count++));
        m_phonCode.push_back(tPhonCode(QString("m"),count++));
        m_phonCode.push_back(tPhonCode(QString("n"),count++));
        m_phonCode.push_back(tPhonCode(QString("o"),count++));
        m_phonCode.push_back(tPhonCode(QString("p"),count++));
        m_phonCode.push_back(tPhonCode(QString("q"),count++));
        m_phonCode.push_back(tPhonCode(QString("r"),count++));
        m_phonCode.push_back(tPhonCode(QString("s"),count++));
        m_phonCode.push_back(tPhonCode(QString("t"),count++));
        m_phonCode.push_back(tPhonCode(QString("u"),count++));
        m_phonCode.push_back(tPhonCode(QString("v"),count++));
        m_phonCode.push_back(tPhonCode(QString("w"),count++));
        m_phonCode.push_back(tPhonCode(QString("x"),count++));
        m_phonCode.push_back(tPhonCode(QString("y"),count++));
        m_phonCode.push_back(tPhonCode(QString("z"),count++));

        m_phonCode.push_back(tPhonCode(QString("oe"),count++));
        m_phonCode.push_back(tPhonCode(QString("ui"),count++));
        m_phonCode.push_back(tPhonCode(QString("aa"),count++));
        m_phonCode.push_back(tPhonCode(QString("ee"),count++));
        m_phonCode.push_back(tPhonCode(QString("ie"),count++));
        m_phonCode.push_back(tPhonCode(QString("oo"),count++));
        m_phonCode.push_back(tPhonCode(QString("uu"),count++));

        m_phonCode.push_back(tPhonCode(QString("ng"),count++));
        m_phonCode.push_back(tPhonCode(QString("au"),count++));

        m_phonCode.push_back(tPhonCode(QString("ij"),count));
        m_phonCode.push_back(tPhonCode(QString("ei"),count++));

        m_phonCode.push_back(tPhonCode(QString("ou"),count));
        m_phonCode.push_back(tPhonCode(QString("au"),count++));

        m_phonCode.push_back(tPhonCode(QString("uw"),count));
        m_phonCode.push_back(tPhonCode(QString("w"),count++));

        m_phonCode.push_back(tPhonCode(QString("ch"),count));
        m_phonCode.push_back(tPhonCode(QString("g"),count++));
}


/**
 * set internal structures to english materials
 */
void apex::CvcComparator::SetEnglish( ) {
    Q_ASSERT("English not supported");
    throw 0;
}

/**
 * c++ version of perl reference script
 * @param word
 * @return
 */
std::vector<int> apex::CvcComparator::getphonemes (const QString& word) const {
        qDebug("Finding phonemes in %s", qPrintable (word));


        std::vector<int> result;

        int current;
        int currlen;

        for (int i=0; i<word.length(); i=i+currlen) { //..
                current=-1;
                currlen=1;

                for (std::vector<tPhonCode>::const_iterator j=m_phonCode.begin(); j!=m_phonCode.end(); ++j ) {
                        if ( word.mid(i, (*j).phoneme.length() ) ==   (*j).phoneme ) {
                                if (current==-1 || (*j).phoneme.length()>currlen) {
                                        current = (*j).code;
                                        currlen = (*j).phoneme.length();
                                }
                        }
                }

                if (current==-1) {
                        qDebug("Error in getphonemes");
                }

                result.push_back(current);
                qDebug("Found phoneme #%i", current);
        }

        return result;
}

QString apex::CvcComparator::GetResultXML( ) const
{
        return QString("<score>%1</score>\n").arg(m_lastScore);
}
