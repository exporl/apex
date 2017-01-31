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

#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;

#include "scriptexpander.h"

#include "xml/apexxmltools.h"
#include "apextools.h"
#include "services/mainconfigfileparser.h"
#include "services/errorhandler.h"
#include "xmlpluginapi.h"

#include <QMap>
#include <QString>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>


using namespace apex::ApexXMLTools;

namespace apex
{
namespace parser
{


    // FIXME
    bool WriteElement(XERCES_CPP_NAMESPACE::DOMElement* e, QString filename)
    {
    // FIXME: error handling
        DOMImplementation* impl =
                DOMImplementationRegistry::getDOMImplementation(X("Core"));


        LocalFileFormatTarget formatTarget(filename.toAscii());
        DOMWriter* domWriter = impl->createDOMWriter();
        if ( domWriter->canSetFeature( X( "format-pretty-print" ), true ) )
            domWriter->setFeature( X( "format-pretty-print" ), true );
        domWriter->writeNode( &formatTarget, *e );

        return true;
    }


const QString ScriptExpander::INSTANCENAME("pLuGiNiNtErFaCe");

void ScriptExpander::ExpandScript(XERCES_CPP_NAMESPACE::DOMNode * base,
                                  const QString& function)
{

    // parse XML into localscript and parameters. Localscript is the script
    // itself, either from inline or read from file
    QVariantMap parameters(m_scriptParameters);
    QString localscript;
    bool fromgeneral=true;

    for ( DOMNode* it = base->getFirstChild() ; it != 0 ;
            it = it->getNextSibling() )
    {
        if ( it->getNodeType() != DOMNode::ELEMENT_NODE )
            continue;

        DOMElement* currentNode = (DOMElement*) it;

        const QString tag( XMLutils::GetTagName( it ) );

        if (tag=="parameter")
        {
            QString name = XMLutils::GetAttribute( currentNode, "name" );
            QVariant value = XMLutils::GetFirstChildText(currentNode);
            parameters[name]=value;
            fromgeneral=false;

        }
        else if (tag=="script")
        {
            QString source = XMLutils::GetAttribute( currentNode, "source" );
            QString value=XMLutils::GetFirstChildText(currentNode);
            // get script (inline or from file)

            if (source=="inline")
                localscript=value;
            else if (source=="file")
            {
                QString filename = ApexTools::findReadableFile (value,
                                   QStringList() <<                                    Paths::Get().GetNonBinaryPluginPath()
                                   << QDir::currentPath(),
                                   QStringList() << "" << ".js");
                qDebug("opening %s", qPrintable(filename));
                QFile file(filename);
                if (! file.open(QIODevice::ReadOnly) )
                {
                    throw ApexStringException("Cannot open plugin script: " + filename);
                }
                localscript=file.readAll();
                file.close();
                fromgeneral=false;
            }
            else
            {
                qFatal("Invalid attribute value");
            }
        }
        else
        {
            qFatal("Unknown element");
        }

    }

    if (fromgeneral && m_libraryFile.isEmpty())
        throw ApexStringException("No script library present. "
                "It should be defined in <general>");


    // Get the script api and attach it to the current script
    if (!m_libraryFile.isEmpty())
    {
        QString libraryfile = ApexTools::findReadableFile (m_libraryFile,
                              QStringList() << Paths::Get().GetNonBinaryPluginPath(),
                              QStringList() << ".js");

        QFile file(libraryfile);
        if (! file.open(QIODevice::ReadOnly) )
        {
            throw ApexStringException("Cannot open script library file: " +
                                      m_libraryFile);
        }

        localscript=file.readAll() + "\n" + localscript;
        file.close();
    }

    // attach the main script library to the current script
    QString mainPluginLibrary(
        MainConfigFileParser::Get().GetPluginScriptLibrary());
    if ( ! mainPluginLibrary.isEmpty() ) {
        QFile file(mainPluginLibrary);
        if (! file.open(QIODevice::ReadOnly) )
        {
            std::cout<<"MainpluginLibrary path value is: "<<mainPluginLibrary.toStdString()<<std::endl;
            throw ApexStringException("Cannot open main script library file: " +
                    mainPluginLibrary);
        }

        localscript=file.readAll() + "\n" + localscript;
        file.close();
    }


    qDebug("Executing XML plugin script");

    QScriptEngine m_scriptEngine;
    QScriptEngineDebugger m_scriptEngineDebugger;
    m_scriptEngineDebugger.attachTo(&m_scriptEngine);

    // create API
    XMLPluginAPI api;
    QScriptValue APIObject = m_scriptEngine.newQObject(&api);
    m_scriptEngine.globalObject().setProperty("api", APIObject);


    // set parameters in params object
    QScriptValue paramsObject = m_scriptEngine.newObject();
    m_scriptEngine.globalObject().setProperty("params", paramsObject);

    for (QMap<QString,QVariant>::const_iterator it=parameters.begin();
            it!=parameters.end(); ++it)
    {
        paramsObject.setProperty(it.key(),
                                 ApexTools::QVariant2ScriptValue(
                                         &m_scriptEngine,
                                         it.value()));
    }


//    qDebug("Script to be evaluated :\n%s\n=================",
//           qPrintable(localscript));

    if (!  m_scriptEngine.canEvaluate(localscript))
    {
        throw ApexStringException("Can not evaluate script");
    }


    // Evaluate script as is: load all functions etc.
    QScriptValue result = m_scriptEngine.evaluate(localscript);

    // check for errors from script execution
    QString XMLString;
    if (m_scriptEngine.hasUncaughtException())
    {
        int lineNo = m_scriptEngine.uncaughtExceptionLineNumber();
        throw ApexStringException(
            QString("Could not execute script at line %1:\n%2")
            .arg (lineNo-1).arg(result.toString()));
    }
/*    else
    {
        XMLString=result.toString();
        qDebug("Result: %s", qPrintable(XMLString));
    }*/



    // evaluate given function
    
    m_scriptEngine.globalObject().setProperty(INSTANCENAME, result );

    //result = m_scriptEngine.evaluate (INSTANCENAME+".GetScreens").call(result);

    qDebug("=========== Evaluating %s ==================", qPrintable(function));
    result = m_scriptEngine.evaluate(function + "()");
    qDebug("========== end evaluation ==============");


    if (m_scriptEngine.hasUncaughtException())
    {
        int lineNo = m_scriptEngine.uncaughtExceptionLineNumber();
        QFile outfile("_apexscript.js");
        if (outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outfile);
            out << localscript;
            outfile.close();
        }
        throw ApexStringException(
            QString("Could not execute script at line %1:\n%2")
            .arg (lineNo).arg(result.toString()));
    }
    XMLString=result.toString();

