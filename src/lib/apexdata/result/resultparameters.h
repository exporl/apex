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

    virtual bool SetParameter(const QString &arg1, const QString &arg2,
                              const QString &arg3, const QDomElement &arg4);

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

    tScriptParameters resultParameters() const;

    const QString extraScript() const;

    // New functions for version 3.1.1

    void setResultParameter(const QString &name, const QString &value);

    bool operator==(const ResultParameters &other) const;

private:
    QString m_matlabScript;
    QString m_subject;
    QString m_extraScript; // javascript code to be executed after loading the
                           // results page, from tag <resultscript>

    QUrl m_resultPage;
    bool m_showRealtime;
    bool m_showAfter;

    bool m_bSaveProcessedResults;

    tScriptParameters mResultParameters;
};
}
}

#endif
