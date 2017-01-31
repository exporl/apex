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

#include "apextools.h"
#include "version.h"

#include "status/statusreporter.h"

#include "apexxmltools.h"
#include "xmldocumentgetter.h"

#include <QFile>
#include <QHash>
#include <QXmlQuery>

#include <iostream>
#include <sstream>

using namespace xercesc;
#include "xmlkeys.h"
using namespace apex::XMLKeys;

#include <iostream>
using namespace std;

namespace apex
{
namespace ApexXMLTools
{

QString XMLutils::transformXSLTQt (const QString &document, const QString &script, const QHash<QString,QVariant> hash)
{
    QString bla("    ");

    QXmlQuery query(QXmlQuery::XSLT20);
    QHashIterator<QString,QVariant> i(hash);
    while (i.hasNext()) {
        i.next();
        query.bindVariable(i.key(), i.value());
    }
    query.setFocus(QUrl(document));
    query.setQuery(QUrl(script));
    query.evaluateTo(&bla);

    return bla;
}

const QString XMLutils::GetTagName(DOMElement *element)
{
    Q_ASSERT(element);
    return X2S(element->getTagName());
}

QString XMLutils::GetAttribute(const DOMElement *element,
        const QString &attribute)
{
    Q_ASSERT(element);
    Q_ASSERT(element->getNodeType() == DOMNode::ELEMENT_NODE);
    Q_ASSERT(!attribute.isEmpty());

    const XMLCh * const attributeName = S2X(attribute);

    if (!element->hasAttribute(attributeName)) {
        //qCDebug(APEX_RS, "XMLutils::GetAttribute: Attribute not found: %s", qPrintable (attribute));
        return QString();
    }

    return X2S(element->getAttribute(attributeName));
}


const QString XMLutils::GetFirstChildText(DOMElement *element)
{
    Q_ASSERT(element);
    if (const DOMNode * const child = element->getFirstChild())
        return X2S(child->getTextContent());
    return QString();
}

/**
 * Convert entire node to string
 * @param pElem
 * @return
 */
const QString XMLutils::NodeToString( DOMNode* pElem)
{
    /*DOMNode* elem = pElem.getFirstChild();

    DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(X("LS"));
    DOMWriter         *writer = ((DOMImplementationLS*)impl)->createDOMWriter();

    XMLCh* temp = writer->writeToString  (*elem );
    return X2S(temp);*/

    QString result;

    for (DOMNode* currentNode=pElem->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            result+= "<" + GetTagName(currentNode) + ">" +
                     GetFirstChildText(currentNode) +
                     "</" + GetTagName(currentNode) + ">" ;
        }
        else if (currentNode->getNodeType() == DOMNode::TEXT_NODE)
        {
            result+= X2S(currentNode->getTextContent());
        }
        else
        {
            Q_ASSERT(0&& "unknown node type");
        }
    }

    return result;

