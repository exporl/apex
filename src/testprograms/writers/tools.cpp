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

#include "tools.h"

#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;

#include "xml/xercesinclude.h"

using namespace xercesc;

#include "domtreeerrorreporter.h"

#include <QString>
#include <QProcess>

QString outfileName(const QString& infile)
{
    return "__out__" + infile;
}

DOMElement* ParseXMLDocument ( QString filename, bool verbose )
{
	qDebug ( "Parsing document %s", qPrintable ( filename ) );
	XercesDOMParser* parser = new XercesDOMParser();

	parser->setIncludeIgnorableWhitespace ( false );   // ignore whitespace that can be ignored according to the xml specs
	parser->setValidationScheme ( XercesDOMParser::Val_Always );
	parser->setCreateCommentNodes ( false );
	parser->setValidateAnnotations ( true );
	parser->setDoNamespaces ( true );
	parser->setDoSchema ( true );
	parser->setValidationSchemaFullChecking ( true );
// 	parser->setExitOnFirstFatalError( false );
// 	parser->setValidationConstraintFatal( false );
// 	parser->setIdentityConstraintChecking( false );

	if ( verbose )
	{
		DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
		parser->setErrorHandler ( errReporter );
	}


	parser->parse ( filename.toAscii() );
	bool bFailed = parser->getErrorCount() != 0;

	if ( bFailed )
		qDebug ( "Error parsing file %s", qPrintable ( filename ) );

	DOMNode* node  = parser->getDocument()->getDocumentElement();

	qDebug ( "tag name of root element: %s", qPrintable ( XMLutils::GetTagName ( node ) ) );

	Q_ASSERT ( node->getNodeType() == DOMNode::ELEMENT_NODE );

//    qDebug("nodetype firstchild: %d", node->getFirstChild()->getNodeType());
//    qDebug("text content: %s", qPrintable(QString::fromUtf16(node->getFirstChild()->getTextContent())));
	return ( DOMElement* ) node;
}


/*QString ShowTree(XERCES_CPP_NAMESPACE::DOMElement* root) {

}*/

bool PrintElement ( xercesc::DOMElement* e )
{
	Q_ASSERT( e != NULL );

	DOMImplementation* impl =
	    DOMImplementationRegistry::getDOMImplementation ( X ( "Core" ) );


	StdOutFormatTarget formatTarget;
	DOMWriter* domWriter = impl->createDOMWriter();
	if ( domWriter->canSetFeature ( X ( "format-pretty-print" ), true ) )
		domWriter->setFeature ( X ( "format-pretty-print" ), true );
	domWriter->writeNode ( &formatTarget, *e );

	return true;
}

bool PrintDocument ( XERCES_CPP_NAMESPACE::DOMDocument* doc )
{
	return PrintElement ( doc->getDocumentElement() );
}



bool WriteElement ( XERCES_CPP_NAMESPACE::DOMElement* e, QString filename )
{
	// FIXME: error handling
	DOMImplementation* impl =
	    DOMImplementationRegistry::getDOMImplementation ( X ( "Core" ) );


	LocalFileFormatTarget formatTarget ( filename.toAscii() );
	DOMWriter* domWriter = impl->createDOMWriter();
	if ( domWriter->canSetFeature ( X ( "format-pretty-print" ), true ) )
		domWriter->setFeature ( X ( "format-pretty-print" ), true );
	domWriter->writeNode ( &formatTarget, *e );

	return true;
}

bool WriteDocument ( XERCES_CPP_NAMESPACE::DOMDocument* doc, QString filename )
{
	WriteElement ( doc->getDocumentElement(), filename );

        return true;
}

bool CompareDocuments ( QString file1, QString file2 )
{
	QString comparecmd ( "/usr/bin/xmldiff" );

	QStringList args;
	args <<file1 << file2;

	qDebug ( "Differences between file %s and %s", qPrintable ( file1 ),
	         qPrintable ( file2 ) );
	qDebug ( "-------------------------------------------------------" );

	int result = QProcess::execute ( comparecmd, args );

	qDebug ( "-------------------------------------------------------" );

	if ( result )
		qDebug ( "Differences found for file %s", qPrintable ( file1 ) );
	else
		qDebug ( "No differences" );

	return result==0;
}





