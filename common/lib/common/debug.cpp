/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "debug.h"

#include <QMetaProperty>

#include <QtGlobal>

#include <cstdio>

#ifdef Q_OS_UNIX
#include <signal.h>
#include <sys/resource.h>
#endif

#ifdef Q_CC_MSVC
#include <CRTDBG.h>
#endif

#ifdef Q_OS_WIN32
#include <QLibrary>
#include <windows.h>
#endif

#ifdef Q_OS_ANDROID
#include <android/log.h>
#endif

namespace cmn
{

void debugger()
{
#ifdef Q_OS_UNIX
    void (*saved_handler)(int);
    saved_handler = signal(SIGINT, SIG_IGN);
    raise(SIGINT);
    signal(SIGINT, saved_handler);
#endif
#ifdef Q_CC_MSVC
    _CrtDbgBreak();
#endif
}

#ifdef Q_OS_WIN32

// from dbghelp.h
struct MiniDumpExceptionInformation {
    DWORD ThreadId;
    EXCEPTION_POINTERS *ExceptionPointers;
    BOOL ClientPointers;
};

enum MiniDumpType {
    MiniDumpNormal = 0x00000000,
    MiniDumpWithDataSegs = 0x00000001,
    MiniDumpWithFullMemory = 0x00000002,
    MiniDumpWithHandleData = 0x00000004,
    MiniDumpFilterMemory = 0x00000008,
    MiniDumpScanMemory = 0x00000010,
    MiniDumpWithUnloadedModules = 0x00000020,
    MiniDumpWithIndirectlyReferencedMemory = 0x00000040,
    MiniDumpFilterModulePaths = 0x00000080,
    MiniDumpWithProcessThreadData = 0x00000100,
    MiniDumpWithPrivateReadWriteMemory = 0x00000200,
    MiniDumpWithoutOptionalData = 0x00000400,
    MiniDumpWithFullMemoryInfo = 0x00000800,
    MiniDumpWithThreadInfo = 0x00001000,
    MiniDumpWithCodeSegs = 0x00002000
};

typedef BOOL(WINAPI *MiniDumpWriteDump)(
    HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MiniDumpType DumpType,
    MiniDumpExceptionInformation *ExceptionParam, void *UserStreamParam,
    void *CallbackParam);

MiniDumpWriteDump dump;
wchar_t dumpPath[_MAX_PATH + 1];
LPTOP_LEVEL_EXCEPTION_FILTER oldFilter;
CoreDumpCallback coreDumpCallback;

static LONG WINAPI exceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    HANDLE file = CreateFileW(dumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        MiniDumpExceptionInformation info;
        info.ThreadId = GetCurrentThreadId();
        info.ExceptionPointers = exceptionInfo;
        info.ClientPointers = 0;

        dump(GetCurrentProcess(), GetCurrentProcessId(), file,
             MiniDumpType(MiniDumpNormal | MiniDumpWithDataSegs |
                          MiniDumpWithPrivateReadWriteMemory),
             &info, NULL, NULL);

        CloseHandle(file);
    }

    if (coreDumpCallback)
        coreDumpCallback(QString::fromWCharArray(dumpPath));

    return oldFilter ? oldFilter(exceptionInfo) : EXCEPTION_CONTINUE_SEARCH;
}

#endif // Q_OS_WIN32

void enableCoreDumps(const QString &applicationFilePath,
                     CoreDumpCallback callback)
{
    Q_UNUSED(applicationFilePath);
    Q_UNUSED(callback);

#ifdef Q_OS_WIN32
#if _MSC_VER >= 1400
    // Additionally, report on memory leaks in Visual Studio.
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    // Try to avoid too much processing in the exception handler
    const QString path = QString::fromLatin1("%2.%1.dmp")
                             .arg(GetCurrentProcessId())
                             .arg(applicationFilePath);
    if (path.length() > _MAX_PATH)
        return;
    path.toWCharArray(dumpPath);
    dumpPath[path.length()] = 0;

    dump = (MiniDumpWriteDump)QLibrary(QL1S("dbghelp"))
               .resolve("MiniDumpWriteDump");
    if (!dump)
        return;

    oldFilter = SetUnhandledExceptionFilter(exceptionFilter);
    coreDumpCallback = callback;
#else
    rlimit limits;
    getrlimit(RLIMIT_CORE, &limits);
    limits.rlim_cur = limits.rlim_max;
    setrlimit(RLIMIT_CORE, &limits);
#endif
}

void dumpQObjectInfo(QObject *object)
{
    qCDebug(EXPORL_COMMON, "Address: %p", object);
    qCDebug(EXPORL_COMMON, "Name: %s", qPrintable(object->objectName()));
    const QMetaObject *meta = object->metaObject();
    qCDebug(EXPORL_COMMON, "Class name: %s", meta->className());

    qCDebug(EXPORL_COMMON, "Properties:");
    for (int i = 0; i < meta->propertyCount(); ++i) {
        const QMetaProperty property = meta->property(i);
        qCDebug(EXPORL_COMMON, "  Property: %s", property.name());
        // enumerator() TODO
        qCDebug(EXPORL_COMMON, "    Has notify signal: %i",
                property.hasNotifySignal());
        qCDebug(EXPORL_COMMON, "    Is constant: %i", property.isConstant());
        qCDebug(EXPORL_COMMON, "    Is designable: %i",
                property.isDesignable());
        qCDebug(EXPORL_COMMON, "    Is enum type: %i", property.isEnumType());
        qCDebug(EXPORL_COMMON, "    Is final: %i", property.isFinal());
        qCDebug(EXPORL_COMMON, "    Is flag type: %i", property.isFlagType());
        qCDebug(EXPORL_COMMON, "    Is readable: %i", property.isReadable());
        qCDebug(EXPORL_COMMON, "    Is resettable: %i",
                property.isResettable());
        qCDebug(EXPORL_COMMON, "    Is scriptable: %i",
                property.isScriptable());
        qCDebug(EXPORL_COMMON, "    Is stored: %i", property.isStored());
        qCDebug(EXPORL_COMMON, "    Is user: %i", property.isUser());
        qCDebug(EXPORL_COMMON, "    Is valid: %i", property.isValid());
        qCDebug(EXPORL_COMMON, "    Is writable: %i", property.isWritable());
        // notifySignal() TODO
        qCDebug(EXPORL_COMMON, "    Notify signal index: %i",
                property.notifySignalIndex());
        qCDebug(EXPORL_COMMON, "    Property index: %i",
                property.propertyIndex());
        qCDebug(EXPORL_COMMON, "    Type: %i", property.type());
        qCDebug(EXPORL_COMMON, "    Type name: %s", property.typeName());
        qCDebug(EXPORL_COMMON, "    User type: %i", property.userType());
    }

    qCDebug(EXPORL_COMMON, "Class infos:");
    for (int i = 0; i < meta->classInfoCount(); ++i) {
        const QMetaClassInfo classInfo = meta->classInfo(i);
        qCDebug(EXPORL_COMMON, "  Class info: %s", classInfo.name());
        qCDebug(EXPORL_COMMON, "    Name: %s", classInfo.name());
        qCDebug(EXPORL_COMMON, "    Value: %s", classInfo.value());
    }
    // constructor(int index) TODO
    // constructorCount()

    // enumerator(int index) TODO
    // enumeratorCount()
    // enumeratorOffset()

    // method(int index) TODO
    // methodCount()
    // methodOffset()

    // *superClass() TODO
}

} // namespace cmn
