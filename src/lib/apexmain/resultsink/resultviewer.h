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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RESULTVIEWER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RESULTVIEWER_H_

#include "apextools/global.h"

namespace apex
{

class RTResultSink;

class APEX_EXPORT ResultViewer : public QObject
{
    Q_OBJECT
public:
    ResultViewer(const QString &p_resultfile);
    void show(bool ask);
    bool addtofile(const QString &p_filename);
    RTResultSink *getResultSink();

    ~ResultViewer();

Q_SIGNALS:
    void viewClosed();

private Q_SLOTS:
    void loadingFinished(bool ok);

private:
    QString findResultPage(const QString &resultFilePath);
    QString findExtraScript(const QString &resultFilePath);
    QMap<QString, QString> findResultParameters(const QString &resultFilePath);

    QString m_sResultfile;
    QScopedPointer<RTResultSink> m_rtResultSink;
};
}

#endif
