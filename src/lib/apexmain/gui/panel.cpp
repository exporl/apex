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

#include "apexdata/screen/screensdata.h"

#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "fileprefixconvertor.h"
#include "guidefines.h"
#include "panel.h"
#include "panelelements.h"

#ifdef FLASH
#include "flashwidget.h"
#endif

#include <QGridLayout>
#include <QPushButton>
#include <QSpacerItem>

namespace apex
{
namespace gui
{

Panel::Panel(QWidget *a_pParent, const data::ScreensData *mwndc)
    : QWidget(a_pParent),
      IPanel(),
      m_pMainWindowConfig(mwndc),
      m_pStart(new QPushButton("Start", this)),
      m_pStop(new QPushButton("Stop", this)),
      m_pPause(new QPushButton("Pause", this)),
      m_pEmergency(new QPushButton("Stop Output", this)),
      m_pRepeat(new QPushButton(tr("Repeat stimulus"), this)),
      m_pProgressBar(new ApexProgressBar(this)),
      m_pStatusWindow(new ApexStatusWindow(this)),
      m_pFeedbackPicture(new ApexFeedbackPicture(
          this,
          ApexTools::addPrefix(mwndc->feedbackPositivePicture(),
                               FilePrefixConvertor::convert(mwndc->prefix())),
          ApexTools::addPrefix(mwndc->feedbackNegativePicture(),
                               FilePrefixConvertor::convert(mwndc->prefix())))),
      m_pLayout(new QGridLayout(this))
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Button);
    mp_Show(false);
    connect(m_pStart, SIGNAL(released()), getSignalSlotProxy(),
            SIGNAL(ms_Start()));
    connect(m_pStop, SIGNAL(released()), getSignalSlotProxy(),
            SIGNAL(ms_Stop()));
    connect(m_pPause, SIGNAL(released()), getSignalSlotProxy(),
            SIGNAL(ms_Pause()));
    connect(m_pEmergency, SIGNAL(released()), getSignalSlotProxy(),
            SIGNAL(ms_Panic()));
    connect(m_pRepeat, SIGNAL(released()), getSignalSlotProxy(),
            SIGNAL(ms_Repeat()));
    m_pEmergency->setAutoFillBackground(true);
    // m_pProgressBar->setAutoFillBackground( true );

    m_pEmergency->setStyleSheet("* {background-color: red}");
    //  widgetattributes::gf_SetBackGroundColor( m_pEmergency, Qt::red );
    //  widgetattributes::gf_SetBackGroundColor( m_pStart, sc_DefaultPanelColor
    //  );
    //  widgetattributes::gf_SetBackGroundColor( m_pStop, sc_DefaultPanelColor
    //  );
    //  widgetattributes::gf_SetBackGroundColor( m_pPause, sc_DefaultPanelColor
    //  );
    //  widgetattributes::gf_SetBackGroundColor( m_pRepeat, sc_DefaultPanelColor
    //  );
    // widgetattributes::gf_SetBackGroundColor( m_pProgressBar,
    // sc_DefaultPanelColor );
    m_pProgressBar->setStyleSheet("QProgressBar::chunk {} "
                                  "QProgressBar {text-align:center;}");

    if (m_pMainWindowConfig->hasStatusPictureEnabled())
        m_pStatusPicture =
            new StatusPicture(this, mwndc->statusAnsweringPicture(),
                              mwndc->statusListeningPicture(),
                              mwndc->statusWaitingForStartPicture());
    else
        m_pStatusPicture = 0;

    int maxbuttonheight = QFontMetrics(m_pStart->font()).height() * 6;
    m_pStart->setMaximumHeight(maxbuttonheight);
    m_pStop->setMaximumHeight(maxbuttonheight);
    m_pPause->setMaximumHeight(maxbuttonheight);

    m_pStart->setSizePolicy(QSizePolicy::Minimum,
                            QSizePolicy::MinimumExpanding);
    m_pStop->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    m_pPause->setSizePolicy(QSizePolicy::Minimum,
                            QSizePolicy::MinimumExpanding);

    mp_EnableStart(false);
    mp_EnableStop(false);
    mp_EnablePause(false);
    mp_EnableRepeat(false);
}

