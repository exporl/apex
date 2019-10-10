Displaying and analysing results
================================
Attachment: [Slides of Resultsviewer workshop (25/04/2018)](2018-04-25_ResultsviewerInfo_slideshow.pdf)

The results XML file
--------------------

After completion of the experiment a file containing results is always
given. The default results file is in the XML format and it contains all
the information about the course of the completed experiment. In the
following sections, we see how to display the results on the screen in a
user-friendly format and how to import them into other software for
further analysis.

automatically assigns a default name to the results file, namely it
appends “-apr” to the name of the experiment file (e.g.
closedsetword-apr). It will never overwrite an existing results file,
but will append a number to results (e.g. closedsetword-apr-1) in the
case of an existing results file.

The results are stored in the element

```xml
    <apex:results>

    </apex:results>
```

The XML file contains information, such as

-   the testing date

-   the testing duration

-   the name of the XSLT script file see [Using XSLT transforms](#using xslt transforms)

-   information on the procedure (eg: the adaptive parameter)

In addition, for each completed presented to the subject it includes

-   details of the procedure (stimulus that was presented and the values
    of the variable parameters in the specific trial).

-   the response that was chosen by the subject

-   the outcome of the corrector

-   possible errors of the output system/device (eg. underruns)

-   the response time (time between the moment the buttons are enabled
    and the moment an answer is given)

-   in case a random generator was used: the value of the random
    generator in the specific trial

Remark: with an adaptive procedure the number of is given

Displaying results
------------------

APEX has infrastructure to show experiment results on the screen,
directly after the experiment, or by opening the results file afterwards
with APEX. The system is based on JavaScript and HTML, which makes it
easy to modify for the end user.

HTML or HyperText Markup Language is the standard markup language for
creating web pages. It is used to define the structure of a page with
content. By using JavaScript, the content or its structure can be
modified dynamically and interactively. Visual layout is achieved by
using Cascading Style Sheets (CSS).

In the element of the experiment file, you can define the HTML file to
be used to display results, The default is (apex:resultsviewer.html).
e.g.,

```xml
      <results>
          <page>apex:resultsviewer.html</page>
          <showduringexperiment>true</showduringexperiment>
          <showafterexperiment>true</showafterexperiment>
      </results>
```

can either refer to a file in the same folder as the experiment file,
e.g., , to a folder somewhere on disk, e.g., , or to a file in the APEX
resultsviewer folder, e.g., .

If is , the resultsviewer will be shown while the experiment is running,
and will be updated after each trial. If is , it will be shown after the
experiment has finished.

### Overview of Resultsviewer files

If you want to make small changes to the way results are shown, one way
to start is to copy to the same folder where your experiment file is
stored, rename it something sensible, change the reference in your
experiment file, and then modify the HTML file and JavaScript according
to the desired result.

All dynamic behavior is done by using JavaScript. We use JQuery 3.1.1
and a custom build of Highcharts.

```xml
    <script src="external/jquery-3.1.1.min.js"></script>
    <script src="external/highcharts-custom.js"></script>
```

And all the resultsviewer code can be found in these files:

```xml
    <script src="resultsviewer.js" type="text/javascript"> </script>
    <script src="rv-internals.js" type="text/javascript"> </script>
    <script src="resultsviewer-config.js" type="text/javascript"> </script>
```

The following div is used for interface elements:

```xml
    <div id="chartbuttons"></div>
```

And these divs are containers for visualizations:

```xml
    <div id="customgraphcontainer"></div>
    <div id="linecontainer"></div>
    <div id="matrixcontainer"></div>
    <div id="polarcontainer"></div>
    <div id="textcontainer"></div>
```

### The internals - APEX

If you want to change more than the basic screen layout, you need to
change or add some JavaScript (JS) code. In what follows, the internals
will be explained. First the APEX side will be explained: how the
results viewer gets the actual data from APEX.

When results are to be viewed (depending on and ), APEX will load the
results HTML file in a basic web browser (called QWebView, based on
WebKit). Then, every time a trial is finished, it will call a JavaScript
function , with as argument a string containing the XML that would
normally be written to the results file. For example, after a trial, the
following JavaScript code could be executed:

```javascript
        newAnswer("<trial id=\"trial1\">\n<procedure type=\"apex:adaptiveProcedure\">\n<answer>down</answer>\n<correct_answer>up</correct_answer>\n<stimulus>stimulus1</stimulus>\n<correct>false</correct>\n\t<parameter>0</parameter>\n\t<stepsize>2</stepsize>\n\t<reversals>2</reversals>\n\t<saturation>false</saturation>\n\t<presentations>4</presentations>\n</procedure><screenresult>\n\t<element id=\"buttongroup1\">down</element>\n\t<element id=\"down\"></element>\n</screenresult>\n<output>\n<device id=\"wavdevice\">\n  <buffer underruns=\"0\"/>\n</device>\n</output>\n<responsetime unit=\'ms\'>135</responsetime>\n<randomgenerators>\n\n</randomgenerators>\n<trial_start_time>2015-10-22T11:28:15</trial_start_time>\n</trial>\n\n");
```

What happens next, fully depends on the HTML/JavaScript code.

Whenever new results should be displayed, APEX will call the function in
JavaScript. Note that before each plot any number of calls to newAnswer
can occur. So newAnswer functions as a funnel for the resultsdata,
preparing it for the plot() function.

### The internals - 

This file is the container that holds the structure of the
data-visualization and the references to the JavaScript code. The only
’local’ JS code in the tags is a reference to the function
interfaceButtons(), used to add interactive behavior to the
resultsviewer in the form of simple HTML buttons or checkboxes.

### The internals - - config object

The config file contains three ways for adjusting the way the
resultsviewer works and looks. The first is the config object. It’s a
JavaScript object, which means it can be manipulated in several ways.
The one used here employs curly brackets, colons, commas and semicolons
to define the properties of the object. Learn more about JS object
notation here:
<https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Working_with_Objects>

```javascript
    var config = {
        
        /* Global options
         *  answers:            full list of possible correct answers (stimuli), 
         *                      eg: ["button-90","button-45","button0","button45","button90"]
         *  correctanswers:     full list of possible answers (responses)
         *  removefromanswer:   list of parts that can be pruned from answerlabels, 
         *                      eg: ["button"] -> "button_chair" becomes "chair"
         *  multiprocedure:     <true/false> if true, will plot each different procedure separately (detected automatically if left undefined)
         *  groupproceduresby:  <string> decides which key to group the procedures by e.g.: "proceduretype"(default) or "procedureid"
         */
            global: 
            {
                answers: [<array of strings>],
                correctanswers: [<array of strings>],
                removefromanswer: [<array of strings>],
                multiprocedure: <true/false>,
                groupproceduresby: <string>
            },
        
        
        /* Line graph options
         *  show:               force line graph to show (true) or hide (false)
         *  parametername:      shows parameter name on graph - e.g. "Gain"
         *  parameterunit:      unit for parameter values - e.g. "dB SPL"
         *  parameterkey:       the Trial object key to use for parameter plotting 
         *                      (default: parametervalue) - e.g. "responsetime"
         *  trialsformean:      <int> the amount of trials used for mean (default 6)
         *  reversalsformean:   <int> the amount of reversals used for mean (default 6)
         */
            line:
            {
                show: <true/false>,
                parametername: <string>,
                parameterunit: <string>,
                parameterkey: <string>,
                trialsformean: <int>,
                reversalsformean: <int>
            },
        
        
        /* Confusion matrix options
         *  show:               force matrix to show (true) or hide (false)
         *  removefrommatrix:   list of answers and stimuli that may be filtered out of matrix plotting
         *                      e.g.: ["up","down"]
         */
            matrix:
            {
                show: <true/false>,
                removefrommatrix: [<array of strings>]
            },
        
        
        /* Polar plot options
         *  show:               force polar plot to show (true) or hide (false)
         *  mindegree:          smallest degree of localization-arc
         *  maxdegree:          largest degree of ..
         */
            polar:
            {
                show: <true/false>,
                mindegree: <integer>,
                maxdegree: <integer>
            }
            
        /* Text printing options (applies to result data text)
         *  show:               force text to show (true) or hide (false)
         *  digitsafterdecimal: how many digits to show after decimal point (default 4)
         *  textstyle:          takes a css style object, applies it to ALL untabulated text
         *  tablestyle          same, for tabulated text
         *                      eg: {"font-size" : "120%", "font-weight" : "bold", 
         *                           "color" : "white", "background-color":"black"}
         *                      doc: http://api.jquery.com/css/#css-properties
         */
            text:
            {
                show: <true/false>,
                digitsafterdecimal: <int>,
                textstyle: {<css object>},
                tablestyle: {<css object>}
            },
    };
```

An easy way of making these options experiment-specific is by using the
resultsparameters. By refering to the option name in the parametername,
you can set the value. Like so:

```xml
<resultparameters>
    <parameter name="line_reversalsformean">4</parameter><!-- see manual for full list of options -->
    <parameter name="line_show">true</parameter>
    <parameter name="line_parametername">Gain</parameter>
    <parameter name="customchartvalue">7</parameter>
</resultparameters>
```

Or through the resultscript.

```xml
    <resultscript>
          console.log(params.customchartvalue); <!-- prints "7" -->
          
          test = "This variable is set through results/resultscript.";
          
          config.line.parameterunit = "dB SPL"; <!-- also possible -->
          
          config = { line:{ parametername: "Gain"; },
                     matrix:{ show: false; } 
                    };
        </resultscript>
```

### The internals - - hooks

Hooks are functions that can be implemented for custom functionality.
The following functions can be used:

```javascript
    /* mapAnswer
     *  Receives a string as parameter and returns a trial object
     *  Can be used for cases where a custom result XML structure is defined
     *  Replaces: defaultMapAnswer
     */
        /*
        function mapAnswer(xmlstring)
        {
            //  Parse xml string into DOM object structure
            //  see https://en.wikipedia.org/wiki/Document_Object_Model
            var xml = new DOMParser().parseFromString(xmlstring, "application/xml");
            
            var trialObj = {
                answer = $(xml).find("answer").text(),
                correct_answer = $(xml).find("correct_answer").text(),
                correct = parseBool( $(xml).find("correct_answer").text() )
            }
            
            return trialObj;
        }
        */

    /* resultsFilter
     *  Receives a trial object and returns true or false
     *  Is used as a boolean function for Array.prototype.filter - if the function returns true, it will be included.
     *  See https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/filter
     *  Can filter out results from array by checking for certain parameters, etc.
     *  Replaces: defaultResultsFilter
     */
        /*
        function resultsFilter(t) 
        {
            //  Returns true for trials that have to be saved for data plotting (contain actual data)
            //  False for confirmation trials, etc.

            var isConfirm = $(t.DOM).find("isConfirmation").text(); //  output xml needs to define this element
            if (!isConfirm.length) return true;
            else return !getBool(isConfirm);
        }
        */

    /* lineFilter
     *  Receives a trial object and returns true or false
     *  Same as resultsFilter, but specifically for results to be used in line plotting
     *  Replaces: n/a
     */
        /*
        function lineFilter()
        {
        }
        */

    /* matrixFilter
     *  Receives a trial object and returns true or false
     *  Same as above, but specifically for results to be used in line plotting
     *  Replaces: n/a
     */
        /*
        function matrixFilter()
        {
        }
        */

    /* Convert Parameter Value
     *  Converts plotted value to another format/scale
     *  See Trial.prototype.parameterValue
     *  Replaces: n/a
     */
        /*
        function convertParameterValue(v) 
        {
            return v*100;
        }
        */
```

### The internals - - plot options

The plots are generated by the Highcharts.js library - (
[www.highcharts.com](www.highcharts.com) ). This holds several options
for every possible part of the plot such as xAxis, yAxis, labels,
legend, style, navigation, tooltips, etc. The chartConfig object employs
the same type of notation as our own config, but is slightly more
complex. For more information, look at the examples on
[www.highcharts.com/demo](www.highcharts.com/demo) .

```javascript
    /*  ============================================================================
     *  Highcharts configuration objects
     *  Documentation can be found on http://www.highcharts.com/docs
     *                            and http://api.highcharts.com/highcharts
     *  -
     *  Line:   http://www.highcharts.com/docs/chart-and-series-types/line-chart
     *  Matrix: http://www.highcharts.com/docs/chart-and-series-types/scatter-chart
                http://www.highcharts.com/demo/bubble
     *  Polar:  http://www.highcharts.com/docs/chart-and-series-types/polar-chart
     *  ============================================================================
     */
```

### The internals - 

The main JS file which implements the and other data-processing
functions. This picks between either or which can be custom defined and
has precedence over the default function. These functions map the
xmlstring to a JavaScript object called Trial and create an array called
.

The class definition of Trial is as follows:

```javascript
    function Trial(answer, correct_answer, correct, stimulusid, standardid, trialid, responsetime, proctype, procid, parametervalue, stepsize, reversals, saturated, xml) {
        //default
        this.answer = answer;
        this.correct_answer = correct_answer;
        this.correct = correct;
        this.stimulusid = stimulusid;
        this.standardid = standardid;
        this.trialid = trialid;
        this.responsetime = responsetime;
        this.proceduretype = proctype;
        this.procedureid = procid;

        //adaptive
        this.parametervalue = parametervalue;
        this.stepsize = stepsize;
        this.reversals = reversals;
        this.saturated = saturated;

        //raw data
        this.xmlDOM = xml;
    }
```

After calling newAnswer, is called and will process the data. It then
feeds it into the plotting mechanism for every enabled plot type.

First, the resultsviewer will try to detect if there is a
multiprocedure, or if there are procedures with different id’s (can be
forced with config). If it is, the results will be grouped by
proceduretype or procedureid (can be changed with ) and put into . These
keyvalues or groupnames will be stored in an ordered array called . The
id in the array corresponds with the index of the data in the Highcharts
series.

Every plottype has a specialized function for preparing data and adding
it to the visualization object. These are stored in the object.

The default plot types are:

line matrix polar text

The structure of the plots object is as follows:

```javascript
    /*  Object specification:
     *      - .data:         array of <object>s with prepared data for plot, see structure below
     *                      index corresponds to index of results.groupNames in case of multiprocedure
     *      - .highChart:    <true/false> true if this type of plot draws a HC plot
     *      - .chartConfig:  <object> to use the previous if true (see ...-config.js) (required)
     *      - .seriesConfig: <object> to use for adding series to charts (optional, in case of multiprocedures)
     *      - .prepare:      <function> for preparing plot data/....
     *      - .setData:      <function> for setting data in plot/div/... or updating plot-config
```

Each type of plot requires its own type of data-object. The structure
for each is shown at the top of the file.

```javascript
    /*  Data objects
     *  ============
     *  plots.line.data   = [{ values: [],      - Array with parametervalues of every trial
     *                         reversals: [],   - Array with reversals
     *                         meanrevs: n,     - Mean of last 6 reversals (set revsForMean to other number to change)
     *                         meanrevstd: n,   - meanrevs std
     *                         meantrials: n    - Mean of last 6 trials (set trialsForMean to other number to change)
     *                         meantrialstd:    - meantrials std
     *                      }]
     *
     *  plots.matrix.data = [{ values: [ { x: n,       - (int) xlabel link
     *                                     y: n,       - (int) ylabel link 
     *                                     z: n } ]    - (int) x & y pairings
     *                         percentages:     - % per stimulus correct (same as ..polar.data.values)
     *                         xlabels: [],     - correct answer / stimulus labels
     *                         ylabels: [],     - answer / response labels
     *                         raw: [][]        - 2D array used to parse matrix.values-objects
     *                      }]
     *
     *  plots.polar.data  = [{ values: []    - Array with % correct per correct answer/stimulus
     *                      }]   
     * 
     *  The following data is re-interpreted from other plotdata files, 
     *  but tabulated with html tags, etc.
     *  plots.text.data   = [{ line:  { allvalues,
     *                                  lastvalue,
     *                                  reversals,
     *                                  meanrevs,
     *                                  meantrials,
     *                                  paramtable
     *                                },
     *                         matrix:{ correctpercentage,
     *                                  correctpercentages,
     *                                  matrixtable,
     *                                  answertable,
     *                                }
     *                      }]
     */    
```

### The internals - 

This file holds most utilities and interface functionality for the
resultsviewer. If you intend to customize resultsviewer.js for your own
use, you will find a lot of helpful functions here.

Data processing related:

```javascript
    stringToBoolean(string)
        Takes any datatype and tries to turn it into a boolean value
        
    wrap(element, content, extra) 
        Wraps content in <element> tags, can have attributes defined in extra, e.g.: [attr("id","idname")]
    bold(content)
    uline(content)
    italic(content)
    table(content,extra)
    tr(content)
    th(content,extra)
    td(content,extra)
        Shorter notations for wrap() function
        
    attr(a,v)
        Returns an attribute object for use in the wrap() function
    attr_id(v)
    attr_class(v)
        Shorter notations for attr()
```

Array functions:

```javascript
    contains(str)
        Checks if an array contains a certain value
    unique()
        Returns an array with no repeating values
    sum()
        Returns sum of array
    last()
        Returns last element of array
    sortAlphaNumerical()
        Checks if array holds numbers in form of strings or if it holds text and sorts both cases correctly
```

String functions:

```javascript
    replaceMultiple(array)
        Goes through array of strings and removes them. e.g.: "button_5".replaceMultiple(["button","_"]) returns "5"
    capitalizeFirstLetter()
        Capitalizes first letter
```

Highcharts related:

```javascript
    drawGraph (type)                
        draws the Highcharts graph for that particular plottype into the div with id "<type>container". 
        Uses the plots.<type>.chartConfig object.
    printLineTooltip (i, x, y)
        Used as a formatter for drawing the line chart tooltip
    printMatrixTooltip (i, x, y, z)
        Used as a formatter for drawing the matrix chart tooltip
    removeSeries ()
        Removes all extra series from all Highcharts
    legendsOff ()
        Hides labels on charts
    getChart (type)
        Returns the chart object of that particular type. (must have rendered before)

    postPlot ()
        Function called after all processing and plotting has been done
    customStyleText ()
        Styles the text based on the config.text.textstyle and .tablestyle objects
    interfaceButtons ()
        Contains all interface rendering & behavior
    allUndefined ()
        Checks if no config for plottypes is defined in config.<type>.show 
    allOff ()
        Checks if all plottypes are turned off for config.<type>.show 
    onlyText ()
        Checks if only text is turned on for config.<type>.show 
```

Exporting results
-----------------

While you could simply copy-paste the relevant information from the
results XML file into the desired format, this is labour intensive and
error prone. Therefore several options are provided to convert the XML
file in a more user friendly format.

The first option, , will append a section to the results file containing
the essential data in comma separated values (CSV) format, which can
easily be copied to another program.

A next option, XSLT transforms, uses the XSLT programming language to
convert the results XML file into another format, which could be text,
XML, HTML, etc.

If you conduct your analysis in Matlab or R, the most efficient option
will be to use the APEX Matlab or R toolbox, and import your result XML
file directly. In the R toolbox, a function is also provided to convert
a series of results XML files into one main CSV file.

### Using the APEX Matlab Toolbox

### Using the APEX R Toolbox

### XSLT transforms <a name="using xslt transforms"/>

An XSLT script transforms the results XML file to a summary of the
results. An XSLT script is provided with APEX (), which can do the most
common transformations. You can also create your own scripts, provided
you have some XSLT knowledge. Please note that we are phasing out the
use of XSLT in APEX, in favour of HTML resultsviewer and the APEX Matlab
and R toolbox. Therefore the is no longer maintained, and we advise
against using it in new projects.

The processed results can be saved in the results XML file and/or shown
immediately after a test (in the last case the program asks whether you
want to see them or not).
