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

#ifndef _APEX_SRC_LIB_APEXSPIN_SPINCONFIGFILEPARSER_H_
#define _APEX_SRC_LIB_APEXSPIN_SPINCONFIGFILEPARSER_H_

#include "apexmain/upgradablexmlparser.h"

#include "apextools/global.h"

#include <QString>

class QDomElement;
class QDomNodeList;

namespace spin
{

namespace data
{
class SpinConfig;
struct CategoryMap;
}

namespace parser
{

class APEXSPIN_EXPORT SpinConfigFileParser : public apex::UpgradableXmlParser
{
    Q_OBJECT
public:
    SpinConfigFileParser();
    SpinConfigFileParser(const QString &file);

    data::SpinConfig parse();

private Q_SLOTS:
    void upgrade3_1_3();

private:
    void parsePrefix(const QDomElement &prefix, data::SpinConfig *into);
    void parseSpeakers(const QDomElement &speakers, data::SpinConfig *into);
    void parseNoises(const QDomElement &noises, data::SpinConfig *into);
    void parseSpeech(const QDomElement &speechmat, data::SpinConfig *into);
    void parseSpeechFiles(const QDomElement &speechfiles,
                          data::SpinConfig *into, const QString &path);
    void parseSubjectScreen(const QDomElement &screen, data::SpinConfig *into);
    void parseExperimenterScreenQuiet(const QDomElement &screen,
                                      data::SpinConfig *into);
    void parseExperimenterScreenNoise(const QDomElement &screen,
                                      data::SpinConfig *into);
    void parseCustomScreens(const QDomElement &screens, data::SpinConfig *into);
    void parseIds(const QDomElement &ids, data::SpinConfig *into);
    void parseGeneralData(const QDomElement &data, data::SpinConfig *into);
    void parseSoundcardSetup(const QDomElement &data, data::SpinConfig *into);
    void addLists(const QDomNodeList &lists, data::CategoryMap *into,
                  const QString &category = QString());
    QStringList expandWildcards(const QString &directory,
                                const QString &fileIdentifier);
};
}
}

#endif
