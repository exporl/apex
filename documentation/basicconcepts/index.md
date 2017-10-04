Basic concepts
==============

Introduction
------------

An Apex3 experiment is defined in an experiment file. Experiment files are
defined in the XML format see [xml](#xml), and it is advised,
but not compulsory, to use OxygenXML to edit the XML format see [introduction](../introduction#xmleditor).

While experiment files can have any filename and any extension, it is
advised to give your experiment file the extension , such that is
immediately associated with Apex3. Also, there is a clear distinction between
*experiment* files and *result* files, which will receive the extension *apr*.

Terminology
-----------

Apex3 is based on a few basic concepts that are common to all psychophysical
experiments. Here we define the concepts device, controller, datablock,
stimulus, filter, screen, response, trial, procedure, experiment file,
result, ID, parameter, and standard. How to implement these concepts in
an experiment file is explained in [Creating experiment files](../creatingexperimentfiles).

device

:   is a system connected to the computer that can be controlled by Apex3.
    Devices can send signals to a transducer. Examples are sound cards
    and interfaces to cochlear implants (CIs). Devices can have
    parameters that are controlled by Apex3.

controller

:   is a system connected to the computer that -unlike a filter- does
    not accept/transfer signals (e.g., gain), but has parameters that
    are controlled by Apex3. An example is a programmable attenuator that
    controls the gain of an amplifier.

datablock

:   is an abstraction of a basic block of data that can be processed by
    Apex3 and can be sent to a matching device. In the case of a sound card,
    the datablock would be an audio signal in the form of a series of
    samples that is commonly stored on disk as a “wave file”.

stimulus

:   is a unit of stimulation that can be presented to the subject, to
    which the subject has to respond. In the simplest case it consists
    of a single datablock that can be sent to a single device. More generally it can
    consist of any combination of datablocks that can be sent to any number of devices,
    simultaneously or sequentially.

filter

:   is a data processor that runs inside Apex3 and that accepts a block of
    data, e.g., a certain number of samples from an audio file, and
    returns a processed version of the block of data. An example is an
    amplifier that multiplies each sample of the given block of data by
    a certain value.

screen

:   is a GUI (Graphical User Interface) that is used by the subject to
    respond to the stimulus that was presented.

response

:   is the response of the test subject. It could, for example, be the
    button that was clicked or the text that was entered via the screen.

trial

:   is a combination of a screen that is shown to the subject, a
    stimulus that is presented via devices and a response of
    the subject.

procedure

:   The procedure controls the flow of an experiment. It decides on the
    next screen to be shown and the next stimulus to be presented.
    Generally, a procedure will make use of a list of predefined trials.

experiment file

:   a combination of procedures and the definitions of all objects that
    are necessary to conduct those procedures.

result

:   is associated with an experiment and contains information on every
    trial that occurred.

ID

:   is a name given to an object defined in an experiment. It is unique
    for an experiment. If, for example, a device is defined, it is given
    an ID by which it can be referred to elsewhere in the experiment.

parameter

:   is a property of an object (e.g. a device or filter) that is given
    an ID. A filter that amplifies a signal could for example have a
    parameter with ID *gain* that is the gain of the amplifier in dB.
    The value of a parameter can be either a number or text.

standard

:   In a multiple alternatives forced choice procedure the reference
    signal is defined as the standard. Internally, Apex3 does not
    differentiate between a stimulus and a standard. See example 2: the
    standard is defined under stimulus and should be referred to by its ID.

XML <a name="xml"/>
--------------------

### Basic XML: elements, tags, attributes

Advantages of the XML format are that it is human readable, i.e., it can
be viewed and interpreted using any text editor, and that it can easily
be parsed by existing and freely available parsers. Moreover, many tools
exist for editing, transforming or otherwise processing XML files. In
addition, a good XML editor can provide suggestions and documentation
while constructing an experiment file. An XML file consists of a series
of *elements*. Elements are started by their name surrounded by and ,
the begin tag, and ended by their name surrounded by and , the end tag.

```xml
        <b>1</b>
```

Every element can have content. There are two types of content: *simple*
content, for example a string or a number, and *complex* content: other
elements. An element can also have attributes: extra properties of the
element that can be set. In the following example, element is started on
line 1 and ended on line \[xml:end\]. Element contains *complex*
content: the elements and . Element contains *simple* content: the
numerical value 1. Element again contains *complex* content: the
elements and . Element has an attribute named attrib1 with value 15.
Element on line \[xml:empty\] shows the special syntax for specifying an
empty element. This is equivalent to . The reference manual lists all
possible elements and attributes.

```xml
    <a>
        <b>1</b>
        <c>
            <c1 attrib1="15"> </c1>
            <c2/>
        </c>
    </a>
```

The begin tag and end tag are case sensitive.

A comment is inserted as follows:

```xml
     <!--
     This is a comment.
      -->
```

A document that complies with the XML specifications is “well-formed”. A
document is checked for well-formedness before any further processing is
done.

### Schemas

A schema describes the structure of the document and specifies where
each element is allowed to occur. In addition it contains exhaustive
documentation. The Apex3 schema’s are located under `/schemas`
in the main Apex3 directory.

A document is called *valid* when it adheres to the associated schema
and if the document complies with the constraints expressed in it [^1].
If the document is valid the experiment file should run directly after
having opened it with Apex3 (although errors that are specific to Apex3 can still
occur). Apex3 will generate warnings and errors if the experiment file is not
valid [^2].

For autocompletion and display of documentation it is
necessary to work in OxygenXML. You need to associate the
experiment file with the main schema, i.e. experiment.xsd. Click
on `associate schema` in Oxygen (pushpin icon) and browse for
experiment.xsd, which is usually located under the main Apex3 directory [^3].
Click on OK. Once an XML-file is associated with the schema, you can check whether your document
is well-formed (menu bar: red `v` icon, bottom row, left) and valid
(menu bar: blue `v` icon, bottom row, second from left).


Try the following:

-   open the experiment file using Apex3. Run the experiment if you wish.

-   open with OxygenXML

-   associate the Apex3 schema

-   check validity

-   make the file NOT well-formed, e.g. by removing an end tag. Check
    validity again.

-   fix it again

-   make the file invalid, e.g. by changing the name of an element

-   run with Apex3. Read error messages.

-   undo the errors

-   check well-formedness

-   check validity

-   Start typing &lt; somewhere in the experiment file, and observe how
    OxygenXML suggests the different options

Shortcuts
---------

The following shortcuts are available:

F4

:   Stop experiment

F5

:   Start experiment

F6

:   Pause experiment

F7

:   Skip intro/outro movie

F9

:   Repeat last trial

Additional shortcuts for screen elements can be defined in the
experiment file.

[^1]: a document can only be valid if it is well-formed

[^2]: by means of message window

[^3]:  Apex3 schema can be found in the Apex3 directory structure under `schemas/experiment.xsd`
