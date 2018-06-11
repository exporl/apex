Introduction
============

In order to be able to perform auditory psychophysical and speech
perception experiments a versatile research platform has been developed
at ExpORL, Dept. Neurosciences, KULeuven (Laneau et al., 2005; Francart
et al., 2008). [^1], allows most auditory behavioral experiments to be
performed without any programming, both for acoustic stimulation, direct
electric stimulation via a CI or any combination of devices.

This manual describes . The idea behind is that one should be able to
set up an experiment without any programming knowledge. It is a generic
platform with abstract interfaces to the computer monitor, computer
input devices such as keyboard, mouse, and touch screen, and output
devices such as sound cards or interfaces to cochlear implants. The user
should be able to use any of the interfaces without programming any
device specific details.

Experiments are defined in the <span>XML</span> format[^2], allowing for
a structured experiment definition in a generic format. A Matlab and R
toolbox is distributed together with to ease the automatic generation of
experiment files and analysis of results files.

Registration
------------

After registration can be downloaded from
<http://www.kuleuven.be/exporl/apex> and can be used free of charge. The
hardware requirements are limited to a personal computer running the
Linux or Windows operating system and the necessary stimulation devices.

Citing
------

For proper citation see License Agreement at
<http://www.kuleuven.be/exporl/apex>. Any publication that deals with
should cite: Francart, T., van Wieringen, A., Wouters, J. APEX3: a
Multi-purpose test platform for auditory psychophysical experiments.
Journal of Neuroscience Methods, vol. 172, no. 2, pp. 283–93, 2008.

When using the animated child mode, you should also refer to: Laneau,
J., Boets, B., Moonen, M., van Wieringen, A. and Wouters, J., “A
flexible auditory research platform using acoustic or electric stimuli
for adults and young children”, Journal of Neuroscience Methods, 142,
pp. 131-136, 2005.

Main features of APEX 4
----------------------

-   No programming is required to set up an experiment.

-   Multiple platforms are supported, including MS Windows and Linux.

-   Multiple output devices are supported, including sound cards, an
    interface to cochlear implants from Cochlear and an interface to
    cochlear implants from Advanced Bionics. The supported implant
    devices can be used in any combination, a CI (or hearing aid) in
    both ears (bilateral electrical stimulation) or simultaneous
    stimulation via a CI in one ear and acoustical stimulation in the
    other (bimodal stimulation).

-   Several psychophysical procedures are readily available and custom
    procedures can easily be added (plug-in procedure).

-   As much information as possible is stored per trial in the
    result file. This includes the subject’s response, but also response
    times, calibration values and much more.

-   Visual feedback can be given after each trial.

-   There is a special animated interface for testing (young) children.

-   There is a Matlab toolbox for experiment file creation and advanced
    result file analysis.

-   Custom signal processing filters can be added. (plug-in filter)

-   Custom interfaces to external controllers can be added.
    (plug-in controller)

-   There is a <span>GUI</span> (Graphical User Interface) to
    calibrate parameters.

Getting help - documentation
----------------------------

There are 4 forms of documentation available for :

The paper

:   The paper gives a concise high level overview. It is advisable to
    read it first. Note that since it was published some implementation
    details have changed, so please refer to the other documentation for
    up to date details.

The manual

:   You are currently reading the manual. It does not cover every
    feature of APEX in detail, but endeavours to describe the most often
    used features in more detail than the paper.

The schema documentation

:   The APEX schema defines the structure of an experiment file (see
    section <span>\[</span>sec:Schema’s<span>\]</span>). It
    systematically contains documentation for each element, and
    exhaustively determines which elements can occur. It is the most up
    to date and most complete source of documentation, but requires some
    insight in the general function of APEX, which is provided by the
    paper and manual. The schema documentation can be consulted in 3
    different ways: (1) using the HTML format schema documentation that
    comes with APEX, in folder , (2) when editing an experiment file
    with the oXygen editor, it will show the documentation for the
    current element in a different pane and suggest elements when you
    start typing, or (3) opening the schema directly in the
    oXygen editor.

The examples

:   APEX is shipped with a large number of examples, illustrating most
    of the features. They are stored in folder in the main APEX folder,
    and are documented in
    chapter <span>\[</span>chap:examples<span>\]</span> . The best way
    to start a new experiment, is probably to take one of the examples
    and modify it until it suits your needs.

<!-- Style conventions of the manual -->
<!-- ------------------------------- -->

<!-- In this manual different style conventions are used: -->

<!-- -   example fragment of XML -->

<!-- -   Information specific to the use of the program OxygenXML is -->
<!--     formatted as follows: -->

<!-- -   a different font is used  -->

Basic setup
-----------

is installed by either running the installer (), or by simply copying
the APEX folder to your computer. After installation, there is a main
directory (default: ) under which the following subdirectories exist and
contain the necessary files:

bin

:   Binary files: the main executable (), the Qt dll’s and some Qt
    plugins

schemas

:   The experiment file schema () and the apexconfig () schema. You can
    point your XML editor to the former schema when editing experiment
    files (section <span>\[</span>sec:Schema’s<span>\]</span>).

config

:   The file contains general settings that are applied to
    all experiments.

plugins

:   contains different plugins (cf.
    appendix <span>\[</span>sec:plugins<span>\]</span>).

amt

:   contains the , for automatic generation or analysis of experiment
    files (cf. appendix <span>\[</span>app:Matlab<span>\]</span>).

examples

:   contains example experiments for nearly every feature of

makes use of an experiment file. An experiment file contains all the
necessary information to run an experiment, such as the layout of the
screen, the workings of the procedure, references to stimulus files and
pictures, the way in which the stimuli are routed to a device etc.
Several tools exist to create experiment files easily and analyze
results, e.g. an XML editor (next section) or the .

XML editor: to create or edit experiment files <a name="xmleditor"/>
---------------------------------------------------------------------

While any text editor, such as wordpad, notepad and many others, can be
used to create or edit experiment files, the use of an editor
specifically suited for editing XML files has many advantages, such as
syntax highlighting, automatic completion and validation.

At ExpORL we use the OxygenXML editor and will therefore give hints on
how to use the syntax in this manual. You are, of course, free to use
any other editor.

A free demo version of OxygenXML is available from
<http://oxygenxml.com>. If you would decide to buy the full version, you
will get a 10% discount if you enter the following coupon code during
the ordering process:

> **oXygen-Kuleuven**

A free version is available for people working in the field of life
sciences.

Overview of the user manual
---------------------------

| ------------------- | ------------------------------------------------- |
| Basic concepts      | Discusses the basic concepts of APEX 4.            |
| Creating experiment | Describes the structure of experiment files.      |
| Scripting           | Describes how to automate (script) tasks in APEX 4.|
| Examples            | Discusses 4 example experiments in detail.        |
| Examplestrategies   | Shows how to implement some common features of psychophysical and perceptual experiments.                                                               |
| Results             | Shows result files can be analyzed.               |
| L34                 | Describes the use of the L34 device               |
| Matlab              | the Matlab toolbox                                |
| Plugins             | Use of plugins                                    |
| Customizing         | Customizing the appearance                        |

An exhaustive list of all elements that can occur in an experiment file
are given in the schema documentation.

[^1]: Application for Psychophysical EXperiments

[^2]: The complete XML specification can be found at
    <http://www.w3.org/TR/xml11/>
