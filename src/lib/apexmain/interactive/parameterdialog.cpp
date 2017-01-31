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

#include "interactive/parameterdialogresults.h"

#include "services/errorhandler.h"

#include "xml/apexxmltools.h"
#include "xml/xalaninclude.h"
#include "xml/xercesinclude.h"

#include "parameterdialog.h"
#include "ui_parameterdialog.h"

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>

XALAN_USING_XALAN(NodeRefList)
XALAN_USING_XALAN(XPathEvaluator)
XALAN_USING_XALAN(XalanDOMString)
XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanDocumentPrefixResolver)
XALAN_USING_XALAN(XalanNode)
XALAN_USING_XALAN(XercesDOMSupport)
XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
XALAN_USING_XALAN(XercesDocumentWrapper)
XALAN_USING_XALAN(XercesParserLiaison)
XALAN_USING_XALAN(XalanElement)
XALAN_USING_XALAN(XalanXPathException)
XALAN_USING_XERCES(DOMNode)

using namespace xercesc;
using namespace apex::ApexXMLTools;

namespace apex
{

// ParameterDialog =============================================================

ParameterDialog::ParameterDialog(DOMDocument *document, QWidget *parent) :
    QDialog(parent),
    doApply(false),
    ui(new Ui::ParameterDialog),
    document(document)
{
    processXml();
    buildWidgets();
}

ParameterDialog::~ParameterDialog()
{
}

void ParameterDialog::processXml()
{
    const DOMElement * const base =
        XMLutils::GetElementsByTagName(document->getDocumentElement(),
                QLatin1String("interactive"));

    if (!base)
        return;

    

//    qDebug("<interactive> found");

    for (DOMNode *currentNode = base->getFirstChild(); currentNode != NULL;
            currentNode = currentNode->getNextSibling()) {
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName(currentNode);
        if (tag != QLatin1String("entry"))
            qFatal("invalid tag: %s", qPrintable(tag));

        const QString xpath = XMLutils::GetAttribute(currentNode,
                QLatin1String("expression"));
        const QString type = XMLutils::GetAttribute(currentNode,
                QLatin1String("type"));
        const QString description = XMLutils::GetAttribute(currentNode,
                QLatin1String("description"));
        const QString defaultValue = XMLutils::GetAttribute(currentNode,
                QLatin1String("default"));

        if (xpath.isEmpty() && description.isEmpty())
            ErrorHandler::Get().addError(tr("Interactive"),
                    tr("Interactive entries should contain an expression or "
                        "a description"));

        ValueType valueType;
        if (type == QLatin1String("int"))
            valueType = IntValue;
        else if (type == QLatin1String("double"))
            valueType = DoubleValue;
        else if (type == QLatin1String("bool"))
            valueType = BoolValue;
        else if (type == QLatin1String("string"))
            valueType = StringValue;
        else if (type == QLatin1String("combo"))
            valueType = ComboValue;
        else
            qFatal("invalid type: %s", qPrintable(type));

        mEntries.append(InteractiveEntry(xpath, valueType, description,
                    defaultValue));
    }
}

void ParameterDialog::buildWidgets()
{
    ui->setupUi(this);

    QGridLayout *gridLayout = new QGridLayout(ui->parameters);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    Q_FOREACH (const InteractiveEntry &entry, mEntries) {
        const unsigned line = gridLayout->rowCount();
        const QString description = entry.description;
        const QString defaultValue = entry.defaultvalue;
        const int type = entry.type;

        QWidget* widget;
        switch (type) {
        case IntValue: {
            QSpinBox * const spinbox = new QSpinBox(this);
            spinbox->setRange(-999999999, 999999999);
            spinbox->setSingleStep(1);
            spinbox->setValue(defaultValue.toInt());
            widget = spinbox;
            break; }
        case DoubleValue: {
            QDoubleSpinBox * const spinbox = new QDoubleSpinBox(this);
            spinbox->setRange(-999999999, 999999999);
            spinbox->setSingleStep(0.1);
            spinbox->setValue(defaultValue.toDouble());
            widget = spinbox;
            break; }
        case StringValue: {
            QLineEdit * const edit = new QLineEdit(this);
            edit->setText(defaultValue);
            widget = edit;
            break; }
        case BoolValue: {
            QCheckBox * const edit = new QCheckBox(this);
            edit->setChecked(defaultValue.toLower() == QLatin1String("true"));
            widget = edit;
            break; }
        case ComboValue: {
            QComboBox* const edit = new QComboBox(this);
            edit->addItems( defaultValue.split("|"));
            widget = edit;
            break; }
        }
        gridLayout->addWidget(widget, line, 1);
        widget->setToolTip(description);
        widgets.append(widget);

        QLabel * const label = new QLabel(description, this);
        label->setBuddy(widget);
        widget->setToolTip(description);
        gridLayout->addWidget(label, line, 0);
    }

    gridLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                QSizePolicy::Expanding), gridLayout->rowCount(), 0);

    if (!widgets.isEmpty())
        widgets.value(0)->setFocus();
}

