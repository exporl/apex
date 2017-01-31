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

#include "experimenttoxmlwriter.h"

#include "screentoxmlwriter.h"

#include "data/screen.h"
//#include "gui/mainwindowconfig.h"
#include "screen/screensdata.h"
#include "parameters/generalparameters.h"
#include "result/resultparameters.h"
#include "runner/experimentdata.h"

#include "xml/xercesinclude.h"

#include <QString>

using namespace XERCES_CPP_NAMESPACE;

namespace apex
{

  DOMDocument* ExperimentToXMLWriter::experimentToXML(
    const Experiment* exp )
  {
    DOMElement* documentElement = document->createElement(X("apex"));
    documentElement->setAttribute(X("version"), X("1"));
    QString expDesc = exp->GetExperimentDescription();
    if ( !expDesc.isEmpty() )
    {
      DOMElement* descEl = document->createElement(X("description"));
      descEl->appendChild( document->createTextNode(expDesc.utf16()));
      documentElement->appendChild( descEl );
    }

    DOMElement* generalParametersElement =
      generalParametersToXML( exp->GetGeneralParameters() );
    documentElement->appendChild( generalParametersElement );

    const ResultParameters* respars = exp->GetResultParameters();
    if ( respars )
    {
        DOMElement* resultParametersElement =
            resultParametersToXML( respars );
        documentElement->appendChild( resultParametersElement );
    }

    DOMElement* screensElement =
      screensToXML( exp->getScreensData(), exp->GetMainWindowConfig() );
    documentElement->appendChild( screensElement );

    document->appendChild( documentElement );
    return document;
  }

  DOMElement* ExperimentToXMLWriter::appendBoolChild( DOMElement* parent, const QString& tagname, bool val )
  {
    DOMElement* boolElem = document->createElement(tagname.utf16());
    QString text = val ? QLatin1String("true") : QLatin1String("false");
    boolElem->appendChild( document->createTextNode(text.utf16()));
    parent->appendChild( boolElem );
    return boolElem;
  }

  DOMElement* ExperimentToXMLWriter::appendTextChild( DOMElement* parent, const QString& tagname, const QString& text )
  {
    DOMElement* textElem = document->createElement(tagname.utf16());
    textElem->appendChild( document->createTextNode(text.utf16()));
    parent->appendChild( textElem );
    return textElem;
  }

  DOMElement* ExperimentToXMLWriter::generalParametersToXML(
    const GeneralParameters* p )
  {
    DOMElement* generalElem =
      document->createElement(X("general"));

    bool exitAfter = p->GetExitAfter();
    appendBoolChild( generalElem, QLatin1String("exitAfter"), exitAfter );

    bool showResults = p->GetShowResults();
    appendBoolChild( generalElem, QLatin1String("showresults"), showResults );

    bool waitForStart = p->GetWaitForStart();
    appendBoolChild( generalElem, QLatin1String("waitforstart"), waitForStart );

    bool saveProcessedResults = p->GetSaveResults();
    appendBoolChild( generalElem, QLatin1String("saveprocessedresults"), saveProcessedResults );

    return generalElem;
  }

  ExperimentToXMLWriter::ExperimentToXMLWriter()
  {
    DOMImplementation* impl = DOMImplementation::getImplementation();
    document = impl->createDocument();
  }

  DOMElement* ExperimentToXMLWriter::resultParametersToXML( const ResultParameters* p )
  {
    DOMElement* ret = document->createElement(X("results"));
    appendTextChild(ret, QLatin1String("xsltscript"), p->GetXsltScript());
    return ret;
  }

  DOMElement* ExperimentToXMLWriter::screensToXML(
    const ScreensData& screens, const MainWindowConfig& mwc )
  {
    DOMElement* ret = document->createElement( X( "screens" ));

    {
      DOMElement* generalElem =
        document->createElement(X( "general" ));

      appendBoolChild( generalElem,QLatin1String( "stopbutton" ),
                       mwc.m_PanelConfig.m_bShowStopButton );
      appendBoolChild( generalElem,QLatin1String( "showpanel" ),
                       mwc.m_PanelConfig.m_bShowPanel );

      QString itiScreen = mwc.m_sItiScreen;
      unsigned itiLength = mwc.m_nItiLength;
      if ( !itiScreen.isEmpty() )
      {
        DOMElement* itiElem =
          document->createElement( X( "intertrialscreen" ));
        itiElem->setAttribute( X( "length" ),
                               QString::number(itiLength).utf16());
        itiElem->appendChild(
          document->createTextNode(QString::number(itiScreen).utf16()));
        generalElem->appendChild( itiElem );
      }
      ret->appendChild( generalElem );
    }

    {
      DOMElement* reinforcementElem =
        document->createElement( X( "reinforcement" ));

      appendBoolChild( reinforcementElem, QLatin1String( "progressbar" ),
                       mwc.m_PanelConfig.m_bShowProgress );
      DOMElement* feedbackElem =
        appendBoolChild( reinforcementElem, QLatin1String( "feedback" ),
                         mwc.m_bShowFeedback );
      feedbackElem->setAttribute(
        X( "length" ), QString::number( mwc.m_nFeedbackLength ).utf16() );
      appendBoolChild( reinforcementElem, QLatin1String( "showcurrent" ),
                       mwc.m_bShowCurrent );
      ret->appendChild( reinforcementElem );
    }

    if ( mwc.m_eMode == gc_eChild )
    {
      DOMElement* childmodeElem =
        document->createElement( X( "childmode" ));

      QString intro = mwc.m_sIntroScreen;
      unsigned introLength = mwc.m_nIntroLength;
      if ( !intro.isEmpty() )
      {
        DOMElement* introElem =
          appendTextChild( childmodeElem, QLatin1String( "intro" ),
                           intro);
        introElem->setAttribute( X( "length" ),
                                 QString::number(introLength).utf16() );
      }

      QString outro = mwc.m_sOutroScreen;
      unsigned outroLength = mwc.m_nOutroLength;
      if ( !outro.isEmpty() )
      {
        DOMElement* outroElem =
          appendTextChild( childmodeElem, QLatin1String( "outro" ),
                           outro );
        outroElem->setAttribute( X( "length" ),
                                 QString::number(outroLength ).utf16() );
      }

      QString panelMovie = mwc.m_PanelConfig.m_sPanelMovie;
      if ( !panelMovie.isEmpty() )
      {
        appendTextChild( childmodeElem, "panel",
                         panelMovie );
      }
      ret->appendChild( childmodeElem );
    }
    {
      // todo: uri_prefix is not stored atm
    }
    {
      QString defaultFont = screens.getDefaultFont();
      if ( !defaultFont.isEmpty() )
        appendTextChild( ret, "defaultFont", defaultFont );
    }
    {
      int defaultFontSize = screens.getDefaultFontSize();
      if ( defaultFontSize != -1 )
        appendTextChild( ret, QLatin1String("defaultFontSize"),
                         QString::number(defaultFontSize));
    }
    {
      // screens
      typedef ScreensData::ScreenMap ScreenMap;
      const ScreensData::ScreenMap& screens = screens.getScreens();
      for ( ScreenMap::const_iterator i = screens.begin();
            i != screens.end(); ++i )
      {
        const Screen* s = i->second;
        ScreenToXMLWriter w( document );

        DOMElement* screenElem = w.screenToXML( s );
        ret->appendChild( screenElem );
      }
    }
    return ret;
  }

}
