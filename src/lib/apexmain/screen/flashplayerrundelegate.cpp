/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "../gui/flashwidget.h"
#include "../gui/guidefines.h"

#include "../runner/experimentrundelegate.h"

#include "apexdata/screen/flashplayerelement.h"

#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "apexcontrol.h"
#include "fileprefixconvertor.h"
#include "flashplayerrundelegate.h"
#include "screenrundelegate.h"

using namespace apex::rundelegates;
using namespace apex;

FlashPlayerRunDelegate::FlashPlayerRunDelegate(ExperimentRunDelegate *p_rd,
                                               QWidget *parent,
                                               const FlashPlayerElement *e)
    : ScreenElementRunDelegate(p_rd, e), element(e)
{
    fw = new FlashWidget(parent);
    if (!fw->initialized())
        throw(ApexStringException(
            tr("FlashPlayer: could not create flash object. "
               "Check whether flash is installed on your computer.")));

    // fw->hide();
    // Get BG color from parent

    // FIXME: for some reason this does not get the right color blue/white
    /*QPalette np = parent->palette();
    np.setColor(QPalette::Active, QPalette::Window, element->getBGColor());
    np.setColor(QPalette::Active, QPalette::Button,
    apex::gui::sc_DefaultBGColor);
    fw->setPalette(np);*/

    fw->setAutoFillBackground(true);
    // set absolute file path for movie
    setMovie(e->getDefault(), true);

    QObject::connect(fw, SIGNAL(clicked()), this, SLOT(sendAnsweredSignal()));

    fw->setShortcut(e->getShortCut());
}

FlashPlayerRunDelegate::~FlashPlayerRunDelegate()
{
}

const ScreenElement *FlashPlayerRunDelegate::getScreenElement() const
{
    return element;
}

void FlashPlayerRunDelegate::sendAnsweredSignal()
{
    Q_EMIT answered(this);
}

void FlashPlayerRunDelegate::connectSlots(ScreenRunDelegate *d)
{
    connect(this, SIGNAL(answered(ScreenElementRunDelegate *)), d,
            SIGNAL(answered(ScreenElementRunDelegate *)));
    // [Tom] changed from SLOT to SIGNAL
}

void FlashPlayerRunDelegate::setMovie(const QString &path, const bool startNow)
{
    QString absPath(ApexTools::addPrefix(
        path, FilePrefixConvertor::convert(element->prefix())));
    absPath = QFileInfo(absPath).absoluteFilePath();

    // qCDebug(APEX_RS, "Flash movie absolute path: %s", qPrintable(absPath));

    if (!fw->loadMovie(absPath))
        throw(ApexStringException(
            tr("FlashPlayer: could not load movie %1").arg(path)));
    if (startNow)
        mp_StartWait();
}

void FlashPlayerRunDelegate::mp_StartWait()
{
    /* apex::ApexControl& ctrl( apex::ApexControl::Get() );
     const int c_nEvtID = ctrl.mf_nGetScreenElementFinishedEvent();
     if( c_nEvtID != 0 )
     {
         ctrl.mf_pGetControlThread()->mp_StartWaitOnPoll< FlashWidget >( 5000,
     c_nEvtID,
                                                                         &FlashWidget::isFinished,
     this );
     }*/
    connect(fw, SIGNAL(finished()), this, SLOT(movieFinished()));
    fw->play();
}

void FlashPlayerRunDelegate::feedBack(const FeedbackMode &mode)
{
    if (element->getOverrideFeedback()) {
        switch (mode) {
        case NoFeedback:
            setMovie(element->getDefault());
            break;
        case HighlightFeedback:
            setMovie(element->getHighlight());
            break;
        case PositiveFeedback:
            setMovie(element->getPositive());
            break;
        case NegativeFeedback:
            setMovie(element->getNegative());
            break;
        };
    } else {
        ScreenElementRunDelegate::feedBack(mode);
    }
}

void FlashPlayerRunDelegate::setEnabled(const bool e)
{
    bool value = e;
    if (element->getDisabled()) // never enable if disabled==true
        value = false;

    fw->setEnabled(e);
}

void FlashPlayerRunDelegate::setVisible(bool b)
{
    fw->setVisible(b);
    if (b)
        mp_StartWait();
    else
        fw->stop();
}

QWidget *FlashPlayerRunDelegate::getWidget()
{
    return fw;
}

// FIXME!!!
void FlashPlayerRunDelegate::movieFinished()
{
    // apex::ApexControl& ctrl( apex::ApexControl::Get() );
    // const int c_nEvtID = ctrl.mf_nGetScreenElementFinishedEvent();
    // ctrl.mf_pGetControlThread()->mp_StartWaitOnTimer(0,c_nEvtID );
    // disconnect(fw, SIGNAL(finished()), this, SLOT(movieFinished()));
}
