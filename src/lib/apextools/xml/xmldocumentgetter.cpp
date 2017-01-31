/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

// The xerces includes must go first, otherwise DOMDocument is confused with the msxml one

#include "xmldocumentgetter.h"
#include "../apextools.h"
#include "apexxmltools.h"

#include <QFile>

using namespace xercesc;

using namespace apex;
using namespace apex::ApexXMLTools;

namespace {

void fInitParser( XercesDOMParser* parser, DOMTreeErrorReporter* errReporter ) {
    parser->setDoNamespaces(true);
    parser->setIncludeIgnorableWhitespace(false);   // ignore whitespace that can be ignored according to the xml specs
    parser->setCreateCommentNodes(false);
    parser->setValidateAnnotations(true);
    parser->setErrorHandler(errReporter);
}

}


XMLDocumentGetter::XMLDocumentGetter(  ) {
    try {
        XMLPlatformUtils::Initialize();
        theErrReporter.reset( new DOMTreeErrorReporter() );
    } catch( ... ) {
        throw ApexException();
    }
}

XMLDocumentGetter::~XMLDocumentGetter(  ) {
    theErrReporter.reset();
    theParser.reset();
    XMLPlatformUtils::Terminate();
}

void XMLDocumentGetter::LogToApex( StatusReporter* a_pError ) {
    theErrReporter->LogToApex( a_pError );
}

void XMLDocumentGetter::setSchemaAndNamespace( const QString& schemaFilename,
                            const QString& xmlNamespace   )
{
    theParser->setValidationScheme(XercesDOMParser::Val_Always);
    theParser->setDoSchema(true);
    theParser->setValidationSchemaFullChecking(true);

    QString temp(schemaFilename);
    ApexTools::ReplaceWhiteSpaceWithNBSP(temp);

    qCDebug(APEX_RS, "%s", qPrintable(QString(xmlNamespace + QString(" ") + temp)));

    theParser->setExternalSchemaLocation(QFile::encodeName(xmlNamespace + " "
                                                           + temp));
}

DOMDocument* XMLDocumentGetter::GetXMLDocument(const QString& filename,
                                               const QString& schemaFilename,
                                               const QString& xmlNamespace)
{
    theParser.reset( new XercesDOMParser() );
    fInitParser( theParser.data(), theErrReporter.data() );
    setSchemaAndNamespace(schemaFilename, xmlNamespace );
    theParser->parse(QFile::encodeName(filename));
    bool bFailed = theParser->getErrorCount() != 0;

    if (bFailed)
        throw ApexStringException("Parsing failed");

    return theParser->getDocument();
}

DOMDocument* XMLDocumentGetter::GetXMLDocument( const MemBufInputSource& data,
                                       const QString& schemaFilename,
                                       const QString& xmlNamespace      )
{
    theParser.reset( new XercesDOMParser() );
    fInitParser( theParser.data(), theErrReporter.data() );
    setSchemaAndNamespace(schemaFilename, xmlNamespace );
    theParser->parse( data );
    bool bFailed = theParser->getErrorCount() != 0;

    if (bFailed)
        throw ApexStringException("Parsing failed");

    return theParser->getDocument();
}

DOMDocument* XMLDocumentGetter::GetXMLDocument(const QString& filename)
{
    theParser.reset( new XercesDOMParser() );
    fInitParser( theParser.data(), theErrReporter.data() );
    theParser->setValidationScheme(XercesDOMParser::Val_Never);
    theParser->setDoSchema(false);
    theParser->setValidationSchemaFullChecking(false);

    theParser->parse(QFile::encodeName(filename));
    bool bFailed = theParser->getErrorCount() != 0;

    if (bFailed)
        throw ApexStringException("Parsing failed");

    return theParser->getDocument();
}