Panel::~Panel()
{
    // parent should delete everything
}

bool Panel::mp_bSetConfig(const data::ScreensData &ac_Config)
{
    QFont fontr(m_pEmergency->font());
    fontr.setPointSize(18);
    fontr.setFamily(sc_sDefaultFont);

    m_pStart->setFont(fontr);
    m_pStop->setFont(fontr);
    m_pPause->setFont(fontr);

    mp_Layout();

    showProgress = ac_Config.hasProgressbarEnabled();
    showEmergency = ac_Config.hasStopButtonEnabled();
    showRepeat = ac_Config.hasRepeatButtonEnabled();

    return true;
}

void Panel::mp_Show(const bool ac_bVal)
{
    if (ac_bVal) {
        m_pStart->show();
        m_pStop->show();
        m_pPause->show();
        if (showEmergency)
            m_pEmergency->show();
        if (showProgress)
            m_pProgressBar->show();
        if (showRepeat)
            m_pRepeat->show();
        else
            m_pRepeat->hide();
        if (m_pMainWindowConfig->hasStatusPictureEnabled())
            m_pStatusPicture->show();
        m_pStatusWindow->show();
        m_pFeedbackPicture->show();
        QWidget::show();
    } else {
        m_pStart->hide();
        m_pStop->hide();
        m_pPause->hide();
        m_pEmergency->hide();
        m_pRepeat->hide();
        m_pProgressBar->hide();
        m_pStatusWindow->hide();
        m_pFeedbackPicture->hide();
        QWidget::hide();
    }
}

void Panel::mp_EnableStart(const bool ac_bVal)
{
    m_pStart->setDisabled(!ac_bVal);
    if (m_pMainWindowConfig->hasStatusPictureEnabled() && ac_bVal)
        m_pStatusPicture->setStatus(StatusPicture::STATUS_WAITING_FOR_START);
}

void Panel::mp_EnableStop(const bool ac_bVal)
{
    m_pStop->setDisabled(!ac_bVal);
    if (m_pMainWindowConfig->hasStatusPictureEnabled()) {
        if (m_pStart->isEnabled())
            m_pStatusPicture->setStatus(
                StatusPicture::STATUS_WAITING_FOR_START);
        else if (ac_bVal)
            m_pStatusPicture->setStatus(StatusPicture::STATUS_ANSWERING);
        else
            m_pStatusPicture->setStatus(StatusPicture::STATUS_LISTENING);
    }
}

void Panel::mp_EnablePause(const bool ac_bVal)
{
    m_pPause->setDisabled(!ac_bVal);
}

void Panel::mp_EnableEmergency(const bool ac_bVal)
{
    m_pEmergency->setDisabled(!ac_bVal);
}

void Panel::mp_EnableRepeat(const bool ac_bVal)
{
    m_pRepeat->setDisabled(!ac_bVal);
}

void Panel::feedBack(const ScreenElementRunDelegate::FeedbackMode ac_eMode)
{
    if (ac_eMode != ScreenElementRunDelegate::NoFeedback) {
        if (m_pMainWindowConfig->hasStatusPictureEnabled()) {
            m_pStatusPicture->setStatus(StatusPicture::STATUS_LISTENING);
        }
    }
    m_pFeedbackPicture->feedBack(ac_eMode);
}

void Panel::mp_SetProgress(const unsigned ac_nProgress)
{
    m_pProgressBar->setValue((int)ac_nProgress);
}

void Panel::mp_SetProgressSteps(const unsigned ac_nProgress)
{
    m_pProgressBar->setMaximum((int)ac_nProgress);
}

// TODO remove
void Panel::mp_Paused()
{
    if (m_pPause->text() == "Continue")
        m_pPause->setText("Pause");
    else
        m_pPause->setText("Continue");
}

void Panel::setPaused(bool paused)
{
    m_pPause->setText(paused ? tr("Continue") : tr("Pause"));
}