    // FIXME memory leak?
}

DOMElement *XMLutils::GetElementsByTagName(DOMElement *base , const QString& tagName)
{
    Q_ASSERT(base);
    Q_ASSERT(!tagName.isEmpty());
    return static_cast<DOMElement*>(base->getElementsByTagName(S2X(tagName))->item(0));
}

const QString   XMLutils::GetTagName( DOMNode*    pElem )
{
    return GetTagName( (DOMElement*) pElem );
}

const QString   XMLutils::GetAttribute( const DOMNode*    pElem , const QString& Attribute )
{
    return GetAttribute( (DOMElement*) pElem, Attribute );
}

const QString   XMLutils::GetFirstChildText( DOMNode*    pElem )
{
    return GetFirstChildText( (DOMElement*) pElem );
}

DOMElement*  XMLutils::GetElementsByTagName( DOMNode*    pBase , const QString& TagName   )
{
    return GetElementsByTagName( (DOMElement*) pBase, TagName );
}

const QString XMLutils::FindAttribute( DOMNode* pLevel_0, const QString& sLevel_1, const QString& sLevel_2, const QString& sID, const QString& sAttribute )
{
    Q_ASSERT(pLevel_0 && pLevel_0->getNodeType() == DOMNode::ELEMENT_NODE);
    for ( DOMNode* currentNode = pLevel_0 ; currentNode != 0 ; currentNode = currentNode->getNextSibling() )
    {
        Q_ASSERT(currentNode && currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
        QString temp=GetTagName(currentNode);
        if ( GetTagName( currentNode ) == sLevel_1 )
        {
            for ( DOMNode* curNode = currentNode->getFirstChild() ; curNode != 0 ; curNode = curNode->getNextSibling() )
            {
                Q_ASSERT(curNode && curNode->getNodeType() == DOMNode::ELEMENT_NODE);
                if ( GetTagName( curNode ) == sLevel_2 )
                {
                    if ( GetAttribute( curNode, gc_sID ) == sID )
                        return GetAttribute( curNode, sAttribute );
                }
            }
        }
    }
    return QString(); //not found
}

const QString XMLutils::FindChild( DOMNode* pLevel_0, const QString& sID )
{
    DOMNode* found = FindChildNode(pLevel_0, sID);
    if (found)
        return GetFirstChildText(found);
    else
        return QString();
}

XERCES_CPP_NAMESPACE::DOMNode* XMLutils::FindChildNode  (  XERCES_CPP_NAMESPACE::DOMNode* pLevel_0,
        const QString& tag )
{
    Q_ASSERT(pLevel_0 && pLevel_0->getNodeType() == DOMNode::ELEMENT_NODE);
    for ( DOMNode* currentNode = pLevel_0->getFirstChild() ; currentNode != 0 ; currentNode = currentNode->getNextSibling() )
    {
        Q_ASSERT(currentNode && currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
        if ( GetTagName( currentNode ) == tag )
        {
            return currentNode;
        }
    }
    return 0; //not found

}

XERCES_CPP_NAMESPACE::DOMElement* XMLutils::CreateTextElement(
    XERCES_CPP_NAMESPACE::DOMDocument* doc,
    QString tag,
    QString value                                            )
{
    DOMElement* result = doc->createElement(S2X(tag));
    DOMNode* text = doc->createTextNode(S2X(value));
    result->appendChild(text);
    return result;
}

XERCES_CPP_NAMESPACE::DOMElement* XMLutils::CreateTextElement(
    XERCES_CPP_NAMESPACE::DOMDocument* doc,
    QString tag,
    double value)
{
    return CreateTextElement(doc,tag, QString::number(value));
}

XERCES_CPP_NAMESPACE::DOMElement* XMLutils::CreateTextElement(
    XERCES_CPP_NAMESPACE::DOMDocument* doc,
    QString tag,
    QDateTime value)
{
    return CreateTextElement(doc,tag, value.toString(Qt::ISODate));
}

XERCES_CPP_NAMESPACE::DOMElement* XMLutils::ParseXMLDocument
    ( const QString &filename, bool verbose, QString schema )
{
    if (!QFile::exists(filename)) {
        throw ApexStringException(QString("XMLutils: file to parse (%1) does not exist")
                    .arg(filename));
    }
    qCDebug(APEX_RS, "Parsing document %s", qPrintable ( filename ) );
    XercesDOMParser* parser = new XercesDOMParser();//FIXME delete?

    // ignore whitespace that can be ignored according to the xml specs
    parser->setIncludeIgnorableWhitespace ( false );
    parser->setValidationScheme ( XercesDOMParser::Val_Always );
    parser->setCreateCommentNodes ( false );
    parser->setValidateAnnotations ( true );
    parser->setDoNamespaces ( true );
    parser->setDoSchema ( true );
    parser->setValidationSchemaFullChecking ( true );

    if ( !schema.isEmpty() ) {
        if (!QFile::exists(schema)) {
            throw ApexStringException(QString("XMLutils: schema file (%1) does not exist")
                    .arg(schema));
        }

        ApexTools::ReplaceWhiteSpaceWithNBSP(schema);
        parser->setExternalSchemaLocation( QFile::encodeName(QString(
                        EXPERIMENT_NAMESPACE) + " " + schema ) );
    }

    if (verbose) {
        DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();//FIXME delete?
        parser->setErrorHandler ( errReporter );
    }


    //ApexTools::ReplaceWhiteSpaceWithNBSP(filename);
    parser->parse ( QFile::encodeName(filename) );
    bool bFailed = parser->getErrorCount() != 0;

    if (bFailed) {
        qCDebug(APEX_RS, "Error parsing file %s", qPrintable ( filename ) );
        throw ApexStringException(QString("Error parsing file %1").arg(filename));
    }

    DOMNode* node  = parser->getDocument()->getDocumentElement();

    qCDebug(APEX_RS, "tag name of root element: %s", qPrintable ( XMLutils::GetTagName ( node ) ) );

    Q_ASSERT ( node->getNodeType() == DOMNode::ELEMENT_NODE );

    //    qCDebug(APEX_RS, "nodetype firstchild: %d", node->getFirstChild()->getNodeType());
    //    qCDebug(APEX_RS, "text content: %s", qPrintable(X2S(node->getFirstChild()->getTextContent())));
    return ( DOMElement* ) node;
}

XERCES_CPP_NAMESPACE::DOMElement* XMLutils::parseString(QString s)
{
    XercesDOMParser* parser = new XercesDOMParser();

    parser->setIncludeIgnorableWhitespace(false);   // ignore whitespace that can be ignored according to the xml specs
    parser->setValidationScheme(XercesDOMParser::Val_Never);
    parser->setCreateCommentNodes(false);
    parser->setValidateAnnotations(false);
    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setValidationSchemaFullChecking(false);

    QByteArray b(s.toUtf8());
    MemBufInputSource source( reinterpret_cast<const XMLByte *>(b.constData()),
                              b.length(),
                                       "membufid",
                                       false);

    parser->parse( source );
    bool bFailed = parser->getErrorCount() != 0;

    if (bFailed)
        qCDebug(APEX_RS, "parsing string failed");

    DOMNode* node  = parser->getDocument()->getDocumentElement();

    Q_ASSERT( node->getNodeType() == DOMNode::ELEMENT_NODE);

    return (DOMElement*) node;
}

bool XMLutils::WriteElement(XERCES_CPP_NAMESPACE::DOMNode* e, const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QFile::Truncate)) {
        return false;
    }
    file.write(elementToString(e).toUtf8());
    return true;
}

QString XMLutils::elementToString(XERCES_CPP_NAMESPACE::DOMNode* e)
{
    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));
    DOMLSSerializer* domSerializer = impl->createLSSerializer();
    domSerializer->getDomConfig()->setParameter(X("format-pretty-print"), true);
    QString result = X2S(domSerializer->writeToString(e));
    domSerializer->release();
    return result;
}

