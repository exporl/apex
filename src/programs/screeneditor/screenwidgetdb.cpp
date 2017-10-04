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

#include "apexdata/screen/answerlabelelement.h"
#include "apexdata/screen/arclayoutelement.h"
#include "apexdata/screen/buttonelement.h"
#include "apexdata/screen/flashplayerelement.h"
#include "apexdata/screen/gridlayoutelement.h"
#include "apexdata/screen/labelelement.h"
#include "apexdata/screen/parameterlistelement.h"
#include "apexdata/screen/pictureelement.h"
#include "apexdata/screen/picturelabelelement.h"
#include "apexdata/screen/texteditelement.h"

#include "screeneditoraddwidgetaction.h"
#include "screenwidgetdb.h"

namespace apex
{
namespace editor
{

using data::AnswerLabelElement;
using data::ArcLayoutElement;
using data::ButtonElement;
using data::FlashPlayerElement;
using data::GridLayoutElement;
using data::LabelElement;
using data::ParameterListElement;
using data::PictureElement;
using data::PictureLabelElement;
using data::TextEditElement;

ScreenWidgetDB::ScreenWidgetDB(ScreenEditor *e)
{
    QAction *addButtonAction = new ScreenEditorAddWidgetAction<ButtonElement>(
        e, QObject::tr("button"));
    addButtonAction->setText(QObject::tr("Button"));
    addWidgetActions.push_back(addButtonAction);

    QAction *addLabelAction =
        new ScreenEditorAddWidgetAction<LabelElement>(e, QObject::tr("label"));
    addLabelAction->setText(QObject::tr("Label"));
    addWidgetActions.push_back(addLabelAction);

    QAction *addAnswerLabelAction =
        new ScreenEditorAddWidgetAction<AnswerLabelElement>(
            e, QObject::tr("answer label"));
    addAnswerLabelAction->setText(QObject::tr("Answer Label"));
    addWidgetActions.push_back(addAnswerLabelAction);

    QAction *addParameterListAction =
        new ScreenEditorAddWidgetAction<ParameterListElement>(
            e, QObject::tr("parameter list"));
    addParameterListAction->setText(QObject::tr("Parameter List"));
    addWidgetActions.push_back(addParameterListAction);

    QAction *addFlashPlayerAction =
        new ScreenEditorAddWidgetAction<FlashPlayerElement>(
            e, QObject::tr("flash player"));
    addFlashPlayerAction->setText(QObject::tr("Flash Player"));
    addWidgetActions.push_back(addFlashPlayerAction);

    QAction *addTextEditAction =
        new ScreenEditorAddWidgetAction<TextEditElement>(
            e, QObject::tr("text edit"));
    addTextEditAction->setText(QObject::tr("Text Edit"));
    addWidgetActions.push_back(addTextEditAction);

    QAction *addPictureAction = new ScreenEditorAddWidgetAction<PictureElement>(
        e, QObject::tr("picture"));
    addPictureAction->setText(QObject::tr("Picture"));
    addWidgetActions.push_back(addPictureAction);

    QAction *addPictureLabelAction =
        new ScreenEditorAddWidgetAction<PictureLabelElement>(
            e, QObject::tr("picture label"));
    addPictureLabelAction->setText(QObject::tr("Picture Label"));
    addWidgetActions.push_back(addPictureLabelAction);

    QAction *addGridLayoutAction =
        new ScreenEditorAddWidgetAction<GridLayoutElement>(
            e, QObject::tr("grid layout"));
    addGridLayoutAction->setText(QObject::tr("Grid layout"));
    addLayoutActions.push_back(addGridLayoutAction);

    QAction *addArcLayoutAction =
        new ScreenEditorAddWidgetAction<ArcLayoutElement>(
            e, QObject::tr("arc layout"));
    addArcLayoutAction->setText(QObject::tr("Arc layout"));
    addLayoutActions.push_back(addArcLayoutAction);
}

QList<QAction *> ScreenWidgetDB::getAddWidgetActions()
{
    return addWidgetActions;
}

QList<QAction *> ScreenWidgetDB::getAddLayoutActions()
{
    return addLayoutActions;
}
}
}
