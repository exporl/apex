/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: TestDOMTreeErrorReporter.cpp 568078 2007-08-21 11:43:25Z amassari $
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "xml/xercesinclude.h"
#include "domtreeerrorreporter.h"
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdlib.h>
#include <string.h>


void TestDOMTreeErrorReporter::warning(const SAXParseException&)
{
    //
    // Ignore all warnings.
    //
}

void TestDOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "Error at file \"" << QString::fromUtf16(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << QString::fromUtf16(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void TestDOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "Fatal Error at file \"" << QString::fromUtf16(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << QString::fromUtf16(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void TestDOMTreeErrorReporter::resetErrors()
{
    fSawErrors = false;
}


