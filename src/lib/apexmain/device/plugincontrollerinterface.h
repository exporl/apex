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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_PLUGINCONTROLLERINTERFACE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_DEVICE_PLUGINCONTROLLERINTERFACE_H_

#include <QtPlugin>

/** @file
 * Plugin Controller interface for APEX.
 */

/** Plugin Controller interface for APEX. Each Controller has to implement the
 * abstract
 * functions in this class.
 *
 * For each Controller plugin, the methods are called similar to this example:
 * @code
 * QString apexMainLoop (PluginControllerInterface *plugin)
 * {
 *     while (trialsLeft()) {
 *         plugin->resetParameters();
 *         if (!plugin->isValidParameter ("fadein", -1))
 *             return plugin->errorMessage();
 *         plugin->setParameter ("fadein", -1, "30");
 *
 *         plugin->prepare ();
 *
 *         // Show screen...
 *         // Wait for user to press a button...
 *     }
 *     return "Experiment successful";
 * }
 * @endcode
 *
 */
class PluginControllerInterface
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginControllerInterface()
    {
    }

    /** Resets all parameters to the default values. This method is called
         * before the parameters are set to specific values with
     * #setParameter().
         * You should call this method in your constructor to be sure that all
         * parameters have sensible values.
     */
    virtual void resetParameters() = 0;

    /** Checks whether the passed Controller parameter is valid for the given
         * channel. A channel of @c -1 denotes a global parameter that applies
     * to
         * all channels. Returns @c true if the parameter is valid.
         *
         * If the method returns @c false it should set an appropriate error
     * message
         * explaining the reason with #setErrorMessage().
         *
         * @param name name of the parameter to be set
         * @param channel channel index or @c -1 if the parameter should apply
     * to
         * all channels
         * @return @c true if the given parameter is valid for the given
     * channel, @c
         * false otherwise
     */
    virtual bool isValidParameter(const QString &name, int channel) const = 0;

    /** Sets a Controller parameter for the given channel. If the passed
     * parameter
         * value can not be converted to the right type, returns @c false.
         *
         * If the method returns @c false it should set an appropriate error
     * message
         * explaining the reason with #setErrorMessage().
         *
         * @param name name of the parameter to be set
         * @param channel channel index or @c -1 if the parameter should apply
     * to
         * all channels
         * @param value value to which the parameter should be set
         * @return @c true if the parameter exists and the value was valid for
     * the
         * given parameter, @c false otherwise
     */
    virtual bool setParameter(const QString &name, int channel,
                              const QString &value) = 0;

    /** Prepares the plugin for processing. No parameters will be changed after
         * this method is called. This method is mostly used to do long-lasting
         * initializations that should not be done in #process() such as the
         * initilization of Controllers or the reading of  external files. The
     * passed
         * total number of frames may be smaller than the number of frames
     * processed
         * with #process().
         *
         * If the method returns @c false it should set an appropriate error
     * message
         * explaining the reason with #setErrorMessage().
         *
         * @param numberOfFrames total number of frames to expect in #process()
         * @return @c true if the plugin is ready for processing and all
     * parameters
         * are valid, @c false otherwise
     */
    virtual bool prepare() = 0;

    virtual void release(){};

    /** Lets the plugin play a stimulus if it is able to generate one.
         * Is called by PluginController's slot syncControlDeviceOutput()
         * Which is connected to OutputDevice's signal stimulusStarted()
         * Connection is made in experimentcontrol.cpp
     */
    virtual void playStimulus()
    {
    }

    /** Returns the last error message. The error message explaines why a method
         * such as #isValidParameter(), #setParameter() or #prepare() returned
     * @c
         * false.
     */
    QString errorMessage() const
    {
        return error;
    }

protected:
    /** Sets an error message. This method should be called with an appropriate
     * error message in a derived class if a method such as #isValidParameter(),
     * #setParameter() or #prepare() returns @c false.
     *
     * @param error error message explaining why a method such as
     * #isValidParameter(), #setParameter() or #prepare() returned @c false.
     */
    void setErrorMessage(const QString &error) const
    {
        this->error = error;
    }

private:
    // #setErrorMessage() is const so that even read-only methods can set an
    // error message
    mutable QString error;
};

/** Plugin Controller creator interface. Each Controller library has to
 * implement the
 * abstract functions in this class. It contains a list of all available plugins
 * in the library and allows the instantiation of plugins.
 *
 * A plugin library must export an instance of a derived class with
 * Q_EXPORT_PLUGIN2:
 *
 * @code
 * class ExampleControllerCreator : public QObject, public
 * PluginControllerCreator
 * {
 *     Q_OBJECT
 *     Q_INTERFACES (PluginControllerCreator)
 * public:
 *     virtual QStringList availablePlugins() const;
 *     virtual PluginControllerInterface *createController (const QString &name,
 *             unsigned channels, unsigned blocksize, unsigned fs) const;
 * };
 * Q_EXPORT_PLUGIN2 (exampleController, ExampleControllerCreator)
 * @endcode
 *
 * @see \ref plugin for a documented example
 */
class PluginControllerCreator
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginControllerCreator()
    {
    }

    /** Returns the available Controllers in the library. A Controller can be
         * instantiated with #createController().
         *
         * @return list of all available Controller plugins
     */
    virtual QStringList availablePlugins() const = 0;

    /** Creates a Controller. The caller takes ownership of the created instance
     * and
         * must free it after use. If it is not possible to create a Controller
         * instance, returns @c NULL.
         *
         * @param name name of Controller as returned by #availablePlugins()
         * @return new Controller instance or @c NULL if it was not possible to
     * create
         * the Controller
     */
    virtual PluginControllerInterface *
    createController(const QString &name) const = 0;
};

Q_DECLARE_INTERFACE(PluginControllerCreator,
                    "be.exporl.apex.pluginController/1.0")

#endif
