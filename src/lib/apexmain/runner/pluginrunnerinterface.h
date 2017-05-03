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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_PLUGINRUNNERINTERFACE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_PLUGINRUNNERINTERFACE_H_

#include <QtPlugin>
#include <QPushButton>

/** @file
 * Plugin Runner interface for APEX.
 */

class PluginRunnerInterface
{
public:
    PluginRunnerInterface(const QString &text) :
        btnText(text),
        btn(text)
    {
    }

    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginRunnerInterface()
    {
    }

    /**
     * Return absolute path of the experiment to be ran
     * Can be a newly generated (written) experiment or an existing one
     * on disk
     */
    virtual QString getFileName() = 0;

    /**
     * Return QPushButton that can be shown on screen to start the
     * pluginrunner
     */
    virtual QPushButton* getButton()
    {
        return &btn;
    }

    /**
     * Return the text that will be show in the button.
     */
    virtual const QString getButtonText()
    {
        return btnText;
    }

private:
    const QString btnText;
    QPushButton btn;
};


/** Plugin Runner creator interface. Each Runner library has to implement the
 * abstract functions in this class. It contains a list of all available plugins
 * in the library and allows the instantiation of plugins.
 *
 * A plugin library must export an instance of a derived class with
 * Q_EXPORT_PLUGIN2:
 *
 * @code
 * class ExampleRunnerCreator : public QObject, public PluginRunnerCreator
 * {
 *     Q_OBJECT
 *     Q_INTERFACES (PluginRunnerCreator)
 * public:
 *     virtual QStringList availableRunners() const;
 *     virtual PluginRunnerInterface *createRunner (
 *              const QString &name ) const;
 * };
 * Q_EXPORT_PLUGIN2 (exampleRunner, ExampleRunnerCreator)
 * @endcode
 *
 * @see \ref plugin for a documented example
 */
class PluginRunnerCreator
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginRunnerCreator()
    {
    }

    /** Returns the available Runners in the library. A Runner can be
     * instantiated with #createRunner().
     *
     * @return list of all available Runner plugins
     */
    virtual QStringList availablePlugins() const = 0;

    /** Creates a Runner. The caller takes ownership of the created instance and
     * must free it after use. If it is not possible to create a Runner
     * instance, returns @c NULL.
     *
     * @param name name of Runner as returned by #availableRunners()
     * @return new Runner instance or @c NULL if it was not possible to create
     * the Runner
     */
    virtual PluginRunnerInterface *createRunner(const QString &name) const = 0;
};

Q_DECLARE_INTERFACE(PluginRunnerCreator, "be.exporl.apex.pluginRunner/1.0")

#endif