    DOMNode* newNode = StringToDOM(XMLString);


    // replace current node (<plugin...>) by newNode (<...>)
    DOMNode* parent=base->getParentNode();
    Q_ASSERT(parent);
    try
    {
        DOMDocument* doc = base->getOwnerDocument();
        newNode = doc->importNode(newNode, true);

        // take dummy element and add all children
        DOMNodeList* childList = newNode->getChildNodes();

        while (childList->getLength()) {            // when inserting elements somewhere else, they disappear from the childList (ahem)
            DOMNode* currentNode = childList->item(0);
            parent->insertBefore(currentNode, base);
        }


        parent->removeChild(base);


        // revalidate the document, get the relevant part of the tree and
        // replace again. This gives us normalization
        Revalidate(doc);
    }
    catch (DOMException& e )
    {
        qDebug("Xerces exception: %s", qPrintable(X2S(e.getMessage())));
    }


    // FIXME: delete original newnode

}





XERCES_CPP_NAMESPACE::DOMNode* ScriptExpander::StringToDOM(QString s)
{
    //qDebug("String to be converted: %s", qPrintable(s));
    DOMElement* root = ParseXMLDocument(s, false);
    return root;

    // debug: print new dom element
    //for ( DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
}


DOMElement* ScriptExpander::ParseXMLDocument(QString s, bool dovalidation)
{
    XercesDOMParser* parser = new XercesDOMParser();

    if (dovalidation)
    {
        parser->setIncludeIgnorableWhitespace(false);   // ignore whitespace that can be ignored according to the xml specs
        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setCreateCommentNodes(false);
        parser->setValidateAnnotations(true);
        parser->setDoNamespaces(true);
        parser->setDoSchema(true);
        parser->setValidationSchemaFullChecking(false);
    }
    else
    {
        parser->setIncludeIgnorableWhitespace(false);   // ignore whitespace that can be ignored according to the xml specs
        parser->setValidationScheme(XercesDOMParser::Val_Never);
        parser->setCreateCommentNodes(false);
        parser->setValidateAnnotations(false);
        parser->setDoNamespaces(false);
        parser->setDoSchema(false);
        parser->setValidationSchemaFullChecking(false);
    }

//    if (verbose) {
    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    errReporter->LogToApex( &apex::ErrorHandler::Get());
    parser->setErrorHandler(errReporter);
//    }


    // wrap string in dummy element (there should be only one root element)
    QString t(s);
    s="<dummy>"+s+"</dummy>";

    QByteArray b(s.toUtf8());

    MemBufInputSource source( reinterpret_cast<const XMLByte *>(b.constData()),
                              b.length(),
                              "membufid",
                              false);

    parser->parse( source );
    bool bFailed = parser->getErrorCount() != 0;

    if (bFailed)
        throw ApexStringException("Error parsing string returned by a plugin"
                " to a DOM tree");          // FIXME: verbosity

    DOMNode* node  = parser->getDocument()->getDocumentElement();

    Q_ASSERT( node->getNodeType() == DOMNode::ELEMENT_NODE);
    return (DOMElement*) node;
}



DOMDocument* ScriptExpander::Revalidate(DOMNode* doc)
{
    DOMImplementation* impl =
        DOMImplementationRegistry::getDOMImplementation(X("Core"));


    //LocalFileFormatTarget formatTarget(filename.toAscii());
    DOMWriter* domWriter = impl->createDOMWriter();
    if ( domWriter->canSetFeature( X( "format-pretty-print" ), true ) )
        domWriter->setFeature( X( "format-pretty-print" ), true );
    //domWriter->writeNode( &formatTarget, *e );

    /*XMLCh* written = domWriter->writeToString(*doc);
    QByteArray writtenString = QString::fromUtf16(written).toUtf8();
    delete written;*/
    MemBufFormatTarget outputMemBuf;
    domWriter->writeNode(&outputMemBuf, *doc);

    //ParseXMLDocument(written, true);

    XercesDOMParser* parser = new XercesDOMParser();


    parser->setIncludeIgnorableWhitespace(false);   // ignore whitespace that can be ignored according to the xml specs
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setCreateCommentNodes(false);
    parser->setValidateAnnotations(true);
    parser->setDoNamespaces(true);
    parser->setDoSchema(true);
    parser->setValidationSchemaFullChecking(true);

//    if (verbose) {
    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    errReporter->LogToApex( &apex::ErrorHandler::Get());
    parser->setErrorHandler(errReporter);
//    }

 /*    MemBufInputSource source(reinterpret_cast<const XMLByte*>
            (writtenString.data()), writtenString.size(),
                             "membufid",
                             false);*/

   MemBufInputSource source(outputMemBuf.getRawBuffer(), outputMemBuf.getLen(),
                             "membufid",
                             false);

    QString temp( Paths::Get().GetExperimentSchemaPath() );
    ApexTools::ReplaceWhiteSpaceWithNBSP( temp );
    parser->setExternalSchemaLocation(
            QString((EXPERIMENT_NAMESPACE) + QString(" ") + temp)
            .toAscii() );



    parser->parse( source );
    bool bFailed = parser->getErrorCount() != 0;

   if (bFailed) {
        if (m_parent) {
            int answer = QMessageBox::critical(m_parent,
                                            tr("Error expanding plugin"),
                                tr("There was an error expanding a plugin, "
                                        "would you like to save the resulting "
                                        "XML document?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::Yes);
            if (answer == QMessageBox::Yes) {
                QString filename ( QFileDialog::getSaveFileName(m_parent,
                                            tr("Save XML document"),
                                "output.xml"));

                LocalFileFormatTarget formatTarget(filename.toAscii());
                if ( domWriter->canSetFeature( X( "format-pretty-print" ), true ) )
                    domWriter->setFeature( X( "format-pretty-print" ), true );
                domWriter->writeNode( &formatTarget, *doc );

            }
        } else {
            qDebug("No parent defined, not interactive");
        }
        throw ApexStringException("XML code generated by a plugin did not validate. Check the plugin");
    }

    return parser->getDocument();
}

ScriptExpander::ScriptExpander(QString libraryFile, QVariantMap scriptParameters, QWidget *parent):
        m_libraryFile(libraryFile),
        m_scriptParameters(scriptParameters),
        m_parent(parent)
{

}

}
}

XERCES_CPP_NAMESPACE::DOMNode * apex::parser::ScriptExpander::findById(XERCES_CPP_NAMESPACE::DOMNode * e, QString id)
{
    QString attr( XMLutils::GetAttribute( e, "id"));
    if (attr==id)
        return e;

    for (DOMNode* currentNode=e->getFirstChild(); currentNode!=0;
         currentNode=currentNode->getNextSibling()) {
        DOMNode* result = findById(currentNode, id);
        if (result)
            return result;
    }
    return 0;
}
