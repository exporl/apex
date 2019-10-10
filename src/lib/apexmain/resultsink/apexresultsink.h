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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_APEXRESULTSINK_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_APEXRESULTSINK_H_

#include "apexmain/apexmodule.h"
#include "apextools/global.h"

#include <QDateTime>
#include <QTextStream>

#include <vector>

namespace apex
{

class TrialResult;

/**
Apex result collector module

@author Tom Francart,,,
*/
class APEX_EXPORT ApexResultSink : public ApexModule
{
    Q_OBJECT
public:
    ApexResultSink(ExperimentRunDelegate &runDelegate);
    ~ApexResultSink();

    virtual QString GetResultXML() const;

    void setFilename(const QString &resultfilePath);

    // Extra XML will be appended to the results file
    void setExtraXml(const QString &extraXml);

    // provide information about how the experiment was stopped
    void setStopCondition(const bool &stoppedByUser);

    void setExperimentStartTime(const QDateTime &experimentStartTime);
    void setExperimentEndTime(const QDateTime &experimentEndTime);

    const QString saveResultfile();
    void collectResults(const QString &trial, const QString &extraXml);

    static const QString resultfileExtension;
    static const QString resultfileXmlClosingTag;

signals:
    void collected(QString xml);

protected:
    virtual const QString askSaveLocation(const QString &suggestedPath);
    virtual bool askShouldDiscardResults();
    virtual void informSavingFailed(const QString &path);
    virtual bool save(const QString &path);

private:
    QString tryToSave(const QString &suggestedPath,
                      const bool allowDeviationFromSuggestion = true);

    void printXMLHeader(QTextStream &out) const;
    // print general stuff about the experiment
    void printIntro(QTextStream &out) const;
    const QString collectEndResults() const;
    void printXMLFooter(QTextStream &out) const;

    QString resultfilePath;
    std::vector<TrialResult *> trialResults;
    QDateTime experimentStartTime;
    QDateTime experimentEndTime;
    QString stopCondition;
    QString extraXml;
};
}

#endif
