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

#ifndef _APEX_SRC_LIB_APEXMAIN_MAINCONFIGFILEPARSER_H_
#define _APEX_SRC_LIB_APEXMAIN_MAINCONFIGFILEPARSER_H_

#include "apexdata/mainconfigfiledata.h"

#include "apextools/global.h"

#include "upgradablexmlparser.h"

namespace apex
{

class APEX_EXPORT MainConfigFileParser : public UpgradableXmlParser
{
    Q_OBJECT
public:
    MainConfigFileParser();
    MainConfigFileParser(const QString &fileName);
    virtual ~MainConfigFileParser();

    static MainConfigFileParser &Get();

    bool parse();

    const data::MainConfigFileData &data() const;

private Q_SLOTS:
    void upgrade3_1_3();

private:
    data::MainConfigFileData m_data;
};
}
#endif
