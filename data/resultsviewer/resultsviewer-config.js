/*  ============================================================================
 *  RESULTSVIEWER CONFIGURATION
 *  ____________________________________________________________________________
 *  Customize parameters & functionality for visualizing Apex results files
 *  ============================================================================
 */

/*  ============================================================================
 *  Resultsviewer configuration object
 *  - these settings are used throughout resultsviewer.js
 *  - can also be set with the following syntax (e.g. in resultscript):
            config.line.parametername = "Gain";
 *  - or through the resultparameters:
            <resultparameters>
                <parameter name="polar_show">true</parameter>
                <parameter name="polar_mindegree">-45</parameter>
            </resultparameters>
 *  ============================================================================
 */

var config = 
{
    /* Global options
     *  stimuli:            <array of strings> full list of correct answers / presented stimuli
     *                                          - eg: ["button-90","button-45","button0","button45","button90"]
     *                                            or  ["-90","-75","-60","-45","-30","-15","0","15","30","45","60","75","90"]
     *  answers:            <array of strings> full list of possible answers / user responses
     *  removefromanswer:   <array of strings> list of parts that can be pruned from answerlabels, eg: "button_chair" becomes "chair"
     *                                          - if only one of these two is defined, they will be set to the same value
     *  multiprocedure:     <true/false> if true, will plot each different procedure separately (detected automatically if left undefined)
     *  groupproceduresby:  <string> decides which key to group the procedures by e.g.: "proceduretype"(default) or "procedureid"
     */
        global: 
        {
            //answers:             [""],
            //stimuli:             [""],
            removefromanswer:    ["_button","button_","button","b_","answer_","_answer","answer"],
            //multiprocedure:      false,
            //groupproceduresby:   "" 
        },
    
    
    /* Line graph options
     *  show:               <true/false> force line graph to show (true) or hide (false)
     *  parametername:      <string> shows parameter name on graph - e.g. "Gain"
     *  parameterunit:      <string> unit for parameter values - e.g. "dB SPL"
     *  parameterkey:       <string> the Trial object key to use for parameter plotting (default: parametervalue) - e.g. "responsetime"
     *  trialsformean:      <int> the amount of trials used for mean (default 6)
     *  reversalsformean:   <int> the amount of reversals used for mean (default 6)
     */
        line:
        {
            //show:             true,
            //parametername:    "",
            //parameterunit:    "",
            //parameterkey:     "",
            //trialsformean:    ,
            //reversalsformean: 
        },
    
    
    /* Confusion matrix options
     *  show:               force matrix to show (true) or hide (false)
     *  removefrommatrix:   list of answers and stimuli that may be filtered out of matrix plotting
     *                      e.g.: ["up","down"]
     */
        matrix:
        {
            //show:             true,
            //removefrommatrix: ["up","down"]
        },
    
    
    /* Polar plot options
     *  show:               force polar plot to show (true) or hide (false)
     *  mindegree:          smallest degree of localization-arc (default -90)
     *  maxdegree:          largest degree of .. (default 90)
     */
        polar:
        {
            //show: true,
            //mindegree: ,
            //maxdegree: 
        },
        
    /* Text printing options (applies to result data text)
     *  show:               force text to show (true) or hide (false)
     *  digitsafterdecimal: how many digits to show after decimal point (default 4)
     *  textstyle:          takes a css style object, applies it to ALL untabulated text
     *  tablestyle          same, for tabulated text
     *                      eg: {"font-size" : "120%", "font-weight" : "bold", "color" : "white", "background-color":"black"}
     *                      doc: http://api.jquery.com/css/#css-properties
     */
        text:
        {
            //show: true,
            //digitsafterdecimal: ,
            //textstyle: {},
            //tablestyle: {}
        },
};



/*  ============================================================================
 *  Results processing functions customization
 *  
 *  The following functions have a default implementation but can be re-implemented.
 *  Resultsviewer.js will automatically detect custom functions and use these instead.
 *  ============================================================================
 */
 

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
        //  Filtered trial objects are permanently removed from results array

        var isConfirm = $(t.DOM).find("isConfirmation").text(); //  output xml needs to define this element
        if (!isConfirm.length)
            return true;
        else
            return !stringToBoolean(isConfirm);
    }
    */

