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

#include "apextools/global.h"

#include "common/paths.h"

#include "panelelements.h"

#include <QResizeEvent>

using namespace cmn;

apex::gui::ApexFeedbackPicture::ApexFeedbackPicture(QWidget *a_pParent,
                                                    QString okFilename,
                                                    QString nOkFilename)
    : QLabel(a_pParent)
{
    if (okFilename.isEmpty()) {
        okPixmap = new QPixmap(Paths::searchFile(QL1S("feedback_ok.png"),
                                                 Paths::iconDirectories()));
    } else
        okPixmap = new QPixmap(okFilename);
    Q_ASSERT(okPixmap);

    if (nOkFilename.isEmpty())
        nokPixmap = new QPixmap(Paths::searchFile(QL1S("feedback_nok.png"),
                                                  Paths::iconDirectories()));
    else
        nokPixmap = new QPixmap(nOkFilename);
    Q_ASSERT(nokPixmap);

    setMinimumHeight(okPixmap->height());
    setMinimumWidth(okPixmap->width());
}

void apex::gui::ApexFeedbackPicture::feedBack(
    const ScreenElementRunDelegate::FeedbackMode mode)
{
    if (mode == ScreenElementRunDelegate::NoFeedback)
        setPixmap(QPixmap());
    else if (mode == ScreenElementRunDelegate::NegativeFeedback)
        setPixmap(*nokPixmap);
    else if (mode == ScreenElementRunDelegate::PositiveFeedback)
        setPixmap(*okPixmap);
    else if (mode == ScreenElementRunDelegate::HighlightFeedback)
        setPixmap(QPixmap());
}

apex::gui::ApexFeedbackPicture::~ApexFeedbackPicture()
{
    delete okPixmap;
    delete nokPixmap;
}

namespace apex
{
namespace gui
{

StatusPicture::StatusPicture(QWidget *parent, QString answeringFilename,
                             QString listeningFilename,
                             QString waitingForStartFilename)
    : QLabel(parent)
{
    QString targetFile;
    if (answeringFilename.isEmpty())
        targetFile =
            Paths::searchFile(QL1S("answering.png"), Paths::iconDirectories());
    else
        targetFile = answeringFilename;
    answeringPixmap = new QPixmap(targetFile);
    Q_ASSERT(!answeringPixmap->isNull());

    if (listeningFilename.isEmpty())
        targetFile =
            Paths::searchFile(QL1S("listening.png"), Paths::iconDirectories());
    else
        targetFile = listeningFilename;
    listeningPixmap = new QPixmap(targetFile);
    Q_ASSERT(!listeningPixmap->isNull());

    if (waitingForStartFilename.isEmpty())
        targetFile = Paths::searchFile(QL1S("waitforstart.png"),
                                       Paths::iconDirectories());
    else
        targetFile = waitingForStartFilename;
    waitingForStartPixmap = new QPixmap(targetFile);
    Q_ASSERT(!waitingForStartPixmap->isNull());

    setStatus(STATUS_WAITING_FOR_START);
}

StatusPicture::~StatusPicture()
{
    if (answeringPixmap)
        delete answeringPixmap;
    if (listeningPixmap)
        delete listeningPixmap;
    if (waitingForStartPixmap)
        delete waitingForStartPixmap;
}

void StatusPicture::setStatus(const status s)
{
    switch (s) {
    case STATUS_ANSWERING:
        setPixmap(*answeringPixmap);
        break;
    case STATUS_LISTENING:
        setPixmap(*listeningPixmap);
        break;
    case STATUS_WAITING_FOR_START:
        setPixmap(*waitingForStartPixmap);
        break;
    default:
        qCDebug(APEX_RS, "Unknown status");
    }
}

/*void apex::gui::ApexFeedbackPicture::resizeEvent( QResizeEvent* e )
{
    setIconSize( e->size() );
}*/

} // ns gui
} // ns apex
