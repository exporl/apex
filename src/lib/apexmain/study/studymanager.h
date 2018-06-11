/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#ifndef _APEX_SRC_LIB_APEXMAIN_STUDY_STUDYMANAGER_H_
#define _APEX_SRC_LIB_APEXMAIN_STUDY_STUDYMANAGER_H_

#include "study.h"

#include "apextools/apexpaths.h"
#include "apextools/global.h"

namespace apex
{

class StudyManagerPrivate;

class APEX_EXPORT StudyManager : public QObject
{
    Q_OBJECT

public:
    /* Use the global instance for networking related methods */
    static StudyManager *instance();

    StudyManager();
    virtual ~StudyManager();

    void setRootPath(const QString &path);
    void showConfigurationDialog();

    const QSharedPointer<Study> &activeStudy() const;
    QStringList studies() const;
    bool belongsToActiveStudy(const QString &path) const;
    QString newExperiment(const QString &experimentFilePath) const;
    void afterExperiment(const QString &resultsFilePath);

public Q_SLOTS:
    /* After these wait for the studiesUpdated signal before calling any
     * of these again to guarantee order. */
    void setActiveStudy(const QString &studyName);
    void linkStudy(const QString &name, const QString &experimentsUrl,
                   const QString &experimentsBranch, const QString &resultsUrl,
                   const QString &resultsBranch);

    void startActiveStudy();
    void pauseActiveStudy();
    void syncActiveStudy();
    void deleteStudy(const QString &studyName);

Q_SIGNALS:
    void studiesUpdated(const QStringList &studies, const QString &activeStudy,
                        const QString &activeStudyStatusMessage);
    void studyStatusMessageUpdated(const QString &statusMessage);

private Q_SLOTS:
    void sharePublicKey(const QString &key);
    void fetchRemoteBranches(const QString &remote);

private:
    QScopedPointer<StudyManagerPrivate> d;
};
}

#endif
