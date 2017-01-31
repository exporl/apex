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
 
#include "writertester.h"
#include "tools.h"
#include "xml/apexxmltools.h"

#include <QFile>
#include <QDir>
using namespace apex::ApexXMLTools;

WriterTester::WriterTester()
{
}

bool WriterTester::Test ( QStringList dirsToCheck )
{
	bool result=true;
	
	for ( int i = 0; i < dirsToCheck.size(); i++ )
	{
		QString dirToCheck = RootDir + dirsToCheck.at( i );
		
		QDir dir( dirToCheck );
		QStringList filters;
		filters << "*.xml";
		dir.setNameFilters( filters );
		QStringList files = dir.entryList();
	
		// for each of the files to be tested
		for ( int i=0; i<files.size(); ++i )
		{
			// add prefix to filename
			QString testfile ( dirToCheck + "/" + files.at ( i ) );
			
			if ( !QFile::exists( testfile ) )
				qFatal( "Fatal: File %s does not exist.", qPrintable( testfile ) );
			
			// parse XML file into DOM tree
			DOMElement* root = ParseXMLDocument ( testfile, true );
			
			Q_ASSERT ( root != NULL );
			
			ParseData ( root );
	
			// initialize XML engine
			DOMImplementation* impl =
				DOMImplementationRegistry::getDOMImplementation ( X ( "Core" ) );
	
			// create new DOM tree
			DOMDocument* doc = impl->createDocument ( 0, X ( "apex" ), 0 );
	
	
			DOMElement* theElement = WriteData ( doc );
			PrintElement( theElement );
	
			// Write the tree to a temporary file
			QString tempfile = QString( "__out__%1" ).arg( files.at( i ) );
			qDebug( "outfile: %s", qPrintable( tempfile ) );
			WriteElement ( theElement, "outfiles/" + tempfile );
	
			// check whether the original document en the generated document match
			QString compfile( "outfiles/" + tempfile );
			qDebug( "compfile: %s", qPrintable( compfile ) );
			result = CompareDocuments ( testfile,compfile ) && result;
		}
	}

	return result;
}
