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

#include "screenparser.h"

#include <QUrl>
#include <QFile>
#include <QKeySequence>
#include <QFileInfo>
#include "xml/apexxmltools.h"
#include "xml/xmlkeys.h"
//#include <appcore/qt_utils.h>

#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;

#include "apexcontrol.h"
#include "screen/answerlabelelement.h"
#include "screen/arclayoutelement.h"
#include "screen/buttonelement.h"
#include "screen/matrixelement.h"
#include "screen/spinboxelement.h"
#include "screen/buttongroup.h"
#include "screen/flashplayerelement.h"
#include "screen/gridlayoutelement.h"
#include "screen/labelelement.h"
#include "screen/parameterlistelement.h"
#include "screen/parameterlabelelement.h"
#include "screen/pictureelement.h"
#include "screen/picturelabelelement.h"
#include "screen/screen.h"
#include "screen/screenelement.h"
#include "screen/screenlayoutelement.h"
#include "screen/texteditelement.h"
#include "screen/flashplayerelement.h"
#include "screen/sliderelement.h"
#include "exceptions.h"
#include "gui/guidefines.h"
//#include "gui/mainwindow.h"
#include "screen/screensdata.h"

#include "parameters/parametermanagerdata.h"
#include "parameters/parameter.h"

//from libtools
#include "apextools.h"
#include "gui/arclayout.h"

using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using apex::data::ParameterData;
using apex::data::AnswerLabelElement;
using apex::data::ArcLayoutElement;
using apex::data::ButtonElement;
using apex::data::ButtonGroup;
using apex::data::FlashPlayerElement;
using apex::data::GridLayoutElement;
using apex::data::LabelElement;
using apex::data::ParameterListElement;
using apex::data::ParameterLabelElement;
using apex::data::PictureElement;
using apex::data::PictureLabelElement;
using apex::data::Screen;
using apex::data::TextEditElement;
using apex::data::SliderElement;
using apex::data::ScreensData;

namespace
{


/**
 * Create a keysequence.
 * Throws ApexStringException in case of error.
 * @param a_pElement the DOMElement
 * @return the keysequence
 */
QKeySequence f_CreateKeySequence( XERCES_CPP_NAMESPACE::DOMElement* a_pElement )
{
    const QString sKey( XMLutils::GetFirstChildText( a_pElement ) );
    const QString sMod( XMLutils::GetAttribute( a_pElement, "modifier" ) );
    const QString sHex( XMLutils::GetAttribute( a_pElement, "hex" ) );
    bool bHex = false;
    if ( !sHex.isEmpty() )
        bHex = apex::ApexTools::bQStringToBoolean( sHex );

    //get modifier
    Qt::Modifier eMod;
    if ( !sMod.isEmpty() )
    {
        if ( sMod == "Alt" )
            eMod = Qt::ALT;
        else if ( sMod == "Ctrl" )
            eMod = Qt::CTRL;
        else if ( sMod == "Shift" )
            eMod = Qt::SHIFT;
        else
            throw( ApexStringException( "f_CreateKeySequence: unknown modifier " + sMod ) );
    }

    if ( bHex )
    {
        //create from hexadecimal
        bool bOk = false;
        const int nHex = sKey.toInt( &bOk, 16 );
        if ( !bOk )
            throw( ApexStringException( "f_CreateKeySequence: requested hex but " + sKey + " is not a valid number." ) );
        if ( !sMod.isEmpty() )
            return QKeySequence( eMod | nHex );
        return QKeySequence( nHex );
    }
    else
    {
        //create from string
        if ( sKey.length() == 1 )
        {
            if ( !sMod.isEmpty() )
                return QKeySequence( sMod + "+" + sKey );
            return QKeySequence( sKey );
        }
        //create from enum
        else
        {
            int nKey = 0;
            if ( sKey == "UpArrow" )
                nKey = Qt::Key_Up;
            else if ( sKey == "DownArrow" )
                nKey = Qt::Key_Down;
            else if ( sKey == "LeftArrow" )
                nKey = Qt::Key_Left;
            else if ( sKey == "RightArrow" )
                nKey = Qt::Key_Right;
            else if ( sKey == "Return" )
                nKey = Qt::Key_Return;
            else if ( sKey == "Space" )
                nKey = Qt::Key_Space;
            else if ( sKey == "Enter" )
                nKey = Qt::Key_Enter;
            else
                throw( ApexStringException( "f_CreateKeySequence: unknown key " + sKey ) );
            if ( !sMod.isEmpty() )
                return QKeySequence( eMod | nKey );
            return QKeySequence( nKey );
        }
    }
}

bool sf_bIsLayout( const QString& ac_sTag )
{
    return ( ac_sTag == gc_sGridLayout || ac_sTag == gc_sVertLayout || ac_sTag == gc_sHoriLayout || ac_sTag == gc_sArcLayout || ac_sTag == gc_sTwoPartLayout);
}
}

