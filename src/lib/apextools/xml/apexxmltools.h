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

#ifndef APEXAPEXXMLTOOLS_H
#define APEXAPEXXMLTOOLS_H

#include "../global.h"

#include "xercesinclude.h"

#include <qstring.h>
#include <string>
#include "../exceptions.h"

#include <QDateTime>

//give a warning when using the xml classes in places where we should not
//use it. eg libdata
#ifdef APEXDATA_MAKEDLL
#ifdef Q_CC_MSVC
#pragma message("We shouldn't use any xml stuff in libdata")
#else
//#warning "We shouldn't use any xml stuff in libdata"
#endif
#endif

//![ stijn ] added method and member to DOMTreeErrorReporter
//!in order to allow output to ApexErrorHandler
//!this will disable std output. Setting to 0 re-enables std output
namespace apex
{
class ApexErrorHandler;
class StatusReporter;
}

//!now defined in apexxmltools.cpp so it can use ApexXMLTools
//!FIXME make XMLUtils and XMLDocumentGetter singletons
class DOMTreeErrorReporter;

namespace apex
{

/**
@author Tom Francart,,,
*/
namespace ApexXMLTools
{




/**
  * QString methods return empty string on error
  * xercesc::DOMElement* methods return 0 on error
  ************************************************************ */
class APEXTOOLS_EXPORT XMLutils
{
    public:
        static const QString GetTagName (
                        xercesc::DOMElement* pElem );
        static QString GetAttribute (
                        const xercesc::DOMElement* pElem ,
                        const QString& Attribute );
        static const QString GetFirstChildText (
                xercesc::DOMElement* pElem );
        static const QString NodeToString (
                        xercesc::DOMNode* pElem );
        static xercesc::DOMElement*  GetElementsByTagName (
                xercesc::DOMElement* pBase ,
                const QString& TagName   );

        static const QString GetTagName (
                xercesc::DOMNode* pElem );
        static const QString GetAttribute (
                const xercesc::DOMNode* pElem ,
                const QString& Attribute );
        static const QString  GetFirstChildText (
                xercesc::DOMNode*    pElem );
        static  xercesc::DOMElement*  GetElementsByTagName (
                xercesc::DOMNode*    pBase ,
                const QString& TagName   );

        //search attribute for element with id TODO
        //returns empty if not found
        static const QString FindAttribute  (
                xercesc::DOMNode* pLevel_0,
                const QString& sID,
                const QString& sAttribute );

        //specifying levels makes searching a lot faster
        //first searches level 1, then level 2, then attribute from element with id
        static const QString  FindAttribute (
                xercesc::DOMNode* pLevel_0,
                const QString& sLevel_1,
                const QString& sLevel_2,
                const QString& sID,
                const QString& sAttribute );


        /**
          * Find the child with tagname sID
        * @param pLevel_0 the element to search
        * @param sID the tag to get
        * @return the value or an empty string if not found
        */
        static const QString FindChild (
                xercesc::DOMNode* pLevel_0,
                const QString& sID );

        /**
         * Find the first child with the given tag name
         * returns 0 if not found
         */
        static xercesc::DOMNode* FindChildNode  (  xercesc::DOMNode* pLevel_0,
                const QString& tag );



        // creators
        /**
         * Create a domelement containing a text node
         * @param doc the document
         * @param tag tag name
         * @param value text value
         * @return
         */
        static xercesc::DOMElement* CreateTextElement(
            xercesc::DOMDocument* doc,
            QString tag,
            QString value);

        static xercesc::DOMElement* CreateTextElement(
            xercesc::DOMDocument* doc,
            QString tag,
            double value);

        static xercesc::DOMElement* CreateTextElement(
            xercesc::DOMDocument* doc,
            QString tag,
            QDateTime value);

		//parse an xml file to a DOMElement
		static xercesc::DOMElement* ParseXMLDocument(QString filename,
				bool verbose=true, QString schema = "");

        /**
         * Parses a QString into a DOMElement. Will not do any validation.
         */
        static xercesc::DOMElement* parseString(QString toParse);

        static bool WriteElement(xercesc::DOMElement* e, QString filename);
        static QString elementToString(xercesc::DOMElement* e);

        static QString xmlEscapedText (const QString &text);
        static QString xmlEscapedAttribute (const QString &text);
};
}//end ns ApexXMLTools
}//end ns apex

//!moved to this file
class APEXTOOLS_EXPORT DOMTreeErrorReporter : public xercesc::ErrorHandler
{
    public:
        DOMTreeErrorReporter() :
                fSawErrors( false ),
                m_pHandler( 0 )
        {}
        ~DOMTreeErrorReporter()
        {}

        //!implement ErrorHandler
        void warning(const xercesc::SAXParseException& toCatch);
        void error(const xercesc::SAXParseException& toCatch);
        void fatalError(const xercesc::SAXParseException& toCatch);
        void resetErrors()
        { fSawErrors = false; }
        bool getSawErrors() const
        { return fSawErrors; }

        //!
        void LogToApex( apex::StatusReporter* a_Handler );

    private:
        bool fSawErrors;
        apex::StatusReporter* m_pHandler;
};

// This needs to be a macro and no function otherwise the QString is already out of scope

#ifdef XMLCH_IS_WCHAR_T
#define X2S(str) QString::fromWCharArray(str)
#define S2X(str) (wchar_t*)str.utf16()
#define X(str) (wchar_t*)QString::fromLatin1(str).utf16()
#else
#define X2S(str) QString::fromUtf16(str)
#define S2X(str) str.utf16()
#define X(str) QString::fromLatin1(str).utf16()
#endif



#endif
