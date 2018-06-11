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

#ifndef _APEX_SRC_LIB_APEXMAIN_GUI_STUDYDIALOG_H_
#define _APEX_SRC_LIB_APEXMAIN_GUI_STUDYDIALOG_H_

#include "apextools/global.h"

#include <QDialog>

namespace apex
{
class StudyDialogPrivate;
class APEX_EXPORT StudyDialog : public QDialog
{
    Q_OBJECT

public:
    StudyDialog(QWidget *parent = nullptr);
    virtual ~StudyDialog();

    void setPublicKey(const QString &key);
    void setRootPath(const QString &path);

public Q_SLOTS:
    void updateStudies(const QStringList &studies, const QString &activeStudy,
                       const QString &statusMessage);
    void updateStudyStatusMessage(const QString &statusMessage);

    void linkStudySetExperimentsBranches(const QStringList &branches);
    void linkStudySetResultsBranch(const QString &branch);
    void linkStudyStatusUpdate(const QString &statusUpdate);
    void linkStudyProgress(int received, int total);
    void linkStudyFailed();

Q_SIGNALS:
    void sharePublicKey(const QString &publicKey);
    void activateStudy(const QString &study);
    void startActiveStudy();
    void pauseActiveStudy();
    void syncActiveStudy();
    void deleteStudy(const QString &study);

    void experimentsUrlSet(const QString &url);
    void linkStudy(QString name, QString experimentsUrl,
                   QString experimentsBranch, QString resultsUrl,
                   QString resultsBranch);

private:
    QScopedPointer<StudyDialogPrivate> d;
};
}

#endif
