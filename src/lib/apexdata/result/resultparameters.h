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

#ifndef APEXRESULTPARAMETERS_H
#define APEXRESULTPARAMETERS_H

//from libdata
#include "parameters/apexparameters.h"

#include "xml/xercesinclude.h"
using namespace xercesc;


#include <QString>
#include <QPair>
#include <QList>
#include <QUrl>

namespace apex {
namespace data
{

    typedef QList<QPair<QString,QString> > tXsltParameters;

/**
Stores info on how the results are to be processed

@author Tom Francart,,,
*/
class APEXDATA_EXPORT ResultParameters : public ApexParameters
{
public:
    ResultParameters(DOMElement* p_paramElement);
    ResultParameters();

    ~ResultParameters();

    virtual bool SetParameter(const QString& arg1, const QString& arg2, const QString& arg3, DOMElement* arg4);


    const QString& GetXsltScript() const { return m_xsltScript; };
    void setXsltScript(const QString& script){m_xsltScript = script;};
    const QString& GetMatlabScript() const { return m_matlabScript; };
    const QString& GetSubject() const { return m_subject; };
    void setSubject(const QString& subject){m_subject = subject;};

    bool showXsltResultsAfter() const { return m_bShowResultsXslt;  };
    bool showJavascriptResultsAfter() const { return m_showAfter; };
    bool showResultsAfter() const {
        return showXsltResultsAfter() || showJavascriptResultsAfter();
    };
    void setShowResults(bool show) {m_bShowResultsXslt = show;};
    bool GetSaveResults() const { return m_bSaveProcessedResults; };
    void setSaveResults(bool save) {m_bSaveProcessedResults=save; };

    const QUrl& resultPage() const { return m_resultPage; };
    void setResultPage(const QString &scriptname) { m_resultPage=scriptname; };
    bool showRTResults() const { return m_showRealtime;};

    tXsltParameters GetXsltParameters() const
    { return mXsltParameters; };
    void setXsltParameter(const QString& name, const QString& value);

    bool operator==(const ResultParameters& other) const;

  private:
    QString m_xsltScript;
    QString m_matlabScript;
    QString m_subject;
    
    QUrl m_resultPage;
    bool m_showRealtime;
    bool m_showAfter;

    bool m_bShowResultsXslt;
    bool m_bSaveProcessedResults;

    tXsltParameters mXsltParameters;


};

}
}

#endif