QString XMLutils::wrapTag(const QString& tag, const QVariant& value)
{
     return QString("<%1>%2</%3>").arg(tag).arg(value.toString()).arg(tag);
}

// These two functions are a hack, and there should be escaping everywhere TODO
QString XMLutils::xmlEscapedText(const QString &text)
{
    QString result = text;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    return result;
}

QString XMLutils::xmlEscapedAttribute (const QString &text)
{
    QString result = xmlEscapedText (text);
    result.replace ("\"", "&quot;");
    result.prepend ("\"");
    result.append ("\"");
    return result;
}

bool XMLutils::xmlBool(const QString &value)
{
    return (value == "true" || value == "1");
}

}//end ns apexxmltools
}//end ns apex


using namespace apex;
using namespace ApexXMLTools;

void DOMTreeErrorReporter::warning(const SAXParseException& /*toCatch*/)
{
    //ignore warnings
}

void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;
    const QString id(X2S(toCatch.getSystemId()));
    const QString line(QString::number(toCatch.getLineNumber()));
    const QString col(QString::number(toCatch.getColumnNumber()));
    const QString mess(X2S(toCatch.getMessage()));

    if ( !m_pHandler ) {
        cerr << "Error at file \""  << qPrintable (id)
        << "\", line "            << qPrintable (line)
        << ", column "            << qPrintable (col)
        << "\n   Message: "       << qPrintable (mess) << endl;
    } else {
        m_pHandler->addError( id, "line: " + line + " : " + mess );
    }
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    error(toCatch); //what's different between error and fatal error??
}

void DOMTreeErrorReporter::LogToApex( apex::StatusReporter* a_Handler )
{
    m_pHandler = a_Handler;
}

