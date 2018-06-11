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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_FEEDBACK_PLUGINFEEDBACKINTERFACE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_FEEDBACK_PLUGINFEEDBACKINTERFACE_H_

#include <QList>
#include <QPair>
#include <QtPlugin>

/** @file
 * Plugin feedback interface for APEX.
 */

typedef QList<QPair<QString, QString>> FeedbackPluginParameters;

class PluginFeedbackInterface
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginFeedbackInterface()
    {
    }

    //! reset status
    // virtual void reset() =0;

    virtual void highLight(const QString &element) = 0;

    //! Indicate that the response was correct (thumbs up)
    virtual void showPositive() = 0;

    //! Indicate that the response was false (thumbs down)
    virtual void showNegative() = 0;

    //! Remove all feedback (highlight & positive/negative)
    virtual void clear() = 0;

    //! Get error string (error condition if non-empty)
    virtual QString errorString() = 0;
};

/** Plugin creator interface. Each plugin library has to implement the
 * abstract functions in this class. It contains a list of all available plugins
 * in the library and allows the instantiation of plugins.
 *
 * A plugin library must export an instance of a derived class with
 * Q_EXPORT_PLUGIN2:
 *
 * @see \ref plugin for a documented example
 */
class PluginFeedbackCreator
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginFeedbackCreator()
    {
    }

    /** Returns the available feedback plugins in the library. A feedback plugin
     * can be instantiated with #createFeedbackPlugin().
     *
     * @return list of all available plugins
     */
    virtual QStringList availablePlugins() const = 0;

    /** The caller takes ownership of the created instance and
     * must free it after use. If it is not possible to create a plugin
     * instance, returns @c NULL.
     *
     * @param name name of feedback plugin as returned by #availablePlugins()
     */
    virtual PluginFeedbackInterface *
    createPluginFeedback(const QString &name,
                         const FeedbackPluginParameters &params) const = 0;
};

Q_DECLARE_INTERFACE(PluginFeedbackCreator, "be.exporl.apex.pluginfeedback/1.0")

#endif
