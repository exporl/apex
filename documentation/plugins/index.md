Plugins
=======

Introduction
------------

Apex supports custom audio filter plugins. In this way, Apex can easily
be extended without modifying Apex itself. A plugin is essentially an
object that provides a processing function that accepts a block of
samples and returns a processed block of samples.

We will illustrate the process of creating a plugin by implementing an
amplifier plugin. Note that Apex already contains an amplifier filter,
making this example less useful in practice.

Next to the aforementioned processing function, a filter can also
contain parameters. Parameter values can be specified in the experiment
xml file or set by another apex module.

Solving problems
----------------

using the plugin dialog

Example: amplifier
------------------

### The experiment file

Our example amplifier looks as follows in the experiment xml file:

```xml
    <filter id="myAmplifier" xsi:type="apex:pluginfilter">
        <device>wavdevice</device>
        <channels>1</channels>
        <continuous>false</continuous>
        <plugin>amplifierplugin</plugin>
        <parameter id="gain" name="gain">-10</parameter>
    </filter>
```

plugin

:   Is the name of the binary plugin file you provide. If no extention
    is specified, Apex will guess one based on the current operating
    system.[^1]

Parameter

:   is defined for each parameter that the plugin accepts. On
    initialisation the plugin will be queried for each parameter whether
    it is valid. The `id` attribute is the parameter id to be referenced
    from elsewhere in the experiment file. The name is what the plugin
    uses to determine which internal parameter to modify.

### Build environment

To successfully build a plugin, you need the following things setup
properly in your build environment:

-   A modern compiler, preferably the same as used for compiling
    Apex[^2]

-   The Qt libraries and build tools
    <http://trolltech.com/developer/downloads/qt/index>

-   The file pluginfilterinterface.h

-   Optional: the amplifier plugin example

### Creating the plugin

The main steps for creating and using a plugin are:

-   Create a header file (.h) for your plugin, defining classes
    inheriting from PluginFilterInterface and PluginFilterCreator.
    Include pluginfilterinterface.h in this header.

-   Create a source files (.cpp) implementing all methods for
    your plugin.

-   Create a .pro file for use with qmake to compile your plugin

-   Compile your plugin using qmake and make

-   Copy your plugin library to the Apex plugins directory

-   Create an apex experiment file that refers to your plugin library

-   Run Apex with the experiment file

The actual plugin is created by using the file `pluginfilterinterface.h`
and inheriting from the PluginFilterInterface and PluginFilterCreator
objects.

The only purpose of the PluginFilterCreator object is to return an
instance of your filter. In this way, it is possible to have multiple
filters in one library. This is currently not implemented in Apex but
only provided in the interface.

The easiest way to implement your own filter, is to start from the
amplifier example. We suggest that you use qmake to process a .pro file
as follows:

```make
    TEMPLATE = lib

    TARGET = amplifierplugin
    CONFIG += plugin
    CONFIG += debug

    QT -= gui

    INCLUDEPATH += /path/to/pluginfilterinterface.h

    SOURCES += amplifierplugin.cpp
    HEADERS += amplifierplugin.h
```

The most important part about the plugin .h file itself is to inherit
from QObject and include the necessary macro’s.

```c++
     class AmplifierPluginCreator : public QObject, public PluginFilterCreator
    {
        Q_OBJECT
        Q_INTERFACES (PluginFilterCreator)

        ...
```

In the .cpp file you should include the following:

```c++
    Q_EXPORT_PLUGIN2(amplifierplugin, AmplifierPluginCreator)
```

Then run qmake and make. Make sure that Apex can find your plugin
library by putting it in the Apex plugins directory or by providing an
absolute or relative path in the experiment xml file.

Note that Apex might crash if your plugin crashes (i.e., overwrites
memory it should not, etc.).

If you want to know more about the general plugin mechanism, consult the
Qt documentation on QPluginLoader and the Qt Plugin Howto (“The
Lower-Level API: Extending Qt Applications”).

[^1]: On Linux, Apex tries to add the extention `.so` and the prefix
    `lib`. On Windows, Apex tries to add the extention `.dll`.

[^2]: On Linux we currently use gcc 4.1, on Windows we use Visual studio
    2003.
