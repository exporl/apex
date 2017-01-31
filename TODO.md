* Figure out what is going on with programs/writer and src/tests/apexwriters/
* Check TODOs in the code and categorize in easy/hard, add relevant ones to todo list
* FIX all compiler warnings

* Fix the messy flash support (flashwidget), if necessary move into plugin
* Check license headers
* Fix include guards
* check all #includes for weird styles such as qwidget.h
* spintest links against spin, but spin is a plugin and no library so this is
  totally weird -> make spin a library, give it its own subdir hierarchy?
* move all data directories in data/:
    * matlab
    * doc
    * manual
    * examples
* cleanup build scripts in build/
* fix writers test crashing the testbench?
* commit tag should come from qmake, not build script
* fix windows debug build, or just don't build it

* Update dependencies
** Mingw op windows (ASIO? - seems to work in BERTHA)
** Make Jack dependency optional
** 24bit sound support on windows without ASIO (upgrade portaudio?)

* Change BFT to use procedure-refactory as reference instead of master

* Fix build system for manual (pdflatex)

* Bertha backend
** Validate (unit tests)
** Remove streamapp

* L34 backend uit RBA

* Real test / demo mode, with compulsory subject ID, automatic upload of results file to server

* GUI
** Add sound card selection GUI (after bertha refactory)

* Connections viewer (using dot)
** Automatically find dot.exe in windows
** Ship dot with APEX?
** Add mouse zooming functionality

* Add functionality to automatically save results to intelligently generated filename (specs Jonas?)

* SPIN
** in stilte niveau ipv SNR op screen tonen
** results filename template
** experiment file automatisch opslaan
** GUI in wizard formaat
** add Matrix experiment/procedure
** add custom plugin procedure
** Refactory: simplify generated experiment format
*** move generation of XML datablocks, stimuli, connections to javascript
*** spin interface should only generate a minimal javascript call along the lines
  of `useSpeechMaterial({ material: 'material', list: 'list', ...})`
*** in a second iteration, allow the specification of these parameters in
  interactive instead of the SPIN gui

* Results analysis
** refactor javascript, make easier to use
** documentation
** make more beautiful (CSS)
** Javascript resuls: use highcharts? http://www.highcharts.com/
** upgrade dependencies (jquery, ...)
** Add R/Shiny support?

* Change schema version numbering to simple integer, so there is no confusion with APEX version number

* Refactor parameter manager

* Screens in HTML/javascript

* Localisation experiment plugin (like SPIN)

* Documentation
** Upgrade to 3.1.2
** Document results handling
** Document plugins (procedure, javascript document generation)

* Clean up examples folder
** Make sure each example works
** Demonstrate a single feature in each example
** Add real-world examples: localisation, ...

* Upgrade SLM interface to new B&K API (using external C# program?)

* Add "runner" that can run multiple experments one after the other (like lists in RBA). Think about using parameters from the previous experiment to setup the next (provide javascript API).

* Calibration
** Store calbration history
** Store calibrated parameter values in results file

* Examples
** Add real-world examples
** Clean up: remove obsolete ones
** Add documentation
** Upgrade to latest schema version

- Reaction times
** Validate current implementation and document variability
** Select and add support for response box
*** http://forum.arduino.cc/index.php?topic=128904.0
*** http://www.ncbi.nlm.nih.gov/pubmed/23585023
*** https://reactiontimes.wordpress.com/arduinort/
*** http://www.lfe.mw.tum.de/en/open-source/arduino-drt/

* APEX matlab toolbox
** Clean up, refactor function names
** Add some automated unit tests
** Document
** Upgrade to 3.1

- Merge script contexts (exchange parameters between various javascript plugins)

- The writers test fails on simple.apx, extrasimple.apx, continuous-waitbeforefirst.xml, train1.xml, train2.xml and
trial-nostimulus.xml from examples/procedures from master. The reason for this failure is that in the constructor of the
class ScreensData the boolean showProgress is set to true. So even if there isn't a reinforcement block in the xml, the
function hasReinforcement() from ScreensData will return true. Thus when the xml is written, a reinforcement block will
be inserted which will result in two different instances of ScreensData and the test will fail. A solution is to create
a reinforcement block in the original xml. This solution is presented in data/tests/libwriters/simple.apx.