namespace apex
{
namespace gui
{

/**
  * Checks whether the file exists. (for flash movies)
  * Appends ac_sRelativePath to ac_sBasePath.
  * @param ac_sBasePath the base path, can be empty
  * @param ac_sRelativePath the file path
  * @return full path if exists, else empty string
  */
QString ScreenParser::f_CheckPath( const QString& ac_sBasePath, const QString& ac_sRelativePath )
{
    QString s = ac_sBasePath;
    if ( !s.isEmpty() && !s.endsWith( '/' ) )
    {
        s += '/';
    }
    else if ( s.isEmpty() )
    {
        //[ stijn ] a new feature in qt4: QUrl::path() strips the drive
        //from the path on windows. Completely unusable, so just return the path
        if ( !QFile::exists( ac_sRelativePath ) )
            return QString();
        return ac_sRelativePath;
    }
    const QUrl sPath( s + ac_sRelativePath );
    //must use path, qurl prefixes itself wih unusable "file:"
    QString p = sPath.path();

    QFileInfo fi(p);
    p=fi.absoluteFilePath();

    if ( !QFile::exists( p ) )
        p = "";
    return p;
}

QString ScreenParser::f_CheckPath(data::FilePrefix p, const QString& ac_sRelativePath )
{
    return f_CheckPath( FilePrefixConvertor::convert(p), ac_sRelativePath);
}

Screen* ScreenParser::createScreen( DOMElement* a_pElement )
{
    Q_ASSERT(a_pElement);
    ButtonGroup* buttonGroup = 0;
    ScreenElement* rootElement = 0;
    QString defaultAnswer;
    ScreenElementMap idToElementMap;

    const QString screenID = XMLutils::GetAttribute( a_pElement, gc_sID );

    for ( DOMNode* it = a_pElement->getFirstChild() ; it != 0 ; it = it->getNextSibling() )
    {
        // this is only necessary when we parse without
        // "setIncludeIgnorableWhitespace(false);" and/or
        // "setCreateCommentNodes(false);", but we sometimes do this like
        // in the screen editor...
        if ( it->getNodeType() != DOMNode::ELEMENT_NODE )
            continue;

        const QString tag( XMLutils::GetTagName( it ) );

        if ( sf_bIsLayout( tag ) )
        {
            try
            {
                rootElement = createLayout( (DOMElement*) it, 0, idToElementMap );
            }
            catch (ApexStringException &s)
            {
                log().addError( "Screen Parser", s.what() );
                delete rootElement;
                delete buttonGroup;
                return 0;
            }
            catch (...)
            {
                log().addError( "Screen Parser", "unknown exception" );
                delete rootElement;
                delete buttonGroup;
                return 0;
            }
        }
        else if ( tag == gc_sButtonGroup )
        {
            /*try
            {*/
            buttonGroup = createButtonGroup( (DOMElement*) it, idToElementMap );
            /*} catch(ApexStringException &s)      {
                error->AddError( "Screen Parser", s.what() );
                delete rootElement;
                if (buttonGroup) delete buttonGroup;
                return 0;
            }*/
        }
        else if ( tag == gc_sDefaultAnswer )
        {
            defaultAnswer = XMLutils::GetFirstChildText( it );

            if ( idToElementMap.find(defaultAnswer) == idToElementMap.end() &&
                    buttonGroup && buttonGroup->getID() != defaultAnswer )
            {
                throw ApexStringException("Unknown default_answer_element: " + defaultAnswer);
            }
        }
        else
            addUnknownTag( "ScreenParser::createScreen", tag );
    }

    if ( !rootElement )
    {
        delete rootElement;
        delete buttonGroup;
        return 0;
    }

    Screen* screen =
        new Screen( screenID, rootElement, idToElementMap, buttonGroup, defaultAnswer );
    screens->manageScreen( screen );
    return screen;
}

ScreenParser::ScreenParser( ScreensData* s, data::ParameterManagerData* pmd )
        : screens( s ),
          parameterManagerData(pmd)
{
}

Screen* ScreenParser::createTestScreen()
{
    static unsigned nCreated = 0;
    const QString layoutID = "testlayout" + QString::number( nCreated );
    const QString screenID = "testscreen"+ QString::number( nCreated++ );

    GridLayoutElement* gle = new GridLayoutElement( layoutID, 0, 5, 5, data::tStretchList(), data::tStretchList() );
    ScreenElementMap idToElementMap;

    ScreenElement* p1 = createTestScreenElement( gle );
    ScreenElement* p2 = createTestScreenElement( gle );

    p1->setX( 2 );
    p1->setY( 3 );
    p2->setX( 2 );
    p2->setY( 1 );

    idToElementMap[ p1->getID() ] = p1;
    idToElementMap[ p2->getID() ] = p2;

    Screen* screen = new Screen( screenID, gle, idToElementMap );
    screens->manageScreen( screen );
    return screen;
}

ScreenElement* ScreenParser::createTestScreenElement( ScreenElement* parent )
{
    static unsigned nCreated = 0;
    const QString id = "testelement"+ QString::number( nCreated++ );
    ButtonElement* e = new ButtonElement( id, parent );
    e->setText( QString::fromUtf8( "TestScreenElement" ) );
    return e;
}

ButtonGroup* ScreenParser::createButtonGroup( DOMElement* a_pElement, const ScreenElementMap& p_idToElementMap  )
{
    ButtonGroup* pRet = new ButtonGroup( XMLutils::GetAttribute( a_pElement, gc_sID ) );

    for ( DOMNode* currentNode = a_pElement->getFirstChild();
            currentNode != 0 ; currentNode = currentNode->getNextSibling() )
    {
        // this is only necessary when we parse without
        // "setIncludeIgnorableWhitespace(false);" and/or
        // "setCreateCommentNodes(false);", but we sometimes do this like
        // in the screen editor...
        if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE )
            continue;

        QString id = XMLutils::GetAttribute( currentNode, gc_sID );

        if ( p_idToElementMap.find(id) == p_idToElementMap.end() )
        {
            throw ApexStringException("Unknown screenelement " + id + " in buttongroup");
        }
        pRet->append( id );
    }

