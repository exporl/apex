/******************************************************************************
 * Copyright (C) 2009 Michael Hofmann <mh21@piware.de>                        *
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

#include "bertha/devicedata.h"
#include "bertha/deviceprocessor.h"

#include "common/exception.h"

#include <QCoreApplication>
#include <QString>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif
#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

using namespace bertha;
using namespace cmn;

void callback(void *param)
{
    Q_UNUSED(param);
    qCDebug(EXPORL_BERTHA, "tata");
}

void milliSleep(unsigned millis)
{
#ifdef Q_OS_WIN32
    Sleep(millis);
#else
    usleep(1000 * millis);
#endif
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

#ifdef Q_OS_UNIX
    const QLatin1String filterName("Jack");
#else
    const QLatin1String filterName("Asio");
#endif

    DeviceData d(QLatin1String("device"), filterName);
    d.setBlockSize(2048);
    d.setSampleRate(48000);
    d.setInputPorts(2);
    d.setOutputPorts(2);

    try {
        DeviceProcessor processor(d);
        DeviceInterface *const device = processor.deviceInterface();

        device->setCallback(&callback, NULL);

        QString error = device->prepare();
        if (!error.isEmpty())
            throw Exception(error);
        for (unsigned i = 0; i < 100; ++i) {
            error = device->start();
            if (!error.isEmpty())
                throw Exception(error);
            milliSleep(100);
            device->stop();
        }
        device->release();
    } catch (const std::exception &e) {
        qFatal("%s", qPrintable(QString::fromLatin1("Processing failed: %1")
                                    .arg(QString::fromLocal8Bit(e.what()))));
    }

    return 0;
}
