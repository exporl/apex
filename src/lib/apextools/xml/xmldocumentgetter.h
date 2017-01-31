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

#ifndef XMLDOCUMENTGETTERH
#define XMLDOCUMENTGETTERH

#include "xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;

#include <QScopedPointer>
#include <QString>
#include <string>
#include "../exceptions.h"

  //![ stijn ] added method and member to DOMTreeErrorReporter
  //!in order to allow output to ApexErrorHandler
  //!this will disable std output. Setting to 0 re-enables std output
namespace apex{
  class ApexErrorHandler;
  class StatusReporter;
}

  //!now defined in apexxmltools.cpp so it can use ApexXMLTools
  //!FIXME put XMLDocumentGetter in own file
  //!make XMLUtils and XMLDocumentGetter singletons
class DOMTreeErrorReporter;

namespace apex {

/**
@author Tom Francart,,,
*/
namespace ApexXMLTools
{

    /**
    * Parses XML documents
    ***********************/
  class APEXTOOLS_EXPORT XMLDocumentGetter
  {
    public:
      XMLDocumentGetter(  );
     ~XMLDocumentGetter(  );

     //! No schema validation
     xercesc::DOMDocument* GetXMLDocument( const QString& filename );

     //! With schema validation
     xercesc::DOMDocument* GetXMLDocument( const QString& filename,
                                            const QString& schemaFilename,
                                            const QString& p_namespace      );

     xercesc::DOMDocument* GetXMLDocument( const MemBufInputSource& data,
                                            const QString& schemaFilename,
                                            const QString& p_namespace      );



      void LogToApex( StatusReporter* a_pError );

    private:
      void setSchemaAndNamespace( const QString& schemaFilename,
                                  const QString& p_namespace   );

        QScopedPointer<XERCES_CPP_NAMESPACE::XercesDOMParser> theParser;
        QScopedPointer<DOMTreeErrorReporter>  theErrReporter;
  };


}
}


#endif
