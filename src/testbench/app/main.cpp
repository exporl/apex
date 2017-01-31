/******************************************************************************
* Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
*                                                                            *
* This file is part of APEX 3.                                               *
*                                                                            *
* APEX 3 is free software: you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by       *
* the Free Software Foundation, either version 2 of the License, or          *
* (at your option) any later version.                                        *
*                                                                            *
* APEX 3 is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License          *
* along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
*****************************************************************************/

#include "testbench.h"

#include <QApplication> //FIXME no gui should be needed here

#include "xml/xercesinclude.h"

#ifdef Q_OS_WIN32
#define FILTER "test_*.dll"
#else
#define FILTER "libtest_*.so"
#endif

// void noMessageOutput(QtMsgType, const char*)
// {
// }

template<class T>
QTextStream& operator<<(QTextStream& stream, QList<T> list)
{
    if (list.isEmpty())
        return stream;

    stream << "(" << list.first();

    for (int i = 1; i < list.size(); i++)
        stream << ", " << list[i];

    return stream << ")";
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    xercesc::XMLPlatformUtils::Initialize();

    //create console output, we don't use console since we want to be able
    //to let the user disable console ouput
    //TODO find a way to disabel qDebug() output during tests
    QTextStream console(stdout);

    QMap<QString, ApexTest*> testsMap;

    QDir pluginsDir = qApp->applicationDirPath();
    pluginsDir.setNameFilters(QStringList() << FILTER);

    Q_FOREACH (QString fileName, pluginsDir.entryList(QDir::Files))
    {
//        qDebug()<< "Tests: checking for plugin in" << fileName << endl;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));

        if (pluginLoader.load())
        {
            ApexTest* test = qobject_cast<ApexTest*>(pluginLoader.instance());

            if (test != 0)
            {
                QString name = test->name();
                //console << "Tests: loaded plugin" << name << endl;

                if (testsMap.contains(name))
                {
                    console << "Tests: multiple plugins with name"
                            << name << ", overwriting existing" << endl;
                }

                testsMap[name] = test;
            }
            else
            {
                console << "Tests: plugin in" << fileName
                        << "is not of type ApexTest" << endl;
            }
        }
        else
            console << pluginLoader.errorString() << endl;
    }

    //parse arguments
    QStringList args(argv[0]);
    QList<ApexTest*> tests;

    if (argc > 1)
    {
        if (QString("-list") == argv[1])
        {
            console << "Available tests:" << endl;
            Q_FOREACH (QString test, testsMap.keys())
                console << "    " << test << endl;

            return 0;
        }
        else
        {
            int i;
            for (i = 1; i < argc && testsMap.keys().contains(argv[i]); i++)
            {
                //console << "Tests: selecting test" << argv[i] << endl;
                tests << testsMap[argv[i]];
            }

            for (; i < argc; i++)
                args << argv[i];
        }
    }

    if (tests.isEmpty()) //no tests specified: run all
        tests = testsMap.values();

    int retVal = 0; //QTest::qExec returns 0 on success
    QStringList passedTests, failedTests;

    Q_FOREACH (ApexTest* test, tests)
    {
        console << "Tests: executing test " << test->name() << " with ";

        if (args.size() > 1)
            console << "args " << args.mid(1);
        else
            console << "no args";

        console << endl;

        //and RUN! :-)
        int ret = QTest::qExec(test, args);

        if (ret != 0)
        {
            retVal = ret;
            failedTests << test->name();
        }
        else
            passedTests << test->name();
    }

    console << "*********** Tests: summary ***********" << endl;
    console << tests.size() << " tests executed" << endl;
    console << passedTests.size() << " tests passed " << passedTests << endl;
    console << failedTests.size() << " tests failed " << failedTests << endl;
    console << "********* Tests: end summary *********" << endl;

    xercesc::XMLPlatformUtils::Terminate();

    return retVal;
}
