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

#include "apextools/apextools.h"

#include "gui/guidefines.h"
#include "gui/ratiolayout.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "screen/screenrundelegate.h"

#include "stimulus/stimulus.h"

#include "fileprefixconvertor.h"
#include "picturebuttonwidget.h"
#include "picturerundelegate.h"

#include <QApplication>
#include <QKeySequence>
#include <QMouseEvent>
#include <QShortcut>
#include <QStyle>
#include <QStyleOption>

namespace apex
{
namespace rundelegates
{

QWidget *PictureRunDelegate::getWidget()
{
    return this;
}

const ScreenElement *PictureRunDelegate::getScreenElement() const
{
    return element;
}

PictureRunDelegate::PictureRunDelegate(ExperimentRunDelegate *p_exprd,
                                       QWidget *parent, const PictureElement *e)
    : QWidget(parent),
      ScreenElementRunDelegate(p_exprd, e),
      element(e),
      m_button(new PictureButtonWidget(this)),
      m_pPixMap(0),
      m_pDisabled(0),
      m_pHighLight(0),
      m_pPositive(0),
      m_pNegative(0)
{
    setObjectName(element->getID());
    setStyleSheet(element->getStyle());

    m_button->setBorderSize(5);
    // connect( this, SIGNAL( released() ), this, SLOT( sendAnsweredSignal() )
    // );
    // connect( m_button, SIGNAL( clicked() ), this, SLOT( sendAnsweredSignal()
    // ) );
    connect(m_button, SIGNAL(mousePressed(const QPointF)), this,
            SLOT(sendAnsweredSignal(const QPointF &)));

    m_pPixMap = new QPixmap(
        ApexTools::addPrefix(element->getDefault(),
                             FilePrefixConvertor::convert(element->prefix())));
    if (!m_pPixMap->isNull())
        m_button->setPixmap(*m_pPixMap);
    else
        Q_ASSERT(element->getDefault().isEmpty());

    QStyleOption opt(0);
    opt.palette = QApplication::palette();

    if (element->getDisabled().isEmpty()) {
        m_pDisabled = new QPixmap(
            style()->generatedIconPixmap(QIcon::Disabled, *m_pPixMap, &opt));
    } else {
        m_pDisabled = new QPixmap(ApexTools::addPrefix(
            element->getDisabled(),
            FilePrefixConvertor::convert(element->prefix())));
        if (m_pDisabled->isNull())
            Q_ASSERT(element->getDisabled().isEmpty());
    }

    m_pHighLight = new QPixmap(
        ApexTools::addPrefix(element->getHighlight(),
                             FilePrefixConvertor::convert(element->prefix())));
    if (m_pHighLight->isNull())
        Q_ASSERT(element->getHighlight().isEmpty());

    m_pPositive = new QPixmap(
        ApexTools::addPrefix(element->getPositive(),
                             FilePrefixConvertor::convert(element->prefix())));
    if (m_pPositive->isNull())
        Q_ASSERT(element->getPositive().isEmpty());

    m_pNegative = new QPixmap(
        ApexTools::addPrefix(element->getNegative(),
                             FilePrefixConvertor::convert(element->prefix())));
    if (m_pNegative->isNull())
        Q_ASSERT(element->getNegative().isEmpty());

    if (!element->getShortCut().isEmpty()) {
        QShortcut *s =
            new QShortcut(e->getShortCut(), this); // FIXME: delete shortcut??
        connect(s, SIGNAL(activated()), this, SIGNAL(released()));
    }

    // create label and layout
    double ratio = double(m_pPixMap->height()) / double(m_pPixMap->width());
    RatioLayout *l = new RatioLayout(ratio);
    setLayout(l);
    l->addWidget(m_button);

    // m_button->setScaledContents(true);        // FIXME: keep aspect ratio
}

void PictureRunDelegate::sendAnsweredSignal(const QPointF &point)
{
    lastClickPosition = point;
    Q_EMIT answered(this);
}

void PictureRunDelegate::connectSlots(gui::ScreenRunDelegate *d)
{
    connect(this, SIGNAL(answered(ScreenElementRunDelegate *)), d,
            SIGNAL(answered(ScreenElementRunDelegate *)));

    connect(d, SIGNAL(newStimulus(stimulus::Stimulus *)), this,
            SLOT(newStimulus(stimulus::Stimulus *)));
}

void PictureRunDelegate::feedBack(const FeedbackMode &mode)
{
    if (element->getOverrideFeedback()) {
        if (mode == ScreenElementRunDelegate::NoFeedback)
            m_button->setPixmap(*m_pPixMap);
        else if (mode == ScreenElementRunDelegate::NegativeFeedback)
            m_button->setPixmap(*m_pNegative);
        else if (mode == ScreenElementRunDelegate::PositiveFeedback)
            m_button->setPixmap(*m_pPositive);
        else if (mode == ScreenElementRunDelegate::HighlightFeedback)
            m_button->setPixmap(*m_pHighLight);
    } else
        ScreenElementRunDelegate::feedBack(mode);

    // m_button->setScaledContents(true);
}

void PictureRunDelegate::setEnabled(const bool e)
{
    // qCDebug(APEX_RS, "PictureRunDelegate::setDisabled");
    ScreenElementRunDelegate::setEnabled(e);
    if (!e)
        m_button->setPixmap(*m_pDisabled);
    else
        m_button->setPixmap(*m_pPixMap);
}

void PictureRunDelegate::mouseReleaseEvent(QMouseEvent * /*event*/)
{
    //    qCDebug(APEX_RS, "release event");
    Q_EMIT(released());
}

void PictureRunDelegate::newStimulus(stimulus::Stimulus *stimulus)
{
    QString value;
    QString id(element->getFileId());
    if (id.isEmpty())
        return;

    ParameterManager *pm = m_rd->GetParameterManager();

    if (stimulus->GetVarParameters()->contains(id)) {
        value = stimulus->GetVarParameters()->value(id).toString();
    } else if (pm->parameterValue(id).isValid()) {
        value = pm->parameterValue(id).toString();
    } else {
        qCDebug(APEX_RS) << "Could not find parameter " << id;
    }

    QPixmap *t = new QPixmap(ApexTools::addPrefix(
        value, FilePrefixConvertor::convert(element->prefix())));

    if (t) {
        delete m_pPixMap;
        m_pPixMap = t;
        m_button->setPixmap(*m_pPixMap);

        QStyleOption opt(0);
        opt.palette = QApplication::palette();

        if (element->getDisabled().isEmpty()) {
            delete m_pDisabled;
            m_pDisabled = new QPixmap(style()->generatedIconPixmap(
                QIcon::Disabled, *m_pPixMap, &opt));
        }

    } else {
        qCWarning(
            APEX_RS, "%s",
            qPrintable(QSL("%1: %2").arg(
                "Picture", QString("Could not open image %1").arg(value))));
    }

    qCDebug(APEX_RS) << "PictureRunDelegate: param " << id << " = " << value;
}
}
}

apex::rundelegates::PictureRunDelegate::~PictureRunDelegate()
{
    delete m_button;
    delete m_pPixMap;
    delete m_pDisabled;
    delete m_pHighLight;
    delete m_pPositive;
    delete m_pNegative;
}
