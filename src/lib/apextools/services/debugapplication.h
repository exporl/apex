#ifndef DEBUG_APPLICATION_H
#define DEBUG_APPLICATION_H

#ifdef Q_OS_UNIX
#include <signal.h>
#endif

//#include "utils/stringexception.h"
#include <QApplication>
#include <stdio.h>


namespace utils {
struct StringException : std::exception
 {
   explicit StringException( const std::string& ac_sError ) :
     mc_sError( ac_sError )
   {}

   ~StringException() throw() {}

   std::string mc_sError;
 };
}

void debugger()
{
    #ifdef Q_OS_UNIX
    void(* saved_handler) (int);
    saved_handler = signal(SIGINT, SIG_IGN);
    raise(SIGINT);
    signal(SIGINT, saved_handler);
    #endif
    #ifdef Q_CC_MSVC
    _CrtDbgBreak();
    #endif
}

static void crashOnAssert(QtMsgType type, const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
    
    if ((type == QtFatalMsg) ||
        (type == QtWarningMsg && (!qgetenv("QT_FATAL_WARNINGS").isNull()))) {
        debugger();
    *(reinterpret_cast<int*>(0)) = 0;
    }
}

class DebugApplication : public QApplication
{
    public:
        DebugApplication(int &argc, char **argv) : QApplication(argc, argv)
        {
            qInstallMsgHandler(crashOnAssert);
        }

        virtual bool notify(QObject *receiver, QEvent *event);
};

bool DebugApplication::notify(QObject *receiver, QEvent *event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (const utils::StringException& e) {
        qFatal("Exception in QCoreApplication::notify(): %s", e.mc_sError.data());
    } catch (const std::exception &e) {
        qFatal("Exception in QCoreApplication::notify(): %s", e.what());
    } catch (...) {
        qFatal("Unknown exception in QApplication::notify()");
    }
    // Silence the compiler
    return false;
}

#endif