/* lineDataFilter
 *  Receives a trial object and returns true or false
 *  Same as resultsFilter, but specifically for results to be used in line plotting
 *  Filtered out results still remain in original results array
 *  Replaces: n/a
 */
    /*
    function lineDataFilter()
    {
        
    }
    */

/* matrixDataFilter
 *  Receives a trial object and returns true or false
 *  Same as above, but specifically for results to be used in matrix and polar plot
 *  Filtered out results still remain in original results array
 *  Replaces: n/a
 */
    /*
    function matrixDataFilter()
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

    
/*  ============================================================================
 *  Custom buttons
 *
 *  - Adds buttons to the resultsviewer window (under checkboxes)
 *  - Can be used to add/transform/... data or visualization
 *  - Push customButton object to array with (all required):
                        - name: <string> reference name (cannot contain spaces/special chars)
                        - label: <string> label on button (can contain anything)
                        - replot: <true/false> whether or not to call plot() after
                                  use true when modifying plotdata
                        - behavior: <function> to be executed on click
 *  Documentation:      http://api.highcharts.com/highstock/Chart.addSeries
                        http://api.highcharts.com/highcharts/Series.update
                        http://api.highcharts.com/highcharts/Series.setData
                        
/*  ============================================================================ 
 */
 
 var customButtons = [];
 
 // Use this block as template
 /*
 customButtons.push({ name: "",
                      label: "",
                      replot: <true/false>,
                      behavior: function()
                               { 
                                    //code
                               }
                     });
 */
 
 // Examples (for line graph)
 /*
 customButtons.push({ name: "btn1",
                     label: "Add series",
                     replot: true,
                     behavior: function()
                               { 
                                    var target = getChart("line");
                                    if (typeof target === "null")   alert("Enable line first");
                                    
                                    target.addSeries( { data: [1,2,3,4,5,6] } );
                                    target.redraw();
                               }
                    });
*/
/*
 customButtons.push({ name: "btn2",
                     label: "Toggle columns",
                     replot: true,
                     behavior: function()
                               { 
                                    var target = getChart("line");
                                    if (typeof target === "null")   alert("Enable line first");
                                    
                                    target.series[0].update( { type: "column" }, false );
                               }
                    });
*/
/*
 customButtons.push({ name: "btn3",
                     label: "Add axis",
                     replot: true,
                     behavior: function()
                               { 
                                    var target = getChart("line");
                                    if (typeof target === "null")   alert("Enable line first");
                                    
                                    target.addAxis({ id: 'ReactionAxis', title: { text: 'Reaction' }, labels: { format: '{value} ms' }, opposite: true });
                                    target.series[1].update({yAxis: 1});
                               }
                    });
*/

/*  ============================================================================
 *  ADVANCED CUSTOMIZATION
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
 
/*  Line chart config
 */
 plots.line.chartConfig =
 {
    title: {
        text: 'Adaptive procedure',
        style: {"fontSize" : "1.6em", "fontFamily" : "Open Sans", "color" : "#666666", "fontWeight" : "bold"}
    },
    subtitle: {
        text: 'Experiment Results'
    },
    xAxis: {
        allowDecimals: false,
        floor: 1,
        tickInterval: 1,
        title: {
            text: 'Trial n°'
        }
    },
    yAxis: {
        allowDecimals: true,
        tickInterval: 1,
        title: {
            text: 'Parameter values'
        },
        labels: {
            format: '{value}'
        },
        plotLines: [{
            value: 0,
            width: 1,
            color: '#808080'
        }]
    },
    tooltip: {
        crosshairs: [true, true],
        formatter: function(){
            return printLineTooltip(this.series.name,this.x,this.y);
        }
    },
    legend: {
        backgroundColor: '#eee',
        enabled: false
    },
    series: [],
    plotOptions: {
        series: {
            pointStart: 1,
            allowPointSelect: false,
            color: 'rgba(41, 180, 180, 0.80)'
        }
    },
    lang: {
        noData: "no data"
    },
    noData: {
        style: {
            fontWeight: 'bold',
            fontSize: '13px',
            color: '#303030'
        }
    },
    credits: {
        enabled: false
    }
};
 
