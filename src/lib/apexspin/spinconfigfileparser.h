/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _EXPORL_SRC_LIB_APEXSPIN_SPINCONFIGFILEPARSER_H_
#define _EXPORL_SRC_LIB_APEXSPIN_SPINCONFIGFILEPARSER_H_

#include "apextools/global.h"

#include "apextools/xml/xercesinclude.h"

#include <QString>

namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
class DOMNode;
class DOMNodeList;
}

namespace spin
{

namespace data
{
class SpinConfig;
struct CategoryMap;
}

namespace parser
{

class APEXSPIN_EXPORT SpinConfigFileParser
{
    public:

        static data::SpinConfig parse(QString file, QString schema);

    private:

        static void parsePrefix(XERCES_CPP_NAMESPACE::DOMNode* prefix,
                                data::SpinConfig* into);
        static void parseSpeakers(XERCES_CPP_NAMESPACE::DOMNode* speakers,
                                  data::SpinConfig* into);
        static void parseNoises(XERCES_CPP_NAMESPACE::DOMNode* noises,
                                data::SpinConfig* into);
        static void parseSpeech(XERCES_CPP_NAMESPACE::DOMNode* speechmat,
                                data::SpinConfig* into);
        static void parseSpeechFiles(XERCES_CPP_NAMESPACE::DOMNode* speechfiles,
                                data::SpinConfig* into, QString path, QString schema);
        static void parseSubjectScreen(XERCES_CPP_NAMESPACE::DOMNode* screen,
                                data::SpinConfig* into);
        static void parseExperimenterScreenQuiet(XERCES_CPP_NAMESPACE::DOMNode* screen,
                                       data::SpinConfig* into);
        static void parseExperimenterScreenNoise(XERCES_CPP_NAMESPACE::DOMNode* screen,
                                       data::SpinConfig* into);
        static void parseCustomScreens(XERCES_CPP_NAMESPACE::DOMNode* screens,
                                            data::SpinConfig* into);
        static void parseIds(XERCES_CPP_NAMESPACE::DOMNode* ids,
                             data::SpinConfig* into);
        static void parseGeneralData(XERCES_CPP_NAMESPACE::DOMNode* data,
                                     data::SpinConfig* into);
        static void parseSoundcardSetup(XERCES_CPP_NAMESPACE::DOMNode* data,
                                     data::SpinConfig* into);
        static void addLists(XERCES_CPP_NAMESPACE::DOMNodeList* lists,
                             data::CategoryMap* into, QString category = "");
        static QStringList expandWildcards(QString directory, QString fileIdentifier);
};

} //ns parser
} //ns spin

#endif