void Panel::mp_Layout()
{
    int rowcount = 0;
    m_pLayout->addWidget(m_pStart, rowcount++, 0);
    m_pLayout->addWidget(m_pStop, rowcount++, 0);
    m_pLayout->addWidget(m_pPause, rowcount++, 0);
    m_pLayout->addItem(new QSpacerItem(0, 10, QSizePolicy::Minimum,
                                       QSizePolicy::MinimumExpanding),
                       rowcount++, 0);
    m_pLayout->addWidget(m_pFeedbackPicture, rowcount++, 0, 1, 1,
                         Qt::AlignHCenter);
    m_pLayout->addItem(new QSpacerItem(0, 10, QSizePolicy::Minimum,
                                       QSizePolicy::MinimumExpanding),
                       rowcount++, 0);
    if (m_pMainWindowConfig->hasStatusPictureEnabled())
        m_pLayout->addWidget(m_pStatusPicture, rowcount++, 0, 1, 1,
                             Qt::AlignHCenter);
    m_pLayout->addWidget(m_pEmergency, rowcount++, 0);
    m_pLayout->addWidget(m_pRepeat, rowcount++, 0);
    m_pLayout->addWidget(m_pStatusWindow, rowcount++, 0);
    m_pLayout->addWidget(m_pProgressBar, rowcount++, 0);
}

void Panel::mp_SetText(const QString &ac_sText)
{
    if (ac_sText.isEmpty())
        m_pStatusWindow->clear();
    else
        m_pStatusWindow->mp_AddMessage(ac_sText);
}

#ifdef FLASH

ChildModePanel::ChildModePanel(QWidget *a_pParent)
    : QWidget(a_pParent),
      m_pMovie(new FlashWidget(this)),
      m_pProgressBar(new ApexProgressBar(this)),
      m_bShowProgressBar(false),
      m_nCurrentProgressFrame(0L),
      m_pLayout(new QVBoxLayout(this))
{
    if (!m_pMovie->initialized())
        throw ApexStringException(
            tr("Could not load panel movie. "
               "Check whether Flash is installed on your system."));

    m_pMovie->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);
    setAutoFillBackground(true);
    setPalette(Qt::white);

    m_pLayout->addWidget(m_pMovie);
    m_pLayout->addWidget(m_pProgressBar);
}

ChildModePanel::~ChildModePanel()
{
}

bool ChildModePanel::mp_bSetConfig(const data::ScreensData &ac_Config)
{
    if (ac_Config.panelMovie().isEmpty())
        return false;

    QString moviePath(
        ApexTools::addPrefix(ac_Config.panelMovie(),
                             FilePrefixConvertor::convert(ac_Config.prefix())));
    if (!m_pMovie->loadMovie(moviePath))
        throw ApexStringException(
            tr("ChildModePanel: cannot open panel movie %1").arg(moviePath));

    m_bShowProgressBar = ac_Config.hasProgressbarEnabled();

    return true;
}

void ChildModePanel::mp_Show(const bool ac_bVal)
{
    if (ac_bVal) {
        QWidget::show();
        m_pMovie->show();
        m_pMovie->gotoFrame(0L);
        if (m_bShowProgressBar)
            m_pProgressBar->show();
        else
            m_pProgressBar->hide();
    } else {
        m_pProgressBar->hide();
        m_pMovie->hide();
        QWidget::hide();
    }
}

void ChildModePanel::feedBack(
    const ScreenElementRunDelegate::FeedbackMode ac_eMode)
{
    m_pMovie->gotoFrame(m_nCurrentProgressFrame);

    if (ac_eMode == ScreenElementRunDelegate::NegativeFeedback) {
        if (m_nCurrentProgressFrame > 0)
            --m_nCurrentProgressFrame;
    } else if (ac_eMode == ScreenElementRunDelegate::PositiveFeedback)
        ++m_nCurrentProgressFrame;
}

void ChildModePanel::mp_SetProgress(const unsigned ac_nProgress)
{
    m_pProgressBar->setValue((int)ac_nProgress);
}

void ChildModePanel::mp_SetProgressSteps(const unsigned ac_nProgress)
{
    m_pProgressBar->setMaximum((int)ac_nProgress);
}
#endif //#ifdef FLASH

PanelQProxy *IPanel::getSignalSlotProxy()
{
    return objectProxy.data();
}
}
}