    return pRet;
}

void ScreenParser::addUnknownTag( const QString& ac_sSource, const QString& ac_sTag )
{
    log().addError( ac_sSource, "Unknown Tag: " + ac_sTag );
}

ScreenParser::mt_FeedBackPaths* ScreenParser::parseFeedBackPaths(
    DOMElement* a_pElement, const QString& ac_sElementID )
{
    mt_FeedBackPaths* pRet = new mt_FeedBackPaths();

    for ( DOMNode* currentNodee = a_pElement->getFirstChild() ;
            currentNodee != 0; currentNodee = currentNodee->getNextSibling() )
    {
        // this is only necessary when we parse without
        // "setIncludeIgnorableWhitespace(false);" and/or
        // "setCreateCommentNodes(false);", but we sometimes do this like
        // in the screen editor...
        if ( currentNodee->getNodeType() != DOMNode::ELEMENT_NODE )
            continue;
        const QString tag( XMLutils::GetTagName( currentNodee ) );
        const QString value( XMLutils::GetFirstChildText( currentNodee ));
        if ( tag == "highlight" )
        {
            QString p = f_CheckPath( FilePrefixConvertor::convert(m_sPath), value );
            if ( p.isEmpty() )
            {
                delete pRet;
                log().addError( "ScreenParser::parseFeedBackPaths", ac_sElementID + ": " + value + " doesn't exist" );
                return 0;
            }
            pRet->m_sHighLight = value;
        }
        else if ( tag == "positive" )
        {
            QString p = f_CheckPath( FilePrefixConvertor::convert(m_sPath), value );
            if ( p.isEmpty() )
            {
                delete pRet;
                log().addError( "ScreenParser::parseFeedBackPaths", ac_sElementID + ": " + value + " doesn't exist" );
                return 0;
            }
            pRet->m_sPositive = value;
        }
        else if ( tag == "negative" )
        {
            QString p = f_CheckPath( FilePrefixConvertor::convert(m_sPath), value );
            if ( p.isEmpty() )
            {
                delete pRet;
                log().addError( "ScreenParser::parseFeedBackPaths", ac_sElementID + ": " + value + " doesn't exist" );
                return 0;
            }
            pRet->m_sNegative = value;
        }
        else if ( tag == "disabled" )
        {
            QString p = f_CheckPath( FilePrefixConvertor::convert(m_sPath), value );
            if ( p.isEmpty() )
            {
                delete pRet;
                log().addError( "ScreenParser::parseFeedBackPaths", ac_sElementID + ": " + value + " doesn't exist" );
                return 0;
            }
            pRet->m_sDisabled = value;
        }
        else
            addUnknownTag( "ScreenParser::parseFeedBackPaths", tag );
    }

    return pRet;
}

ScreenLayoutElement* ScreenParser::createLayout(
    DOMElement* element, ScreenElement* parent, ScreenElementMap& elements )
{
    ScreenLayoutElement* ret = 0;

    const QString tag = XMLutils::GetTagName( element );
    DOMNode* it = element;

    //it should be a layout?
    assert( sf_bIsLayout( tag ) );

    //get common stuff
    QString width  = XMLutils::GetAttribute( it, gc_sWidth );
    QString height = XMLutils::GetAttribute( it, gc_sHeight );
    QString id     = XMLutils::GetAttribute( it, gc_sID );
    QString x      = XMLutils::GetAttribute( it, gc_sX );
    QString y      = XMLutils::GetAttribute( it, gc_sY );
    QString row      = XMLutils::GetAttribute( it, gc_sRow );
    QString col      = XMLutils::GetAttribute( it, gc_sCol );

    if (!x.isEmpty() && !col.isEmpty())
    {
        log().addError( "ScreenParser::createLayout", id + tr(": x and col cannot both be defined") );
        throw( 0 );
    }
    if (!y.isEmpty() && !row.isEmpty())
    {
        log().addError( "ScreenParser::createLayout", id + tr(": x and col cannot both be defined") );
        throw( 0 );
    }

    unsigned nX;
    if (!col.isEmpty())
        nX = col.toUInt();
    else
        nX = x.toUInt();

    unsigned nY;
    if (!row.isEmpty())
        nY = row.toUInt();
    else
        nY = y.toUInt();

    if ( x.isEmpty() && col.isEmpty())
        nX = 1;
    if ( y.isEmpty() && row.isEmpty())
        nY = 1;

    if ( tag == gc_sVertLayout || tag == gc_sHoriLayout || tag == gc_sGridLayout)
    {
        unsigned nW = width.toUInt();
        unsigned nH = height.toUInt();

        // columnstretch and rowstretch
        QString sRowstretch      = XMLutils::GetAttribute( it, "rowstretch" );
        QString sColstretch      = XMLutils::GetAttribute( it, "columnstretch" );

        data::tStretchList rowstretch;
        if (!sRowstretch.isEmpty())
        {
            rowstretch = parseStretchList(sRowstretch);
//            qDebug("SRowstretch=%s, rowstretch.size=%d", qPrintable(sRowstretch), rowstretch.size());
            if ( rowstretch.size() != (int) nH )
                throw ApexStringException(tr("The number of row stretch factors for layout %1 is not equal to its height").arg(id));
        }

        data::tStretchList colstretch;
        if (!sColstretch.isEmpty())
        {
            colstretch = parseStretchList(sColstretch);
            if ( colstretch.size() != (int) nW )
                throw ApexStringException(tr("The number of column stretch factors for layout %1 is not equal to its width").arg(id));
        }



        if ( tag == gc_sVertLayout )
            nW = 1;
        if ( tag == gc_sHoriLayout )
            nH = 1;
        if ( nH == 0 || nW == 0 || nW > sc_nMaxElementsInRow || nH > sc_nMaxElementsInCol )
        {
            log().addError( "ScreenParser::createLayout", id + ": Width or Height not in range" );
            throw( 0 );
        }

        ret = new GridLayoutElement( id, parent, nW, nH, rowstretch, colstretch );
    }
    else if ( tag == gc_sArcLayout )
    {
        const unsigned nW = width.toUInt();
        if ( nW == 0 )
        {
            log().addError( "ScreenParser::createLayout", id + ": Width must be > 0" );
            throw( 0 );
        }
        const QString sType( XMLutils::GetAttribute( it, "type" ) );

        ::ArcLayout::ArcType eType = ::ArcLayout::ARC_TOP;
        if ( sType == "lower" )
            eType = ::ArcLayout::ARC_BOTTOM;
        else if ( sType == "left" )
            eType = ::ArcLayout::ARC_LEFT;
        else if ( sType == "right" )
            eType = ::ArcLayout::ARC_RIGHT;
        else if ( sType == "full" )
            eType = ::ArcLayout::ARC_FULL;

        ArcLayoutElement* p = new ArcLayoutElement( id, parent, nW, eType );

        ret = p;
    }
    else if ( tag == gc_sTwoPartLayout )
    {

        throw ApexStringException(tr("TwoPartlayout is obsolete. Please use gridlayout with stretchfactors instead"));

    }
    else
    {
        log().addError( "ScreenParser::createLayout", id + ": unknown layout type" );
        throw( 0 );
    }

    elements[id] = ret;

    ret->setX( nX - 1 );
    ret->setY( nY - 1 );

    // go deeper, return value is unused since created elements
    // automatically become children of ret
    for ( DOMNode* itt = element->getFirstChild() ; itt != 0 ; itt = itt->getNextSibling() )
    {
        // this is only necessary when we parse without
        // "setIncludeIgnorableWhitespace(false);" and/or
        // "setCreateCommentNodes(false);", but we sometimes do this like
        // in the screen editor...
        if ( itt->getNodeType() != DOMNode::ELEMENT_NODE )
            continue;
        DOMElement* el = static_cast<DOMElement*>( itt );
        ScreenElement* childel = createElement( el, ret, elements );
        if ( childel )
        {
            //try {
            ret->checkChild( childel );
            /*} catch( ApexStringException& e )
            {
                error->AddError( "ScreenParser::createLayout()", e.what() );
                delete childel;
                delete ret;
                throw( 0 );
            } */
        }
        else
        {
            throw ApexStringException("Error parsing child element of layout");
        }
    }

    return ret;
}

ScreenElement* ScreenParser::createElement(
    DOMElement* element, ScreenElement* parent, ScreenElementMap& elements )
{
    const QString tag = XMLutils::GetTagName( element );

    if ( sf_bIsLayout( tag ) )
        return createLayout( element, parent, elements );
    else
        return createNonLayoutElement( element, parent, elements );
}

ScreenElement* ScreenParser::createNonLayoutElement(
    DOMElement* element, ScreenElement* parent, ScreenElementMap& elements )
{
    ScreenElement* ret = 0;
    const QString tag = XMLutils::GetTagName( element );
    const QString id  = XMLutils::GetAttribute( element, gc_sID );
    QString x      = XMLutils::GetAttribute( element, gc_sX );
    QString y      = XMLutils::GetAttribute( element, gc_sY );
    QString row      = XMLutils::GetAttribute( element, gc_sRow );
    QString col      = XMLutils::GetAttribute( element, gc_sCol );

    if (!x.isEmpty() && !col.isEmpty())
    {
        log().addError( "ScreenParser::createLayout", id + tr(": x and col cannot both be defined") );
        throw( 0 );
    }
    if (!y.isEmpty() && !row.isEmpty())
    {
        log().addError( "ScreenParser::createLayout", id + tr(": x and col cannot both be defined") );
        throw( 0 );
    }

    unsigned nX;
    if (!col.isEmpty())
        nX = col.toUInt();
    else
        nX = x.toUInt();

    unsigned nY;
    if (!row.isEmpty())
        nY = row.toUInt();
    else
        nY = y.toUInt();

    if ( x.isEmpty() && col.isEmpty())
        nX = 1;
    if ( y.isEmpty() && row.isEmpty())
        nY = 1;


    assert( !sf_bIsLayout( tag ) );
    assert( parent );

    if ( tag == gc_sButton )
    {
        ButtonElement* temp = new ButtonElement( id, parent );
        temp->setPrefix(m_sPath);

        DOMElement* te = XMLutils::GetElementsByTagName( element, gc_sText );
        if ( te )
            temp->setText( XMLutils::GetFirstChildText( te ) );

        ret = temp;
    }
    else if ( tag == gc_sSpinBox )
    {
        data::SpinBoxElement* temp = new data::SpinBoxElement( id, parent );
        temp->setPrefix(m_sPath);

        qDebug("Creating spinbox %s", qPrintable (id));

        DOMElement* te = XMLutils::GetElementsByTagName( element, gc_sValue );
        if ( te )
            temp->SetValue( XMLutils::GetFirstChildText( te ).toFloat() );

        te = XMLutils::GetElementsByTagName( element, gc_sMin );
        if ( te )
            temp->SetMin( XMLutils::GetFirstChildText( te ).toFloat() );

        te = XMLutils::GetElementsByTagName( element, gc_sMax );
        if ( te )
            temp->SetMax( XMLutils::GetFirstChildText( te ).toFloat() );

        te = XMLutils::GetElementsByTagName( element, gc_sStep );
        if ( te )
            temp->SetStep( XMLutils::GetFirstChildText( te ).toFloat() );

        te = XMLutils::GetElementsByTagName( element, gc_sParameter );
        if ( te )
            temp->SetParameter( XMLutils::GetFirstChildText( te ) );

        te = XMLutils::GetElementsByTagName( element, gc_sReset );
        if ( te )
            temp->SetReset( XMLutils::GetFirstChildText( te ) == "true" );

        ret = temp;
    }
    else if ( tag == gc_sMatrix )
    {
        data::MatrixElement* temp = new data::MatrixElement(id, parent);
        temp->setPrefix(m_sPath);

        for ( DOMNode* currentNode = element->getFirstChild();
              currentNode != 0; currentNode = currentNode->getNextSibling() )
        {
            // this is only necessary when we parse without
            // "setIncludeIgnorableWhitespace(false);" and/or
            // "setCreateCommentNodes(false);", but we sometimes do this like
            // in the screen editor...
            if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE )
                continue;

            QString tag( XMLutils::GetTagName( currentNode ) );
            if ( tag == "autocontinue" )
            {
                QString value( XMLutils::GetFirstChildText(currentNode));
                temp->setAutoContinue( (value=="true")?true:false);
            } else if ( tag=="element") {
                int row=XMLutils::GetAttribute(currentNode, "row").toInt();
                int col=XMLutils::GetAttribute(currentNode, "col").toInt();
                QString name;
                QString text;

                for ( DOMNode* c = currentNode->getFirstChild();
                      c != 0; c = c->getNextSibling() )
                {
                    QString tag( XMLutils::GetTagName( c ) );
                    QString value( XMLutils::GetFirstChildText(c));
                    if (tag=="name")
                        name=value;
                    else if (tag=="text")
                        text=value;
                    else
                        Q_ASSERT("Invalid tag");
                }
                if (text.isEmpty())
                    text=name;

                data::MatrixElement::MatrixButton newButton(name, text);
                temp->setButton(row,col,newButton);
            }
        }

        ret=temp;
    }
    else if ( tag == gc_sTextEdit )
    {
        TextEditElement* temp = new TextEditElement( id, parent );
        temp->setPrefix(m_sPath);

        DOMElement* te = XMLutils::GetElementsByTagName( element, gc_sText );
        if ( te )
            temp->setText( XMLutils::GetFirstChildText( te ) );

        te = XMLutils::GetElementsByTagName( element, gc_sInputmask );
        if ( te )
        {
            const QString sMask( XMLutils::GetFirstChildText( te ) );
            temp->setInputMask( sMask );
        }

        ret = temp;
    }
    else if ( tag == gc_sPicture )
    {
        PictureElement* temp = new PictureElement( id, parent );
        temp->setPrefix(m_sPath);

        for ( DOMNode* currentNode = element->getFirstChild();
                currentNode != 0; currentNode = currentNode->getNextSibling() )
        {
            // this is only necessary when we parse without
            // "setIncludeIgnorableWhitespace(false);" and/or
            // "setCreateCommentNodes(false);", but we sometimes do this like
            // in the screen editor...
            if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE )
                continue;
            QString tag( XMLutils::GetTagName( currentNode ) );
            if ( tag == gc_sPath )
            {
                QString filename=XMLutils::GetFirstChildText( currentNode );
                QString p = f_CheckPath( FilePrefixConvertor::convert(m_sPath),  filename);
                if ( p.isEmpty() )
                {
                    delete temp;
                    log().addError( "ScreenParser::createNonLayoutElement", id + ": " + FilePrefixConvertor::convert(m_sPath) + "/" + filename + " doesn't exist" );
                    throw ApexStringException("Error parsing screen");
                }
                // check whether it is a valid picture
                QImage testimage( p );
                if (testimage.isNull()) {
                    log().addError( "ScreenParser::createNonLayoutElement", id + ": " + p + " invalid picture.\nCheck whether the file is not corrupt and you have the necessary image plugins." );
                    return 0;
                }
                temp->setDefault(filename);
                temp->setUriId(
                            XMLutils::GetAttribute(currentNode, "id"));
                if (! temp->getUriId().isEmpty() && parameterManagerData) {
                    parameterManagerData->registerParameter(
                                temp->getUriId(),
                                data::Parameter("Picture", "string", temp->getDefault(),
                                                0, true, temp->getUriId()));
                }
            }
            else if ( tag == gc_sFeedback )
            {
                mt_FeedBackPaths* p = parseFeedBackPaths( (DOMElement*) currentNode, id );
                if ( p )
                {
                    temp->setHighlight( p->m_sHighLight );
                    temp->setPositive( p->m_sPositive );
                    temp->setNegative( p->m_sNegative );
                    temp->setDisabled( p->m_sDisabled );
                    delete p;
                }
                else
                {
                    delete temp;
                    return 0;
                }
                temp->setOverrideFeedback( true );
            }
            else if ( tag == gc_sShortcut )
            {
                try
                {
                    QKeySequence seq = f_CreateKeySequence( (DOMElement*) currentNode );
                    temp->setShortCut( seq );
                }
                catch ( ApexStringException& e )
                {
                    log().addError( "ScreenParser::createNonLayoutElement", e.what() );
                }
            }
            else
                addUnknownTag( "ScreenParser::createNonLayoutElement", tag );
        }

