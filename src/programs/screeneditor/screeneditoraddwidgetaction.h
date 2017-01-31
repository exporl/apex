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

#ifndef _EXPORL_SRC_PROGRAMS_SCREENEDITOR_SCREENEDITORADDWIDGETACTION_H_
#define _EXPORL_SRC_PROGRAMS_SCREENEDITOR_SCREENEDITORADDWIDGETACTION_H_

#include "apexdata/screen/screen.h"

#include "screeneditor.h"
#include "screenelementcreatortemplate.h"

#include <QAction>

namespace apex
{
  namespace editor
  {

    /**
     * This base class is only necessary because Qt moc can't handle
     * templates..
     */
    class ScreenEditorAddWidgetActionBase
      : public QAction
    {
      Q_OBJECT
    protected:
      ScreenEditor* editor;
      QString elName;
    public:
      ScreenEditorAddWidgetActionBase( ScreenEditor* e, const QString& name );
    public slots:
      void addWidgetSlot();
    protected:
      virtual void addWidgetSlotCalled() = 0;
    };

    /**
     * This template class is a \ref QAction that calls
     * addWidgetClicked() on the given \ref ScreenEditor when it is
     * triggered.  It will call addWidgetClicked() with a newly
     * created instance of \ref
     * ScreenElementCreatorTemplate<ElementT>.  This will make sure
     * that a new element of type ElementT ( which should be a
     * subclass of \ref ScreenElement, not \ref
     * ScreenElementEditorDelegate ! ) is added to the screen being
     * edited.
     */
    template <typename ElementT>
    class ScreenEditorAddWidgetAction
      : public ScreenEditorAddWidgetActionBase
    {
    public:
      ScreenEditorAddWidgetAction( ScreenEditor* e, const QString& name );
      void addWidgetSlotCalled();
    };

    template <typename ElementT>
    ScreenEditorAddWidgetAction<ElementT>::ScreenEditorAddWidgetAction( ScreenEditor* e, const QString& name )
      : ScreenEditorAddWidgetActionBase( e, name )
    {
    }

    template <typename ElementT>
    void ScreenEditorAddWidgetAction<ElementT>::addWidgetSlotCalled()
    {
      editor->addWidgetClicked(
        new ScreenElementCreatorTemplate<ElementT>( elName )
        );
    }

  }
}
#endif
