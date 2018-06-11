/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULIPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULIPARSER_H_

class QWidget;

namespace apex
{

namespace data
{
class StimuliData;
struct StimulusDatablocksContainer;
class StimulusParameters;
}

namespace parser
{

class APEX_EXPORT StimuliParser
{
public:
    StimuliParser(QWidget *parent = 0);

    void Parse(const QString &fileName, const QDomElement &p_base,
               data::StimuliData *c, const QString &scriptLibraryFile,
               const QVariantMap &scriptParameters, bool expand);

private:
    void ParseStimulus(const QDomElement &p_base);
    data::StimulusParameters
    CreateStimulusParameters(const QDomElement &a_pBase);
    data::StimulusDatablocksContainer
    CreateDatablocksContainer(const QDomElement &data);

    data::StimuliData *currentData;
    QWidget *m_parent;
};
}
}

#endif
