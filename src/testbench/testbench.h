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

#ifndef _APEX_SRC_TESTBENCH_TESTBENCH_H_
#define _APEX_SRC_TESTBENCH_TESTBENCH_H_

#include <QtTest>
#include <QtPlugin>

#include <memory>

/**
 * @class ApexTest
 * @author Job Noorman
 *
 * @brief The base class for all tests in Apex testbench.
 *
 * All tests are plugins with this class as interface. When writing your own
 * test, you don't need to inherit QObject since this class inherits it.
 */
class ApexTest : public QObject
{
    public:

        virtual ~ApexTest() {}

        virtual QString name() const = 0;
};

Q_DECLARE_INTERFACE(ApexTest, "be.exporl.apex.test/1.0");

/**
 * @def TEST_COPY
 *
 * Performs a simple test for the copy constructor and assignment operator
 * of class @param C.
 *
 * @param C The class to test.
 *
 * @sa TEST_COPY_INIT
 */
#define TEST_COPY(C) TEST_COPY_INIT(C, (void))

/**
 * @def TEST_COPY_INIT
 *
 * Does the same as @ref TEST_COPY but lets you specify an init function to
 * initialise the data before it is copied.
 *
 * @param C    The class to test.
 * @param init The init function to use.
 *
 * @sa TEST_COPY
 */
#define TEST_COPY_INIT(C, init)             \
    do                                      \
    {                                       \
        std::auto_ptr<C> orig(new C());     \
        init(orig.get());                   \
        C copy_ctor(*orig);                 \
        C copy_assign;                      \
        copy_assign = *orig;                \
        QVERIFY(*orig == copy_ctor);        \
        QVERIFY(*orig == copy_assign);      \
        delete orig.release();              \
        QVERIFY(copy_assign == copy_ctor);  \
    }                                       \
    while (false)

/**
 * @def EXPECT_EXCEPTION
 *
 * Evaluates the given expression and calls QFAIL if no exception was thrown.
 */
#define EXPECT_EXCEPTION(expression)                                    \
    do                                                                  \
    {                                                                   \
        bool gotIt = false;                                             \
        try {expression;}                                               \
        catch(...) {gotIt = true;}                                      \
        if (!gotIt)                                                     \
            QFAIL("Expected an exception while executing "#expression); \
    }                                                                   \
    while (false)

#endif




