/*  Matrix chart config
 */
  plots.matrix.chartConfig =
 {
    title: {
        text: 'Constant Procedure',
        style: {"fontSize" : "1.6em", "fontFamily" : "Open Sans", "color" : "#666666", "fontWeight" : "bold"}
    },
    subtitle: {
        text: 'Experiment Results'
    },
    chart: {
        type: 'bubble'
    },
    xAxis: {
        allowDecimals: false,
        gridLineWidth: 1,
        labels: {
            format: '{value}'
        },
        tickInterval: 1,
        title: {
            text: 'Stimulus'
        },
        categories: [] //   x-labels / stimuli / stimuli

    },
    yAxis: {
        reversed: true,
        allowDecimals: false,
        tickInterval: 1,
        floor: 0,
        ceiling: 1,
        title: {
            text: 'Response'
        },
        labels: {
            format: '{value}'
        },
        categories: [] //   y-labels / answers / responses
    },
    tooltip: {
        crosshairs: [true, true],
        formatter: function() {
            return printMatrixTooltip(this.series.index, this.point.x,this.point.y,this.point.z);
        }
    },
    legend: {
        backgroundColor: '#eee',
        enabled: false
    },
    series: [{
        data: {}
    }],
    plotOptions: {
        series: {
            pointStart: 1,
            allowPointSelect: false,
            color: 'rgba(41, 180, 180, 0.80)',
            dataLabels: {
                enabled: true,
                crop: false,
                enabled: true,
                //overflow: "none",
                style: {
                    fontSize: "1.2em",
                    color: "black"
                },
                formatter: function formatter() {
                    if (this.point.z == 1)
                        return;
                    return this.point.z;
                }
            }
        } 
    },
    credits: {
        enabled: false
    },
    lang: {
        noData: "no data"
    },
    noData: {
        style: {
            fontWeight: 'bold',
            fontSize: '13px',
            color: '#303030'
        }
    }
};
 
/*  Polar chart config
 */
  plots.polar.chartConfig =
 {
    title: {
        text: 'Localization',
        style: {"fontSize" : "1.6em", "fontFamily" : "Open Sans", "color" : "#666666", "fontWeight" : "bold"}
    },

    chart: {
        polar: true
    },
    subtitle: {
        text: '% correct per direction'
    },
    pane: {
        startAngle: -90,
        endAngle: 90,
        center: ["50%", "85%"],
        size: "140%"
    },

    xAxis: {
        tickInterval: 45,
        min: -90,
        max: 90,
        showLastLabel: true,
        labels: {
            formatter: function formatter() {
                return this.value + '°';
            }
        }
    },
    yAxis: {
        min: 0,
        max: 1,
        labels: {
            enabled: false
        }
    },
    tooltip: {
        formatter: function formatter() {
            return 'The score for <b>' + this.x + ' °</b> is <b>' + (this.y * 100).toFixed(3) + ' %</b>';
        }
    },
    plotOptions: {
        series: {
            pointStart: -90,
            pointInterval: 45,
            pointPlacement: 'on',
            borderWidth: 2,
            borderColor: 'rgba(255, 255, 255, 0.50)'
        },
        column: {
            pointPadding: 0,
            groupPadding: 0
        }
    },
    legend: {
        enabled: false
    },
    series: [{
        type: 'column',
        name: 'Column',
        data: [],
        color: 'rgba(41, 180, 180, 0.80)'
    }],
    credits: {
        enabled: false
    },
    lang: {
        noData: "no data"
    },
    noData: {
        style: {
            fontWeight: 'bold',
            fontSize: '13px',
            color: '#303030'
        }
    }
};

plots.polar.seriesConfig = 
{
    type: 'column'
}

/* Chart colors
 * picked to be compound colors of first color
 */
 
config.global.chartColors = [ "rgba(41,  180, 180, 0.80)",
                              "rgba(231, 70,  56,  0.80)",
                              "rgba(58,  68,  78,  0.80)",
                              "rgba(56,  139, 231, 0.80)",
                              "rgba(189, 130, 113, 0.80)" ];
                              