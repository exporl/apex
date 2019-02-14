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

#ifndef _EXPORL_SRC_LIB_APEXDATA_RESULT_RESULTPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXDATA_RESULT_RESULTPARAMETERS_H_

#include "../parameters/apexparameters.h"

#include "apextools/xml/xmltools.h"

#include <QList>
#include <QPair>
#include <QString>
#include <QUrl>

namespace apex
{
namespace data
{

typedef QMap<QString, QString> tScriptParameters;

class APEXDATA_EXPORT ResultParameters
{
public:
    ResultParameters();
    virtual ~ResultParameters();

    bool Parse(const QDomElement &p_paramElement);

    const QString &matlabScript() const;

    const QString &subject() const;
    void setSubject(const QString &subject);

    bool showResultsAfter() const;
    void setShowResultsAfter(bool show);

    bool saveResults() const;
    void setSaveResults(bool save);

    const QUrl &resultPage() const;
    void setResultPage(const QString &scriptname);

    bool showRTResults() const;

    const QString extraScript() const;
    void setExtraScript(const QString &extraScript);

    tScriptParameters resultParameters() const;
    void setResultParameter(const QString &name, const QString &value);

    bool operator==(const ResultParameters &other) const;

private:
    bool SetParameter(const QString &p_name, const QString &p_value,
                      const QDomElement &p_elem);

    QString m_matlabScript;
    QString m_subject;
    QString m_extraScript;
    tScriptParameters m_resultParameters;

    QUrl m_resultPage;
    bool m_showRealtime;
    bool m_showAfter;
    bool m_bSaveProcessedResults;
};
}
}

#endif
