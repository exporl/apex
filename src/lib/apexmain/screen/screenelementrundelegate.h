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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_SCREENELEMENTRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_SCREENELEMENTRUNDELEGATE_H_

#include <QCoreApplication>
#include <QPointF>

class QWidget;
class QString;
class QGridLayout;
class QLayout;

class ArcLayout;

namespace apex
{

class ExperimentRunDelegate;

namespace gui
{
class ScreenRunDelegate;
}

namespace data
{
class ScreenElement;
}

namespace rundelegates
{
using data::ScreenElement;
using gui::ScreenRunDelegate;

/**
 * \class ScreenElementRunDelegate ScreenElementRunDelegate.h
 * rundelegates/ScreenElementRunDelegate.h
 *
 * The ScreenElementRunDelegate class is a class that represents a
 * \ref ScreenElement towards a running Apex program.  It
 * separates purely runtime stuff like element feedback, QWidget
 * representation, signals from ApexControl etc. from the pure
 * data.  This is done so that the data classes can be used for
 * other purposes as well.
 *
 * A related class is \ref ScreenElementEditorDelegate, which has
 * a similar purpose, but towards a ScreenEditor instead of a
 * running Apex program.
 *
 * Ownership of these RunDelegates is managed by the class
 * creating them.  Normally this will be the class \ref
 * ScreenRunDelegate, which deletes its RunDelegates in its
 * destructor.
 *
 * This is an abstract base class.
 */
class ScreenElementRunDelegate
{
    Q_DECLARE_TR_FUNCTIONS(ScreenElementRunDelegate)
public:
    /**
     * Feedback mode.
     */
    enum FeedbackMode {
        NoFeedback,        //!< no feedback
        HighlightFeedback, //!< white border
        NegativeFeedback,  //!< red border
        PositiveFeedback   //!< green border
    };

protected:
    FeedbackMode feedBackMode;
    ScreenElementRunDelegate(ExperimentRunDelegate *p_parent,
                             const data::ScreenElement *const d);

public:
    QString getID() const;
    /**
     * Show feedback on this element.  The default implementation is
     * fine, but some element types override it with their proper
     * implementation.
     */
    virtual void feedBack(const FeedbackMode &mode);
    FeedbackMode getFeedBackMode() const;
    virtual ~ScreenElementRunDelegate();

    /**
     * In this function, the RunDelegate should add itself to the
     * given grid layout, either as a widget, either as a layout.
     *
     * Some elements need a special treatment by the layout, and
     * this function gives them the chance to do it themselves.  The
     * default implementation should be fine for most types.
     */
    virtual void addToGridLayout(QGridLayout *gl);

    /**
     * In this function, the RunDelegate should add itself to the
     * given arc layout, either as a widget, either as a layout.
     *
     * Some elements need a special treatment by the layout, and
     * this function gives them the chance to do it themselves.  The
     * default implementation should be fine for most types.
     */
    virtual void addToArcLayout(::ArcLayout *al);

    /**
     * Return the @ref ScreenElement that this RunDelegate
     * represents.  RunDelegates only have a constant pointer to
     * their ScreenElement, as they are not allowed to modify them..
     */
    virtual const ScreenElement *getScreenElement() const = 0;

    /**
     * Get the widget representing this RunDelegate on-screen.  This
     * will only be called if getLayout() returns 0.  Layout elements
     * should return 0 here, non-layout elements a widget which they
     * own.  No ownership is transferred..
     */
    virtual QWidget *getWidget() = 0;

    /**
     * Enable/Disable the widget.
     * A disabled element must not respond to user interaction when it's
     * disabled,
     * but is allowed to signal this by eg switching to a different color or
     * style.
     * @param ac_bEnable true for enable
     */
    virtual void setEnabled(const bool e = true);

    /**
     * Text content.
     * Elements displaying text or allowing to enter text should return true
     * here.
     * These elements must also re-implement the text properties below.
     * @return true if the widget has text of some kind
     */
    virtual bool hasText() const;
    /**
     * User editable text content.
     * If the element text is constant, eg in a label, it makes no sense to
     * report it in the results file, therefore this method will return false.
     * @return true if the returned text is useful for reporting
     */
    virtual bool hasInterestingText() const;
    /**
     * Get the text.
     * If the element allows entering text, this must return the updated text
     * before the Answered signal is emitted.
     * @return a reference to the text string
     */
    virtual const QString getText() const;

    /**
     * connect various signals and slots to the corresponding
     * signals/slots on the ScreenRunDelegate...
     */
    virtual void connectSlots(ScreenRunDelegate *d);

    /**
     * Return the layout for this element.  Non-layout elements should
     * return 0 here, as the default implementation does.  Layouts
     * return the layout they represent..  It should be an object
     * owned by them, no ownership is transferred here..
     */
    virtual QLayout *getLayout();

    /**
      * Return the relative position of the mouse click
      */
    virtual const QPointF getClickPosition() const;

    /**
     * Clear the text.
     */
    virtual void clearText();

protected:
    /**
     * Set poperties that are common to all screenelements, like
     * style sheet, fgcolor, ...
     */
    void setCommonProperties(QWidget *target);
    void setBgColor(QWidget *target, const QString &color);
    void setFgColor(QWidget *target, const QString &color);

    ExperimentRunDelegate *m_rd;
    QPointF lastClickPosition;

private:
    bool isValidColor(const QString &color);

    const data::ScreenElement *const element;
};
}
}
#endif
