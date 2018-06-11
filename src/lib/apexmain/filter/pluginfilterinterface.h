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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_FILTER_PLUGINFILTERINTERFACE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_FILTER_PLUGINFILTERINTERFACE_H_

#include <QtPlugin>

/** @file
 * Plugin filter interface for APEX.
 */

/** Plugin filter interface for APEX. Each filter has to implement the abstract
 * functions in this class.
 *
 * For each filter plugin, the methods are called similar to this example:
 * @code
 * QString apexMainLoop (PluginFilterInterface *plugin)
 * {
 *     while (trialsLeft()) {
 *         plugin->resetParameters();
 *         if (!plugin->isValidParameter ("fadein", -1))
 *             return plugin->errorMessage();
 *         plugin->setParameter ("fadein", -1, "30");
 *
 *         plugin->prepare (numberOfFrames());
 *
 *         // Show screen...
 *
 *         while (block = getNextDataBlock())
 *             plugin->process (block);
 *
 *         // Wait for user to press a button...
 *     }
 *     return "Experiment successful";
 * }
 * @endcode
 *
 * @see \ref plugin for a documented example
 */
class PluginFilterInterface
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginFilterInterface()
    {
    }

    /** Resets all parameters to the default values. This method is called
     * before the parameters are set to specific values with #setParameter().
     * You should call this method in your constructor to be sure that all
     * parameters have sensible values.
     */
    virtual void resetParameters() = 0;

    /** Checks whether the passed filter parameter is valid for the given
     * channel. A channel of @c -1 denotes a global parameter that applies to
     * all channels. Returns @c true if the parameter is valid.
     *
     * If the method returns @c false it should set an appropriate error message
     * explaining the reason with #setErrorMessage().
     *
     * @param name name of the parameter to be set
     * @param channel channel index or @c -1 if the parameter should apply to
     * all channels
     * @return @c true if the given parameter is valid for the given channel, @c
     * false otherwise
     */
    virtual bool isValidParameter(const QString &name, int channel) const = 0;

    /** Sets a filter parameter for the given channel. If the passed parameter
     * value can not be converted to the right type, returns @c false.
     *
     * If the method returns @c false it should set an appropriate error message
     * explaining the reason with #setErrorMessage().
     *
     * @param name name of the parameter to be set
     * @param channel channel index or @c -1 if the parameter should apply to
     * all channels
     * @param value value to which the parameter should be set
     * @return @c true if the parameter exists and the value was valid for the
     * given parameter, @c false otherwise
     */
    virtual bool setParameter(const QString &name, int channel,
                              const QString &value) = 0;

    /** Prepares the plugin for processing. No parameters will be changed after
     * this method is called. This method is mostly used to do long-lasting
     * initializations that should not be done in #process() such as the
     * initilization of filters or the reading of  external files. The passed
     * total number of frames may be smaller than the number of frames processed
     * with #process().
     *
     * If the method returns @c false it should set an appropriate error message
     * explaining the reason with #setErrorMessage().
     *
     * @param numberOfFrames total number of frames to expect in #process()
     * @return @c true if the plugin is ready for processing and all parameters
     * are valid, @c false otherwise
     */
    virtual bool prepare(unsigned numberOfFrames) = 0;

    /** Processes a block of sample frames.
     *
     * @param data non-interleaved sample data
     */
    virtual void process(double *const *data) = 0;

    /** Returns the last error message. The error message explaines why a method
     * such as #isValidParameter(), #setParameter() or #prepare() returned @c
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

/** Plugin filter creator interface. Each filter library has to implement the
 * abstract functions in this class. It contains a list of all available plugins
 * in the library and allows the instantiation of plugins.
 *
 * A plugin library must export an instance of a derived class with
 * Q_EXPORT_PLUGIN2:
 *
 * @code
 * class ExampleFilterCreator : public QObject, public PluginFilterCreator
 * {
 *     Q_OBJECT
 *     Q_INTERFACES (PluginFilterCreator)
 * public:
 *     virtual QStringList availablePlugins() const;
 *     virtual PluginFilterInterface *createFilter (const QString &name,
 *             unsigned channels, unsigned blocksize, unsigned fs) const;
 * };
 * Q_EXPORT_PLUGIN2 (examplefilter, ExampleFilterCreator)
 * @endcode
 *
 * @see \ref plugin for a documented example
 */
class PluginFilterCreator
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~PluginFilterCreator()
    {
    }

    /** Returns the available filters in the library. A filter can be
     * instantiated with #createFilter().
     *
     * @return list of all available filter plugins
     */
    virtual QStringList availablePlugins() const = 0;

    /** Creates a filter. The caller takes ownership of the created instance and
     * must free it after use. If it is not possible to create a filter
     * instance, returns @c NULL.
     *
     * @param name name of filter as returned by #availablePlugins()
     * @param channels number of channels for the plugin
     * @param blocksize size of one block to be processed in frames
     * @param sampleRate sample rate in Hz
     * @return new filter instance or @c NULL if it was not possible to create
     * the filter
     */
    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blocksize,
                                                unsigned sampleRate) const = 0;
};

Q_DECLARE_INTERFACE(PluginFilterCreator, "be.exporl.apex.pluginfilter/1.0")

#endif
