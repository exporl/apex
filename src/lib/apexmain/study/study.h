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

#ifndef _APEX_SRC_LIB_APEXMAIN_STUDY_STUDY_H_
#define _APEX_SRC_LIB_APEXMAIN_STUDY_STUDY_H_

#include "apextools/global.h"

namespace apex
{

class StudyPrivate;

class APEX_EXPORT Study : public QObject
{
    Q_OBJECT
public:
    Study(const QString &name, const QString &experimentsUrl,
          const QString &experimentsBranch, const QString &resultsUrl,
          const QString &resultsBranch, const QString &rootPath,
          const QString &resultsWorkdirRootPath);
    virtual ~Study();

    bool isPublic() const;
    bool isPrivate() const;

    void updateExperiments();
    void fetchExperiments(bool blocking = false);
    void checkoutExperiments(bool blocking = false);
    /* May throw */
    void addResult(const QString &path);
    void updateResults(bool blocking = false);
    /* Potentially destructive, will discard results not in remote.
     * May throw.
     */
    void pullResults();

    QString name() const;
    QString experimentsUrl() const;
    QString experimentsBranch() const;
    QString resultsUrl() const;
    QString resultsBranch() const;
    QString experimentsPath() const;

    QString indexExperiment() const;
    QStringList experiments() const;
    bool belongsToStudy(const QString &path) const;
    const QString
    makeResultfilePath(const QString &experimentfilePath,
                       const QString &relativeResultfilePath) const;
    const QString makeResultsPath(const QString &relativePath) const;
    QString statusMessage();
    const QString getDocumentationPath() const;

Q_SIGNALS:
    void updateExperimentsProgress(int received, int total);
    void updateResultsProgress(int sent, int total);

    void updateExperimentsDone();
    void updateExperimentsFailed();
    void updateResultsDone();
    void updateResultsFailed();

private slots:
    void makeDocumentationAccessible() const;

private:
    const QString createResultfilePathRoot() const;
    const QString createUniqueResultfilePathFragmentToAllowOctopusMerge() const;
    const QString
    createRelativeResultfilePath(const QString &experimentfilePath,
                                 const QString &relativeResultfilePath) const;

    QScopedPointer<StudyPrivate> d;
};
} // namespace apex

#endif
