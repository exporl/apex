/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
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

#ifndef _APEX_SRC_LIB_APEXANDROID_APEXANDROIDNATIVE_H_
#define _APEX_SRC_LIB_APEXANDROID_APEXANDROIDNATIVE_H_

#include "apextools/global.h"

#include <jni.h>

#include <QObject>
#include <QString>

namespace apex
{
namespace android
{
class APEX_EXPORT ApexAndroidBridge : public QObject
{
    Q_OBJECT

public:
    static ApexAndroidBridge *instance();
    static void signalApexInitialized();
    static void signalExperimentStarted();
    static void signalExperimentFinished();
    static void signalTrialStarted();
    static void forceAudioVolumeToMaximum();
    static QString getDeviceSerialNumber();
    static QString getSSLCertificateDirectory();
    static void startGdbServer();
    static void shareText(const QString &text);
    static void sendToast(const QString &text);
    static void openBrowser(const QString &uri);

    ApexAndroidBridge();

public Q_SLOTS:

    void createShortcutToFile();
    void createShortcutToRunner();
    void enableKioskMode();
    void disableKioskMode();

Q_SIGNALS:
    void networkAvailable();
    void linkStudy(const QString &name, const QString &experimentsUrl,
                   const QString &experimentsBranch, const QString &resultsUrl,
                   const QString &resultsBranch);

private Q_SLOTS:
    void onNetworkAvailable();
    void onLinkStudy(const QString &name, const QString &experimentsUrl,
                     const QString &experimentsBranch,
                     const QString &resultsUrl, const QString &resultsBranch);
};
}
}
#endif