        ret = temp;
    }
    else if ( tag == gc_sLabel )
    {
        LabelElement* temp = new LabelElement( id, parent );
        temp->setPrefix(m_sPath);

        DOMElement* te = XMLutils::GetElementsByTagName( element, gc_sText );
        if ( te )
            temp->setText( XMLutils::GetFirstChildText( te ) );

        ret = temp;
    }
    else if ( tag == gc_sPictureLabel )
    {
        PictureLabelElement* temp = new PictureLabelElement( id, parent );
        temp->setPrefix(m_sPath);

        DOMElement* te = XMLutils::GetElementsByTagName( element, gc_sPath );
        if ( te )
        {
            QString filename = XMLutils::GetFirstChildText( te );
            QString p = f_CheckPath( FilePrefixConvertor::convert(m_sPath), filename );
            if ( p.isEmpty() )
            {
                delete temp;
                log().addError( "ScreenParser::createNonLayoutElement", id + ": " + p + " doesn't exist" );
                return 0;
            }

            // check whether it is a valid picture
            QImage testimage( p );
            if (testimage.isNull()) {
                log().addError( "ScreenParser::createNonLayoutElement", id + ": " + p + " invalid picture.\nCheck whether the file is not corrupt and you have the necessary image plugins." );
                return 0;
            }
            temp->setPicture( filename );
        }
        else
            assert( 0 );

        te = XMLutils::GetElementsByTagName( element, "uriDisabled" );
        if ( te )
        {
            QString filename = XMLutils::GetFirstChildText( te );
            QString p = f_CheckPath( FilePrefixConvertor::convert(m_sPath), filename );
            if ( p.isEmpty() )
            {
                delete temp;
                log().addError( "ScreenParser::createNonLayoutElement", id + ": " + p + " doesn't exist" );
                return 0;
            }

            // check whether it is a valid picture
            QImage testimage( p );
            if (testimage.isNull()) {
                log().addError( "ScreenParser::createNonLayoutElement", id + ": " + p + " invalid picture.\nCheck whether the file is not corrupt and you have the necessary image plugins." );
                return 0;
            }
            temp->setPictureDisabled( filename );
        }

        ret = temp;
    }
    else if ( tag == gc_sFlash )
    {
#ifndef FLASH
        throw ApexStringException("this apex version does not support flash");
#endif

        FlashPlayerElement* temp = new FlashPlayerElement( id, parent );
        temp->setPrefix(m_sPath);
        for ( DOMNode* currentNode = element->getFirstChild() ;
                currentNode != 0; currentNode = currentNode->getNextSibling() )
        {
            // this is only necessary when we parse without
            // "setIncludeIgnorableWhitespace(false);" and/or
            // "setCreateCommentNodes(false);", but we sometimes do this like
            // in the screen editor...
            if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE )
                continue;
            QString tag( XMLutils::GetTagName( currentNode ) );
            if ( tag == gc_sPath )
            {
                QString filename = XMLutils::GetFirstChildText( currentNode );
                QString p = f_CheckPath( m_sPath, filename );
                if ( p.isEmpty() )
                {
                    delete temp;
                    throw (ApexStringException( id + ": cannot find movie file " + FilePrefixConvertor::convert(m_sPath) + filename ));
//          return 0;
                }
                else
                {
                    qDebug("File %s found", qPrintable(p));
                }
                temp->setDefault(filename);
            }
            else if ( tag == gc_sFeedback )
            {
                mt_FeedBackPaths* p = parseFeedBackPaths( (DOMElement*) currentNode, id );
                if ( p )
                {
                    temp->setHighlight( p->m_sHighLight );
                    temp->setPositive( p->m_sPositive );
                    temp->setNegative( p->m_sNegative );
                    delete p;
                }
                else
                {
                    delete temp;
                    return 0;
                }
                temp->setOverrideFeedback( true );
            }
            else if ( tag == gc_sShortcut )
            {
                try
                {
                    QKeySequence seq = f_CreateKeySequence( (DOMElement*) currentNode );
                    temp->setShortCut( seq );
                }
                catch ( ApexStringException& e )
                {
                    log().addError( "ScreenParser::createNonLayoutElement", e.what() );
                }
            }
            else
                addUnknownTag( "ScreenParser::createNonLayoutElement", tag );
        }

        ret = temp;
    }
    else if ( tag == gc_sAnswerLabel )
    {
        AnswerLabelElement* temp = new AnswerLabelElement( id, parent );
        temp->setPrefix(m_sPath);
        ret = temp;
    }
    else if ( tag == gc_sParameterList )
    {
        ParameterListElement* temp = new ParameterListElement( id, parent );
        temp->setPrefix(m_sPath);

        for (DOMNode* currentNode=element->getFirstChild();
                currentNode!=0; currentNode=currentNode->getNextSibling())
        {
            // this is only necessary when we parse without
            // "setIncludeIgnorableWhitespace(false);" and/or
            // "setCreateCommentNodes(false);", but we sometimes do this like
            // in the screen editor...
            if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE )
                continue;
            QString tag = XMLutils::GetTagName(currentNode);
            QString value = XMLutils::GetFirstChildText(currentNode);
            QString name= XMLutils::GetAttribute(currentNode,"name");
            QString expression= XMLutils::GetAttribute(currentNode,"expression");

            if (tag=="parameter")
            {
                bool result = temp->addParameter(value,name,expression);

                if (!result)
                {
                    log().addError( "ScreenParser::createNonLayoutElement" + id, "invalid expression:" + expression );
                }
            }
        }

