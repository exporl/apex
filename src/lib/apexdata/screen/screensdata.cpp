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
#include "apextools/exceptions.h"

#include "screen/screen.h"

#include "screensdata.h"

namespace apex
{
namespace data
{
ScreensData::ScreensData()
    : m_eMode(gc_eNormal),
      standardFontSize(-1),
      showFeedback(false),
      showCurrent(false),
      fullScreen(false),
      showFeedbackOn(HIGHLIGHT_CORRECT),
      showMenu(true),
      beforeLength(0),
      afterLength(0),
      betweenTrialsLength(0),
      lengthOfFeedback(1000),
      feedbackLengthDefined(false),
      showPanel(true),
      showProgress(true),
      showStopButton(false),
      showRepeatButton(false),
      showStatusPicture(false)
{
}

ScreensData::~ScreensData()
{
    ScreenMap::const_iterator it;
    for (it = screens.begin(); it != screens.end(); it++)
        delete it->second;
}

const Screen &ScreensData::GetScreen(const QString &id) const
{
    ScreenMap::const_iterator i = screens.find(id);
    Q_ASSERT(i != screens.end());
    return *i->second;
}

void ScreensData::manageScreen(Screen *s)
{
    screens[s->getID()] = s;
    // keep( s );
}

const FilePrefix &ScreensData::prefix() const
{
    return filePrefix;
}

void ScreensData::setPrefix(FilePrefix prefix)
{
    filePrefix = prefix;
}

bool ScreensData::hasPrefix() const
{
    return !filePrefix.value().isEmpty();
}

bool ScreensData::hasDefaultFont() const
{
    return !standardFont.isEmpty();
}

bool ScreensData::hasDefaultFontSize() const
{
    return standardFontSize > 0;
}

void ScreensData::setDefaultFontSize(int fs)
{
    standardFontSize = fs;
}

void ScreensData::setDefaultFont(const QString &font)
{
    standardFont = font;
}

int ScreensData::defaultFontSize() const
{
    return standardFontSize;
}

QString ScreensData::defaultFont() const
{
    return standardFont;
}

const ScreensData::ScreenMap &ScreensData::getScreens() const
{
    return screens;
}

Screen &ScreensData::GetScreen(const QString &id)
{
    ScreenMap::iterator i = screens.find(id);
    if (i == screens.end()) {
        throw ApexStringException(tr("Screen not found: %1").arg(id));
    }
    return *i->second;
}

ScreensData::ScreenMap &ScreensData::getScreens()
{
    return screens;
}

bool ScreensData::hasStyle() const
{
    return !styleSheet.isEmpty();
}

void ScreensData::setStyle(const QString s)
{
    styleSheet = s;
};

QString ScreensData::style() const
{
    return styleSheet;
};

bool ScreensData::hasApexStyle() const
{
    return !apexStyleSheet.isEmpty();
}

void ScreensData::setApexStyle(const QString s)
{
    apexStyleSheet = s;
};
QString ScreensData::apexStyle() const
{
    return apexStyleSheet;
};

bool ScreensData::hasProgressbarEnabled() const
{
    return showProgress;
}

void ScreensData::setProgressbarEnabled(bool show)
{
    showProgress = show;
}

bool ScreensData::hasFeedbackEnabled() const
{
    return showFeedback;
}

void ScreensData::setFeedbackEnabled(bool show)
{
    showFeedback = show;
}

unsigned ScreensData::feedbackLength() const
{
    return lengthOfFeedback;
}

bool ScreensData::hasFeedbackLengthDefined() const
{
    return feedbackLengthDefined;
}

void ScreensData::setFeedbackLength(unsigned length)
{
    lengthOfFeedback = length;
    feedbackLengthDefined = true;
}

tFeedbackOn ScreensData::feedbackOn() const
{
    return showFeedbackOn;
}

QString ScreensData::feedbackOnString() const
{
    switch (showFeedbackOn) {
    case HIGHLIGHT_NONE:
        return "none";
    case HIGHLIGHT_CORRECT:
        return "correct";
    case HIGHLIGHT_ANSWER:
        return "clicked";
    }

    Q_ASSERT(false);
    return QString();
}

void ScreensData::setFeedbackOn(tFeedbackOn on)
{
    showFeedbackOn = on;
}

QString ScreensData::feedbackPositivePicture() const
{
    return feedbackPositive;
}

QString ScreensData::feedbackNegativePicture() const
{
    return feedbackNegative;
}

void ScreensData::setFeedbackPositivePicture(QString picture)
{
    feedbackPositive = picture;
}

void ScreensData::setFeedbackNegativePicture(QString picture)
{
    feedbackNegative = picture;
}

bool ScreensData::hasShowCurrentEnabled() const
{
    return showCurrent;
}

void ScreensData::setShowCurrentEnabled(bool show)
{
    showCurrent = show;
}

bool ScreensData::hasFullScreenEnabled() const
{
    return fullScreen;
}

void ScreensData::setFullScreenEnabled(bool enable)
{
    fullScreen = enable;
}

bool ScreensData::hasPanelEnabled() const
{
    return showPanel;
}

void ScreensData::setPanelEnabled(bool enable)
{
    showPanel = enable;
}

bool ScreensData::hasRepeatButtonEnabled() const
{
    return showRepeatButton;
}

void ScreensData::setRepeatButtonEnabled(bool enable)
{
    showRepeatButton = enable;
}

void ScreensData::setStatusPictureEnabled(bool enable)
{
    showStatusPicture = enable;
}

bool ScreensData::hasStatusPictureEnabled() const
{
    return showStatusPicture;
}

QString ScreensData::statusAnsweringPicture() const
{
    return QString(); // FIXME: add to experiment file
}

QString ScreensData::statusListeningPicture() const
{
    return QString();
}

QString ScreensData::statusWaitingForStartPicture() const
{
    return QString();
}

bool ScreensData::hasMenuEnabled() const
{
    return showMenu;
}

void ScreensData::setMenuEnabled(bool enable)
{
    showMenu = enable;
}

bool ScreensData::hasStopButtonEnabled() const
{
    return showStopButton;
}

void ScreensData::setStopButtonEnabled(bool enable)
{
    showStopButton = enable;
}

bool ScreensData::hasInterTrialScreen() const
{
    return !betweenTrialsScreen.isEmpty();
}

QString ScreensData::interTrialScreen() const
{
    return betweenTrialsScreen;
}

unsigned ScreensData::interTrialLength() const
{
    return betweenTrialsLength;
}

void ScreensData::setInterTrialScreen(QString screen)
{
    betweenTrialsScreen = screen;
}

void ScreensData::setInterTrialLength(unsigned length)
{
    betweenTrialsLength = length;
}

bool ScreensData::hasGeneralScreenData() const
{
    return fullScreen || showPanel || showRepeatButton || showMenu ||
           showStopButton || !betweenTrialsScreen.isEmpty();
}

bool ScreensData::hasReinforcement() const
{
    return hasProgressbarEnabled() || hasFeedbackEnabled() ||
           hasFeedbackLengthDefined() || hasShowCurrentEnabled();
}

bool ScreensData::hasIntroScreen() const
{
    return !beforeScreen.isEmpty();
}

QString ScreensData::introScreen() const
{
    return beforeScreen;
}

void ScreensData::setIntroScreen(QString screen)
{
    beforeScreen = screen;
}

unsigned ScreensData::introLength() const
{
    return beforeLength;
}

void ScreensData::setIntroLength(unsigned length)
{
    beforeLength = length;
}

bool ScreensData::hasOutroScreen() const
{
    return !afterScreen.isEmpty();
}

QString ScreensData::outroScreen() const
{
    return afterScreen;
}

void ScreensData::setOutroScreen(QString screen)
{
    afterScreen = screen;
}

unsigned ScreensData::outroLength() const
{
    return afterLength;
}

void ScreensData::setOutroLength(unsigned length)
{
    afterLength = length;
}

bool ScreensData::hasPanelMovie() const
{
    return !movieOfPanel.isEmpty();
}

QString ScreensData::panelMovie() const
{
    return movieOfPanel;
}

void ScreensData::setPanelMovie(QString movie)
{
    movieOfPanel = movie;
}

const QList<FeedbackPluginPair> &ScreensData::feedbackPlugins() const
{
    return feedbackPluginList;
}

void ScreensData::addFeedbackPlugin(const QString &name,
                                    const FeedbackPluginParameters &params)
{
    feedbackPluginList.push_back(FeedbackPluginPair(name, params));
}

bool ScreensData::hasChildmode() const
{
    return hasIntroScreen() || hasOutroScreen() || hasPanelMovie();
}

bool ScreensData::operator==(const ScreensData &other)
{
    if (!ApexTools::areEqualPointerMaps(screens, other.screens)) {
        qCDebug(APEX_RS, "screenmaps are not equal");
        return false;
    }

    return standardFont == other.standardFont &&
           standardFontSize == other.standardFontSize &&
           showFeedback == other.showFeedback &&
           showCurrent == other.showCurrent && fullScreen == other.fullScreen &&
           showFeedbackOn == other.showFeedbackOn &&
           showMenu == other.showMenu && beforeLength == other.beforeLength &&
           afterLength == other.afterLength &&
           betweenTrialsLength == other.betweenTrialsLength &&
           lengthOfFeedback == other.lengthOfFeedback &&
           feedbackLengthDefined == other.feedbackLengthDefined &&
           showPanel == other.showPanel && showProgress == other.showProgress &&
           showStopButton == other.showStopButton &&
           showRepeatButton == other.showRepeatButton &&
           beforeScreen == other.beforeScreen &&
           afterScreen == other.afterScreen &&
           betweenTrialsScreen == other.betweenTrialsScreen &&
           styleSheet == other.styleSheet &&
           apexStyleSheet == other.apexStyleSheet &&
           movieOfPanel == other.movieOfPanel &&
           feedbackPositive == other.feedbackPositive &&
           feedbackNegative == other.feedbackNegative &&
           filePrefix == other.filePrefix;
}
}
}