void ParameterDialog::apply()
{
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();

    for (unsigned i = 0; i < unsigned(widgets.size()); ++i) {
        InteractiveEntry *entry = &mEntries[i];
        const QString xpath = entry->xpath;
        const int type = entry->type;
        const QWidget * const widget = widgets[i];



        switch (type) {
        case IntValue:
            entry->result =
                QString::number(dynamic_cast<const QSpinBox*>(widget)->value());
            break;
        case DoubleValue:
            entry->result =
                QString::number(dynamic_cast<const QDoubleSpinBox*>(widget)->value());
            break;
        case StringValue:
            entry->result = dynamic_cast<const QLineEdit*>(widget)->text();
            break;
        case BoolValue:
            entry->result = dynamic_cast<const QAbstractButton*>(widget)
                ->isChecked() ? QLatin1String("true") : QLatin1String("false");
            break;
        case ComboValue:
            entry->result= dynamic_cast<const QComboBox*>(widget)->currentText();
            break;
        default:
            qFatal("invalid type: %u", type);
        }

        if (xpath.isEmpty()) {
            qDebug("Empty xpath expression found, not transforming");
            continue;
        }

//        qDebug("Applying expression %s of type %i", qPrintable(xpath), type);

        // do xpath stuff
#if defined(Q_OS_LINUX) || defined(Q_CC_MSVC)
        XercesDOMSupport theDomSupport;
        XercesParserLiaison theLiaison(theDomSupport);
#else
        XercesParserLiaison theLiaison;
        XercesDOMSupport theDomSupport(theLiaison);
#endif


        theLiaison.setBuildWrapperNodes(true);
        theLiaison.setBuildMaps(true);

        XalanDocument* xalanDocument = theLiaison.createDocument(document);
        XalanNode * const contextNode = xalanDocument;
        Q_ASSERT(xalanDocument);

        XercesDocumentWrapper * const docWrapper =
            theLiaison.mapDocumentToWrapper(xalanDocument);
        Q_ASSERT(docWrapper);

        XalanElement * namespaceNode = xalanDocument->getDocumentElement();
        Q_ASSERT(namespaceNode);

        XPathEvaluator theEvaluator;
        XalanDocumentPrefixResolver thePrefixResolver(docWrapper);
        NodeRefList resultList;

        try {
            theEvaluator.selectNodeList(resultList, theDomSupport, contextNode,
                S2X(xpath), thePrefixResolver);
        } catch (XalanXPathException& e) {
            throw ApexStringException(
                    tr("Invalid XPath expression in interactive entry (%1): %2")
                    .arg(xpath)
                    .arg(X2S(e.getMessage().data()) ));
        } catch (...) {
            throw ApexStringException(tr("Invalid XPath expression"));
        }

        if (!resultList.getLength()) {
            QMessageBox::warning(this, tr("Invalid query"),
                    tr("Warning: your query \"%1\" did not return any results, "
                       "therefore the requested new value will be ignored.")
                    .arg(xpath));
            continue;
        }

        for (unsigned i = 0; i < resultList.getLength(); ++i) {
            XalanNode * const xalanNode = resultList.item(i);

            XalanDOMString nodeName = xalanNode->getNodeName();
            const DOMNode * const xercesNode = docWrapper->mapNode(xalanNode);

            const QString value =
                XMLutils::GetFirstChildText((DOMElement*)xercesNode);

//            qDebug("Found value: %s", qPrintable(value));

            const int nodeType = xercesNode->getNodeType();
            if (nodeType != DOMNode::ELEMENT_NODE &&
                nodeType != DOMNode::ATTRIBUTE_NODE) {
                qDebug("Unknown node type");
                continue;
            }

//            qDebug("Set node to value: %s", qPrintable(entry->result));
            if (xercesNode->getFirstChild()) {
                xercesNode->getFirstChild()->setNodeValue(S2X(entry->result));
            } else {
                DOMNode * const n = const_cast<DOMNode*>(xercesNode);
                Q_ASSERT(n);
                n->appendChild(document->createTextNode(S2X(entry->result)));
            }

            entry->succeeded = true;
        }
    }

    XPathEvaluator::terminate();
    XMLPlatformUtils::Terminate();
}

data::ParameterDialogResults* ParameterDialog::results() const
{
    std::auto_ptr<data::ParameterDialogResults> result
        (new data::ParameterDialogResults);

    Q_FOREACH (const InteractiveEntry &entry, mEntries)
        result->push_back(data::ParameterDialogResult(entry.xpath,
                    entry.description, entry.result, entry.succeeded));

    return result.release();
}

const QList<ParameterDialog::InteractiveEntry>& ParameterDialog::entries() const
{
    return mEntries;
}

} // namespace apex
