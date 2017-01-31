/**************
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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREENSDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREENSDATA_H_

#include "../fileprefix.h"

#include "apextools/apextypedefs.h"
#include "apextools/global.h"

#include "apextools/status/errorlogger.h"

#include "apextools/xml/xercesinclude.h"

#include <QStringList>

namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
};

class QObject;

namespace apex
{
namespace data
{
class Screen;
class ScreenElement;
class ButtonGroup;
}

namespace data
{


enum gt_eGuiMode
{
    gc_eNormal,
    gc_eChild
};

enum tFeedbackOn
{
    HIGHLIGHT_NONE,
    HIGHLIGHT_CORRECT,
    HIGHLIGHT_ANSWER
};

typedef QList< QPair<QString, QString> > FeedbackPluginParameters;
typedef QPair<QString, FeedbackPluginParameters> FeedbackPluginPair;

//using data::Screen;

/**
 * The ScreensData keeps the data about all the Screens in an
 * Experiment.  It contains a set of \ref Screen's, it has a
 * id-to-screen-map to quickly find a screen with a given id,
 * and has some data about the default font for all screens.
 */
class APEXDATA_EXPORT ScreensData : public ErrorLogger
{
    public:
        typedef std::map<QString, Screen*> ScreenMap;

        ScreensData();
        ~ScreensData();

        const Screen& GetScreen(const QString& id) const;
        Screen& GetScreen(const QString& id);
        void manageScreen(Screen* s);
        const ScreenMap& getScreens() const;
        ScreenMap& getScreens();

        bool hasDefaultFont() const;
        bool hasDefaultFontSize() const;
        QString defaultFont() const;
        int defaultFontSize() const;
        void setDefaultFont(const QString& font);
        void setDefaultFontSize(int fs);

        const FilePrefix& prefix() const;
        void setPrefix(FilePrefix prefix);
        bool hasPrefix() const;

        bool hasProgressbarEnabled() const;
        void setProgressbarEnabled(bool show);

        bool hasFeedbackEnabled() const;
        bool hasFeedbackLengthDefined() const;
        unsigned feedbackLength() const;
        void setFeedbackLength(unsigned length);
        void setFeedbackEnabled(bool show);

        tFeedbackOn feedbackOn() const;
        QString feedbackOnString() const;
        void setFeedbackOn(tFeedbackOn on);

        QString feedbackPositivePicture() const;
        QString feedbackNegativePicture() const;
        void setFeedbackPositivePicture(QString picture);
        void setFeedbackNegativePicture(QString picture);

        bool hasShowCurrentEnabled() const;
        void setShowCurrentEnabled(bool show);

        bool hasFullScreenEnabled() const;
        void setFullScreenEnabled(bool enable);

        bool hasPanelEnabled() const;
        void setPanelEnabled(bool enable);

        bool hasRepeatButtonEnabled() const;
        void setRepeatButtonEnabled(bool enable);

        bool hasStatusPictureEnabled() const;
        void setStatusPictureEnabled(bool enable);
        QString statusAnsweringPicture() const;
        QString statusListeningPicture() const;
        QString statusWaitingForStartPicture() const;

        bool hasMenuEnabled() const;
        void setMenuEnabled(bool enable);

        bool hasStopButtonEnabled() const;
        void setStopButtonEnabled(bool enable);

        bool hasInterTrialScreen() const;
        QString interTrialScreen() const;
        unsigned interTrialLength() const;
        void setInterTrialScreen(QString screen);
        void setInterTrialLength(unsigned length);

        bool hasStyle() const;
        void setStyle(const QString s);
        QString style() const;

        bool hasApexStyle() const;
        void setApexStyle(const QString s);
        QString apexStyle() const;

        bool hasIntroScreen() const;
        QString introScreen() const;
        void setIntroScreen(QString screen);
        unsigned introLength() const;
        void setIntroLength(unsigned length);

        bool hasOutroScreen() const;
        QString outroScreen() const;
        void setOutroScreen(QString screen);
        unsigned outroLength() const;
        void setOutroLength(unsigned length);

        bool hasPanelMovie() const;
        QString panelMovie() const;
        void setPanelMovie(QString movie);

        bool hasChildmode() const;
        bool hasGeneralScreenData() const;
        bool hasReinforcement() const;

        const QList<FeedbackPluginPair>& feedbackPlugins() const;
        void addFeedbackPlugin(const QString& name,
                               const FeedbackPluginParameters& params);

        bool operator==(const ScreensData& other);

        gt_eGuiMode  m_eMode;

    private:

        QString     standardFont;
        int         standardFontSize;
        bool        showFeedback;
        bool        showCurrent;
        bool        fullScreen;
        tFeedbackOn showFeedbackOn;
        bool        showMenu;
        unsigned    beforeLength;
        unsigned    afterLength;
        unsigned    betweenTrialsLength;
        unsigned    lengthOfFeedback;
        bool        feedbackLengthDefined;
        bool        showPanel;
        bool        showProgress;
        bool        showStopButton;
        bool        showRepeatButton;
        bool        showStatusPicture;
        QString     beforeScreen;
        QString     afterScreen;
        QString     betweenTrialsScreen;
        QString     styleSheet;
        QString     apexStyleSheet;
        QString     movieOfPanel;

        ScreenMap screens;

        FilePrefix  uriPrefix;
        QString     feedbackPositive;
        QString     feedbackNegative;
        QList<FeedbackPluginPair> feedbackPluginList;
};


}
}

#endif