//     m_pControl.connect(&m_pControl, SIGNAL(NewStimulus(stimulus::Stimulus* )), temp, SLOT(NewStimulus(stimulus::Stimulus*)));

//     temp->mp_SetGeometry( ScreenElement::mt_Geometry( 0.9, 0.9 ) );

        ret = temp;
    }
    else if (tag == "parameterlabel")
    {
        ParameterLabelElement* temp = new ParameterLabelElement(id,parent);
        temp->setPrefix(m_sPath);

        DOMElement* te = XMLutils::GetElementsByTagName( element, "parameter" );
        if ( !te )
            qFatal("Parameter element not found");

        QString value = XMLutils::GetFirstChildText(te);
        QString name= XMLutils::GetAttribute(te,"name");
        QString expression= XMLutils::GetAttribute(te,"expression");

        temp->setParameter (ParameterData(value,name,expression));

        ret = temp;


    }
    else if (tag == "slider")
    {
        SliderElement* temp = new SliderElement(id, parent);
        temp->setPrefix(m_sPath);

        for (DOMNode* currentNode=element->getFirstChild();
                currentNode!=0; currentNode=currentNode->getNextSibling())
        {
            if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE )
                continue;
            QString tag = XMLutils::GetTagName(currentNode);
            QString value = XMLutils::GetFirstChildText(currentNode);

            if (tag=="orientation")
            {
                if (value=="horizontal")
                    temp->SetOrient(Qt::Horizontal);
                else if (value=="vertical")
                    temp->SetOrient(Qt::Vertical);
                else
                    qFatal("Invalid orientation");
            }
            else if (tag=="min")
            {
                temp->SetMin(value.toFloat());
            }
            else if (tag=="max")
            {
                temp->SetMax(value.toFloat());
            }
            else if (tag=="value")
            {
                temp->SetValue(value.toFloat());
            }
            else if (tag=="tickinterval")
            {
                temp->SetTickInt(value.toInt());
            }
            else if (tag=="stepsize")
            {
                temp->SetStepSize(value.toInt());
            }
            else if (tag=="pagesize")
            {
                temp->SetPageSize(value.toInt());
            } else {
                qFatal("Invalid tag");
            }


        }
        ret=temp;
    }
    else
        return 0;

    ret->setX( nX - 1 );
    ret->setY( nY - 1 );

    // generic attributes
    for (DOMNode* currentNode=element->getFirstChild();
         currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE )
            continue;
        QString tag = XMLutils::GetTagName(currentNode);
        QString value = XMLutils::GetFirstChildText(currentNode);

        if (tag==gc_sFontsize) {
            ret->setFontSize( value.toInt() );
        } else if (tag=="font") {
            ret->setFont( value );
        } else if (tag==gc_sFGColor) {
            if (!checkColor(value, ret->getID())) {
                delete ret;
                return 0;
            }
            ret->setFGColor( value );
        } else if (tag==gc_sBGColor) {
            if (!checkColor(value, ret->getID())) {
                delete ret;
                return 0;
            }
            ret->setBGColor( value );
        } else if (tag=="style") {
            ret->setStyle( value );
        } else if (tag==gc_sDisabled) {
            ret->setDisabled(apex::ApexTools::bQStringToBoolean( value ));
        } else if (tag==gc_sShortcut) {
            try
            {
                QKeySequence seq = f_CreateKeySequence( (DOMElement*) currentNode );
                QString action( XMLutils::GetAttribute( currentNode, "action" ) );
                if (action.isEmpty())
                    action=QLatin1String("click");
                ret->setShortCut( seq, action );
            }
            catch ( ApexStringException& e )
            {
                log().addError( "ScreenParser::createNonLayoutElement", e.what() );
            }
            
        }
    }

    elements[ret->getID()] = ret;
    return ret;
}

bool ScreenParser::checkColor (const QString& value, const QString& element) const
{
    QColor tc(value);
    if (! tc.isValid())
    {
        log().addError( "ScreenParser::createNonLayoutElement", "Invalid foreground color string \"" + value + "\" for element "+ element);
        return false;
    }
    return true;
}

}
}

apex::data::tStretchList apex::gui::ScreenParser::parseStretchList(QString d)
{
    QStringList temp( d.split(","));
    data::tStretchList result;
    for (QStringList::const_iterator it=temp.begin(); it!=temp.end(); ++it)
    {
        result.append( it->toInt());
    }
    return result;
}
