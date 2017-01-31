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

#include "screen/screenparser.h"
#include "screen/screensdata.h"

#include "xml/apexxmltools.h"
#include "xml/xercesinclude.h"

#include "screeneditorexperimentfileparser.h"
#include "screenswriter.h"
#include "version.h"

#include <iostream>

using namespace xercesc;

namespace apex
{
  namespace editor
  {
    class SEErrorHandler : public ErrorHandler
    {
    public:
      void warning( const SAXParseException& exc );
      void error( const SAXParseException& exc );
      void fatalError( const SAXParseException& exc );
      void resetErrors();
      void output(const QString &category, const SAXParseException& exc );
    };
    void SEErrorHandler::resetErrors()
    {
    }
    void SEErrorHandler::warning( const SAXParseException& exc )
    {
      output( "warning", exc );
    }
    void SEErrorHandler::error( const SAXParseException& exc )
    {
      output( "error", exc );
    }
    void SEErrorHandler::fatalError( const SAXParseException& exc )
    {
      output( "fatal error", exc );
    }
    void SEErrorHandler::output(const QString &category, const SAXParseException& exc )
    {
      QString message;
      if ( exc.getMessage() )
        message = X2S(exc.getMessage());
      QString systemId;
      if ( exc.getSystemId() )
        systemId = X2S(exc.getSystemId());
      QString publicId;
      if ( exc.getPublicId() )
        publicId = X2S(exc.getPublicId());
      QString out =
        "ScreenEditorExperimentFileParser: " + category + ":"
        "\n\tsystem id: " + systemId +
        "\n\tpublic id: " + publicId +
        "\n\tline number: " + QString::number( exc.getLineNumber() ) +
        "\tcolumn number: " + QString::number( exc.getColumnNumber() ) +
        "\n\tmessage: " + message;
      std::cerr << out.toStdString() << std::endl;;
    }

    using ApexXMLTools::XMLutils;
    using gui::ScreenParser;

    using XERCES_CPP_NAMESPACE::DOMNode;
    using XERCES_CPP_NAMESPACE::StdOutFormatTarget;
    using XERCES_CPP_NAMESPACE::XMLPlatformUtils;

    ScreenEditorExperimentFileParser::ScreenEditorExperimentFileParser()
    {
      XMLPlatformUtils::Initialize();
      parser = new XercesDOMParser();
      parser->setExternalSchemaLocation(
                        QString(EXPERIMENT_NAMESPACE " " + schemaLoc) );
      parser->setValidationScheme(XercesDOMParser::Val_Always);
      parser->setDoNamespaces(true);
      parser->setDoSchema(true);
      parser->setValidationSchemaFullChecking(true);
//    parser->setIncludeIgnorableWhitespace(false);   // ignore whitespace that can be ignored according to the xml specs
//    parser->setCreateCommentNodes(false);
      parser->setValidateAnnotations(true);
      errorHandler = new SEErrorHandler();
      parser->setErrorHandler( errorHandler );
    }

    std::auto_ptr<ScreenEditorExperimentData> ScreenEditorExperimentFileParser::parse( const QString& file )
    {
      try
      {
        parser->parse( file.ascii() );
        bool failed = parser->getErrorCount() != 0;
        if ( failed ) return std::auto_ptr<ScreenEditorExperimentData>();
      }
      catch( ... )
      {
        return std::auto_ptr<ScreenEditorExperimentData>();
      }
      std::auto_ptr<ScreenEditorExperimentData> ret( new ScreenEditorExperimentData() );
      DOMDocument* doc = parser->getDocument();
      ret->expDocument = doc;
      for ( DOMNode* currentNode = doc->getDocumentElement()->getFirstChild();
            currentNode != 0; currentNode = currentNode->getNextSibling() )
      {
        assert( currentNode );
        if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE)
          continue;

        const QString tag = XMLutils::GetTagName( currentNode );
        if ( tag == "screens" )
        {
          assert( dynamic_cast<DOMElement*>( currentNode ) );
          DOMElement* screensEl = static_cast<DOMElement*>( currentNode );
          ret->screensElement = screensEl;
          ret->screens = new ScreensData;

          for ( DOMNode* cn = screensEl->getFirstChild();
                cn != 0; cn = cn->getNextSibling() )
          {
            if ( cn->getNodeType() != DOMNode::ELEMENT_NODE )
              continue;
            const QString ctag = XMLutils::GetTagName( cn );
            if ( ctag == "screen" ) {
              assert( dynamic_cast<DOMElement*>( cn ) );
              DOMElement* ce = static_cast<DOMElement*>( cn );
              ScreenParser screenParser( ret->screens );
              Screen* s = screenParser.createScreen( ce );
              ret->screenToElementMap[s] = ce;
            } else if ( ctag == "defaultFont" ) {
              const QString font = XMLutils::GetFirstChildText( cn );
              ret->screens->setDefaultFont( font );
            } else if ( ctag == "defaultFontSize" ) {
              unsigned fs = XMLutils::GetFirstChildText( cn ).toUInt();
              ret->screens->setDefaultFontSize( fs );
            } else continue;
          }
        } else continue;
      }
      return ret;
    }

    ScreenEditorExperimentFileParser::~ScreenEditorExperimentFileParser()
    {
      delete parser;
      XMLPlatformUtils::Terminate();
      delete errorHandler;
    }

    void ScreenEditorExperimentFileParser::print( const ScreenEditorExperimentData* d )
    {
      DOMImplementation* imp = DOMImplementation::getImplementation();

      StdOutFormatTarget formatTarget;
      DOMWriter* domWriter = imp->createDOMWriter();
      if ( domWriter->canSetFeature( X( "format-pretty-print" ), true ) )
        domWriter->setFeature( X( "format-pretty-print" ), true );
      domWriter->writeNode( &formatTarget, *d->expDocument );
    }

    void ScreenEditorExperimentFileParser::save( const ScreenEditorExperimentData* d, const QString& file )
    {
      DOMImplementation* imp = DOMImplementation::getImplementation();

      LocalFileFormatTarget formatTarget( file.ascii() );
      DOMWriter* domWriter = imp->createDOMWriter();
      if ( domWriter->canSetFeature( X( "format-pretty-print" ), true ) )
        domWriter->setFeature( X( "format-pretty-print" ), true );
      domWriter->writeNode( &formatTarget, *d->expDocument );
    }

    void ScreenEditorExperimentData::storeScreenChanges( const Screen* s )
    {
      screenToElementMapT::const_iterator i = screenToElementMap.find( s );
      assert( i != screenToElementMap.end() );
      DOMElement* se = i->second;
      assert( se->getParentNode() == screensElement );

      writer::ScreensWriter writer;
      DOMElement* ne = writer.addScreen( expDocument, *s );
      screensElement->replaceChild( ne, se );
      assert( ne->getParentNode() == screensElement );
      screenToElementMap[s] = ne;
    }

    void ScreenEditorExperimentData::addScreen( Screen* s )
    {
      screens->manageScreen( s );
      writer::ScreensWriter writer;
      DOMElement* se = writer.addScreen( expDocument, *s );
      screensElement->appendChild( se );
      screenToElementMap[s] = se;
    }

    ScreenEditorExperimentData::~ScreenEditorExperimentData()
    {
      delete screens;
    }

    QString ScreenEditorExperimentFileParser::schemaLoc;

  }
}
