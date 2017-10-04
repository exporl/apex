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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREENELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREENELEMENT_H_

#include "../fileprefix.h"
#include "apextools/apextypedefs.h"

#include <QHash>
#include <QString>
#include <QtCore>
#include <vector>

#include "apextools/global.h"

namespace apex
{

namespace data
{

using gui::ScreenElementMap;

class ScreenElement;
class ScreenElementVisitor;

/**
 * \class ScreenElement ScreenElement.h data/ScreenElement.h
 *
 * The ScreenElement class represents a visible element of a \ref
 * Screen.  ScreenElement's can have children, have an id, know
 * about their parent, etc.  This class is an abstract base class.
 *
 * This class is not aware of any type of on-screen
 * representation, it is only a data container, that contains the
 * data as parsed from the XML.  If you are looking for QWidgets
 * and such, you should be looking at \ref
 * ScreenElementRunDelegate or \ref ScreenElementEditorDelegate.
 *
 * Ownership of the ScreenElement's is managed by their parent
 * ScreenElement. A ScreenElement without a parent is a root
 * element, and is owned by the screen class containing the
 * screen.
 *
 * After creating a ScreenElement, you should not forget to call
 * Screen::addElement, to let the screen know about the
 * ScreenElement, and to allow it to keep its id-to-element map
 * consistent.
 */
class APEXDATA_EXPORT ScreenElement
{
    Q_DECLARE_TR_FUNCTIONS(ScreenElement);

    ScreenElement *parent;
    QString id;
    int x;
    int y;
    int fontSize;
    QString font;
    QHash<QString, QString> shortcuts;

    QString fgcolor;
    QString bgcolor;

    QString style;

    bool disabled;

    FilePrefix filePrefix;

private:
    virtual void deleteChild(ScreenElement *child);

protected:
    ScreenElement(const QString &id, ScreenElement *parent);

public:
    typedef enum {
        AnswerLabel,
        ArcLayout,
        Button,
        SpinBox,
        Slider,
        CheckBox,
        Empty,
        FlashPlayer,
        GridLayout,
        Label,
        ParameterList,
        Picture,
        PictureLabel,
        TextEdit,
        TwoPartLayout,
        ParameterLabel,
        Matrix,
        Html
    } ElementTypeT;

    /**
     * Don't delete ScreenElement's directly, always go via the
     * containing screen class, so it can keep its id-to-element map
     * consistent.
     */
    virtual ~ScreenElement();

    /**
     * Which type of ScreenElement are we looking at ?
     */
    virtual ElementTypeT elementType() const = 0;

    /**
     * Let the given
     * \ref ScreenElementVisitor visit this class.
     * This is part of the visitor design pattern.
     *
     * \see ScreenElementVisitor
     */
    virtual void visit(ScreenElementVisitor *v) = 0;
    virtual void visit(ScreenElementVisitor *v) const = 0;

    /**
     * Get this ScreenElement's parent.  This can return 0 if this
     * element is a root element.
     */
    ScreenElement *getParent();
    const ScreenElement *getParent() const;

    const QString getID() const;

    const FilePrefix &prefix() const;
    void setPrefix(const FilePrefix &prefix);

    // warning: don't call this method if the ScreenElement is in a
    // Screen already.  Call Screen->setElementID() to avoid data
    // inconsistencies..
    void setID(const QString &id);

    int getX() const
    {
        return x;
    }
    int getY() const
    {
        return y;
    }
    int getFontSize() const
    {
        return fontSize;
    }
    QString getShortCut(const QString &action = QLatin1String("click")) const;
    void setX(int nx)
    {
        x = nx;
    }
    void setY(int ny)
    {
        y = ny;
    }
    void setFontSize(int fs)
    {
        fontSize = fs;
    }
    void setShortCut(const QString &key,
                     const QString action = QLatin1String("click"));

    void setFGColor(const QString &p)
    {
        fgcolor = p;
    };
    const QString &getFGColor() const
    {
        return fgcolor;
    };
    void setBGColor(const QString &p)
    {
        bgcolor = p;
    };
    const QString &getBGColor() const
    {
        return bgcolor;
    };

    void setStyle(const QString s)
    {
        style = s;
    };
    QString getStyle() const
    {
        return style;
    };

    void setDisabled(bool p)
    {
        disabled = p;
    };
    bool getDisabled() const
    {
        return disabled;
    };
    void setFont(QString f)
    {
        font = f;
    };
    QString getFont() const
    {
        return font;
    };

    /**
     * Is this ScreenElement of a type that can have children ?
     */
    bool canHaveChildren() const;
    /**
     * How many children can this ScreenElement maximally have (
     * with its current properties ) ?  E.g. a GridLayout with width
     * 5 and height 3 will return 15 here.
     */
    virtual int getNumberOfChildPlaces() const;
    /**
     * This is for use in a ScreenEditor.  Normally not every place
     * of a layout ScreenElement has to be filled.  However,
     * sometimes we want every place to be filled.  After calling
     * this function, every possible place for a child will be
     * filled by an EmptyElement.
     *
     * The owning screen must be passed to this function so that the
     * screen can be informed about new elements, and so that free
     * id's can be found for the new EmptyElement's.
     */
    virtual void fillChildrenWithEmpties(Screen *s);

    virtual int getNumberOfChildren() const;
    virtual ScreenElement *getChild(int i);
    virtual const ScreenElement *getChild(int i) const;
    virtual void addChild(ScreenElement *child);

    /**
     * Return a pointer to the vector of children of this
     * ScreenElement.  This will return 0 if canHaveChildren() is
     * false.
     */
    virtual const std::vector<ScreenElement *> *getChildren() const;
    virtual std::vector<ScreenElement *> *getChildren();

    /**
     * Return the text that is displayed in this element or
     * QString() when there is none.
     */
    virtual QString text() const;

    virtual bool operator==(const ScreenElement &other) const;
    bool operator!=(const ScreenElement &other) const;
};
}
}

#endif
