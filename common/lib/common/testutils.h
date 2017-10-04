/******************************************************************************
 * Copyright (C) 2007,2012  Michael Hofmann <mh21@piware.de>                  *
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

#ifndef _EXPORL_COMMON_LIB_COMMON_TESTUTILS_H_
#define _EXPORL_COMMON_LIB_COMMON_TESTUTILS_H_

#include "temporarydirectory.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTest>

#ifdef Q_OS_UNIX
#define START_TIMER                                                            \
    timeval oldtime;                                                           \
    gettimeofday(&oldtime, NULL);
#define END_TIMER                                                              \
    timeval newtime;                                                           \
    gettimeofday(&newtime, NULL);                                              \
    qCDebug(EXPORL_COMMON, "Elapsed: %f",                                      \
            (newtime.tv_sec * 1e6 + newtime.tv_usec - oldtime.tv_sec * 1e6 -   \
             oldtime.tv_usec) /                                                \
                1000.0);
#else
#define START_TIMER                                                            \
    LARGE_INTEGER frequency, oldtime;                                          \
    QueryPerformanceFrequency(&frequency);                                     \
    QueryPerformanceCounter(&oldtime);
#define END_TIMER                                                              \
    LARGE_INTEGER newtime;                                                     \
    QueryPerformanceCounter(&newtime);                                         \
    qCDebug(EXPORL_COMMON, "Elapsed: %f",                                      \
            (newtime.QuadPart - oldtime.QuadPart) /                            \
                double(frequency.QuadPart));
#endif

#define ONLY_FULL()                                                            \
    do {                                                                       \
        if (!getenv("FULL_TESTS"))                                             \
            QSKIP("Set the environment variable FULL_TESTS to enable.",        \
                  SkipAll);                                                    \
    } while (0)

#define WAIT(what)                                                             \
    do {                                                                       \
        for (int i = 0; i < 20 && !(what); ++i)                                \
            QTest::qWait(50);                                                  \
        QVERIFY(what);                                                         \
    } while (0)

#define FUZZCOMP(value, expected, delta)                                       \
    fuzzComp(value, expected, delta, #value, #expected, __FILE__, __LINE__)

template <typename A, typename B, typename C>
void fuzzComp(A value, B expected, C delta, const char *valueText,
              const char *expectedText, const char *file, int line)
{
    if (!(qAbs(value - expected) < delta))
        QTest::qFail(qPrintable(QString::fromLatin1(
                                    "Compared values differ more than %1.\n"
                                    "  Actual (%4): '%2'\n"
                                    "  Expected (%5): '%3'")
                                    .arg(delta)
                                    .arg(value)
                                    .arg(expected)
                                    .arg(valueText, expectedText)),
                     file, line);
}

#define CHARCOMP(a, b, n)                                                      \
    do {                                                                       \
        for (unsigned compLoop = 0; compLoop < (n); ++compLoop)                \
            if (uchar((a)[compLoop]) != uchar((b)[compLoop]))                  \
                QFAIL(qPrintable(                                              \
                    QString::fromLatin1("Array values differ "                 \
                                        "at index %1: was %2, expected %3")    \
                        .arg(compLoop)                                         \
                        .arg(uchar((a)[compLoop]))                             \
                        .arg(uchar((b)[compLoop]))));                          \
    } while (0)

#define ARRAYCOMP(a, b, n)                                                     \
    do {                                                                       \
        for (unsigned compLoop = 0; compLoop < n; ++compLoop)                  \
            if (qAbs((a)[compLoop] - (b)[compLoop]) > 0.0001)                  \
                QFAIL(qPrintable(QString::fromLatin1(                          \
                                     "Array values not the "                   \
                                     "same at index %1: was %2, expected %3")  \
                                     .arg(compLoop)                            \
                                     .arg((a)[compLoop])                       \
                                     .arg((b)[compLoop])));                    \
    } while (0)

#define DATACOMP(a, as, ac, b, bs, bc, n)                                      \
    do {                                                                       \
        for (unsigned compLoop = 0; compLoop < n; ++compLoop)                  \
            if (qAbs((a)[compLoop * as + ac] - (b)[compLoop * bs + bc]) >      \
                0.0001)                                                        \
                QFAIL(qPrintable(QString::fromLatin1(                          \
                                     "Array values not the "                   \
                                     "same at index %1: was %2, expected %3")  \
                                     .arg(compLoop)                            \
                                     .arg((a)[compLoop * as + ac])             \
                                     .arg((b)[compLoop * bs + bc])));          \
    } while (0)

#define ARRAYFUZZCOMP(value, expected, delta, size)                            \
    arrayFuzzComp(value, expected, delta, size, __FILE__, __LINE__)

template <typename A, typename B, typename C>
void arrayFuzzComp(A value, B expected, C delta, unsigned size,
                   const char *file, int line)
{
    for (unsigned i = 0; i < size; ++i)
        if (!(qAbs(value[i] - expected[i]) < delta))
            QTest::qFail(
                qPrintable(QString::fromLatin1(
                               "Compared values differ more than %1 at %2.\n"
                               "  Actual: '%3'\n"
                               "  Expected: '%4'")
                               .arg(delta)
                               .arg(i)
                               .arg(value[i])
                               .arg(expected[i])),
                file, line);
}

#define TEST_EXCEPTIONS_TRY try {
#define TEST_EXCEPTIONS_CATCH                                                  \
    }                                                                          \
    catch (const std::exception &e)                                            \
    {                                                                          \
        QFAIL(qPrintable(QString::fromLatin1("No exception expected: %1")      \
                             .arg(QString::fromLocal8Bit(e.what()))));         \
    }

namespace cmn
{

class TemporarySettings
{
public:
    TemporarySettings()
    {
        QCoreApplication::setApplicationName(
            QString::fromLatin1("%1-%2")
                .arg(QFileInfo(QCoreApplication::applicationFilePath())
                         .fileName())
                .arg(QCoreApplication::applicationPid()));
        QCoreApplication::setOrganizationName(QL1S("ExpORL"));
        QCoreApplication::setOrganizationDomain(QL1S("exporl.med.kuleuven.be"));
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                           tempDir.dir().path());

        {
            QSettings settings;
            settings.clear();
        }

        {
            QSettings settings(QL1S("Trolltech"));
            Q_FOREACH (const auto &group, settings.childGroups())
                if (group.startsWith(QL1S("Qt Plugin Cache ")))
                    settings.remove(group);
        }
    }

    ~TemporarySettings()
    {
        QDir root = tempDir.dir();
        Q_FOREACH (const auto &dirName,
                   root.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QDir dir = QDir(root.filePath(dirName));
            Q_FOREACH (const auto &fileName, dir.entryList(QDir::Files))
                dir.remove(fileName);
            root.rmdir(dirName);
        }
    }

private:
    TemporaryDirectory tempDir;
};

} // namespace cmn

#endif
