/*  ============================================================================
 *  Chart functions
 *  ============================================================================
 */
 function getChart(type)
{
    if (plots[type].highChart) {
        return $("#"+type+"container").highcharts();
    }
    else {
        return null;
    }
}

 function drawGraph(type)
 {
     // Parses the chartConfig objects &
     // draws graph in the container divs
     if (plots[type].highChart) {
         $("#"+type+"container").highcharts(_defineProperty( plots[type].chartConfig ));
     }
 }

// Tooltip formatters
function printLineTooltip(index,x,y)
{
    var s = "Adapted parameter";
    if (typeof config.line.parametername === "string") {
        s = config.line.parametername;
    }
    s = wrap("b", s);
    var pUn = "";
    if (typeof config.line.parameterunit === "string") {
        pUn = config.line.parameterunit;
    }
    var s = wrap("div",index,attr_class("seriesname"))
            + wrap("div", "<br>Trial " + bold(x) + " :<br> "+printParameterName() +"value " + bold(y) + " " 
            + printParameterUnit() );
    return s;
}
 
function printMatrixTooltip(index, x, y, z) {
    var cmData = plots.matrix.data[index];
    var s = "<b>Stimulus: " + cmData.xlabels[x] + "</b><br>  <b>Response: "
            + cmData.ylabels[y] + "</b><br>  <b>Pairings: " + cmData.raw[x][y] + "</b>";
    return s;
}

//  Remove series above index 0
function removeSeries()
{
    $.each(plots.types, function(i,type) {
        if (plots[type].highChart) {
            var c = getChart(type);
            if (!c)
                return;

            while( c.series.length > 1) {
                c.series[c.series.length - 1].remove();
            }
        }
    });
}

//  Turns off legends 
function legendsOff()
{
    $.each(plots.types, function(i,type) {
        if (plots[type].highChart) {
            var c = getChart(type);
            if (c) c.legend.update({ enabled: false},false);
            else plots[type].chartConfig.legend.enabled = false;
        }
    });
}


//============================================================================
//  RV internals (do not touch)
//============================================================================

//  For setting an empty object when the prepare functions have no results to work with
var dummy = {};
dummy.line = 
{
    values: [null],
    reversals: [0],
    meanrevs: 0,
    meanrevstd: 0,
    meantrials: 0,
    meantrialsstd: 0
}
dummy.matrix =
{
    values:[null],
    percentages:[],
    xlabels: [],
    ylabels: [],
    raw: []
}
dummy.polar =
{
   percentages:[null]
}
dummy.text = 
{
    line: {
        allvalues: "",
        lastvalue: "",
        reversals: "",
        meanrevs: "",
        meantrials: "",
        paramtable: ""
    },
    matrix: {
        correctpercentage: "",
        correctpercentages: "",
        matrixtable: "",
        answertable: ""
    }
}

function resetGroups()
{
    //  For turning off multiprocedure, resets all groups to 1 default groupproceduresby
    results.group = [];
    results.group["Default"] = results.all;
    results.groupNames = ["Default"];
    results.groupNames.length = 1;
}



/* =====================================
 *  HTML config tools & Config prepare functions
 * =====================================

/* Smart config prepare
  * Parses params[] into the config object
  * e.g. params["line_show"] -> config.line.show
  * Sets default values for various config keys
  */
 function prepareConfig()
 {
     if (cfgReady)
         return;
     
     // Parse resultparameters with proper notation
     if (typeof params !== "undefined") {
         $.each(params, function(key,value) {
             var configPath = key.split(/[\._]/); //  split by dot or underscore
             
             if (configPath.length > 1) {   //  did it split?
                 // force type for specific params
                 if (configPath[1] === "show")   value = stringToBoolean(value);
                 if (configPath[1] === "mindegree" || configPath[1] === "maxdegree") {
                     value = parseFloat(value);}
                 
                 config[configPath[0]][configPath[1]] = value;
             }
         });
     }
     
    //  equate to each other if only one is defined
    if (config.global.stimuli !== "undefined" && config.global.answers === "undefined") {
        config.global.answers = config.global.stimuli; }
    else if (config.global.stimuli === "undefined" && config.global.answers !== "undefined") {
        config.global.stimuli = config.global.answers; }
     
    //  split labels
    if (typeof config.global.answers === "string") {
        config.global.answers = config.global.answers.split(/,| /); }
    if (typeof config.global.stimuli === "string") {
        config.global.stimuli = config.global.stimuli.split(/,| /); }
    
    //  set to empty if undefined
    if (typeof config.global.removefromanswer === "undefined")   config.global.removefromanswer = [""];
    
    //  how many points after decimal
    if (typeof config.global.digitsafterdecimal === "undefined") config.global.digitsafterdecimal = 4;
    
    //  line chart yAxis units
    if (typeof config.line.parameterunit === "string") {
        plots.line.chartConfig.yAxis.labels = { format: '{value} '+ config.line.parameterunit };
    }
    
      //  line chart yAxis param name
    if (typeof config.line.parametername === "string") {
        plots.line.chartConfig.yAxis.title = { text: config.line.parametername };
    }
        
    cfgReady = true;
 }
 
 /* Prepares config.<type>.show based on results heuristics
  */
 function prepareConfigShow()
 {
    // Adaptive - Line graph
    if (results.all.some(function(t) 
            { return t.isAdaptiveProcedure(); })) config.line.show = true;
    else if (results.all.some(function(t) 
            { return t.hasValidParameter(); })) config.line.show = true;
    else config.line.show = false;

    // Constant - Confusion matrix (bubbleplot)
    if (results.all.some(function(t) 
            { return t.isConstantProcedure(); })) config.matrix.show = true;
    else config.matrix.show = false;

    // Localization - Polar plot
    config.polar.show = false;

    //  Text
    config.text.show = true;
 }
 
 
 /* Smart prepare multiprocedure config
  * Checks for multiple procedures in the results, by proceduretype or procedureid
  * Sets default values for various config keys
  */
  var forceMultiProcOff = false;
 function prepareMultiProcedureConfig()
 {
     // Don't do this if they turned the checkbox off before
     if (forceMultiProcOff)
         return;
     
    //  Check for multiple procedures
    //  Either sort by proceduretype (has priority) or procedureid
    var proctypes = results.all.map(function(t) { return t.proceduretype; }).unique();
    var procids   = results.all.map(function(t) { return t.procedureid; }).unique();
    
    if (typeof config.global.multiprocedure === "undefined") {
        config.global.multiprocedure = prepareGroupProceduresBy(proctypes,procids);
    }
    else {
        if (typeof config.global.groupproceduresby === "undefined") {
            prepareGroupProceduresBy(proctypes,procids);
        }
    }
    
    //  Turn on legend in every type of highchart
    if (config.global.multiprocedure) {
        $.each(plots.types, function(index,type) {
            if (plots[type].highChart)
                plots[type].chartConfig.legend.enabled = true;
        });
    }
    
    //  Decide config.global.groupproceduresby key
    function prepareGroupProceduresBy(tproctypes,tprocids)
    {
        if (proctypes.length > 1) {
            config.global.groupproceduresby = "proceduretype";
            return true;
        }
        else if (procids.length > 1) {
            config.global.groupproceduresby = "procedureid";
            return true;
        }
    }
    
    //  Group results by key (config.global.groupproceduresby) if multiprocedure is enabled
    var key = "proceduretype";
    if (config.global.multiprocedure) {
        if (typeof config.global.groupproceduresby !== "undefined")
            key = config.global.groupproceduresby;
        results.group = groupBy(results.all,key);
    }
    else {
        results.group["default"] = results.all;
    }
    
    //  loop over groupnames, see if they're already part of results.groupNames, if not then push
    $.each(Object.keys(results.group), function(index, key) {
        if (!results.groupNames.contains(key))
            results.groupNames.push(key);
    });
 }

function postPlot()
{
    //  Text re-style (must be at end)
    customStyleText();
    
    //  Set multiproc checkbox state
    updateMultiprocCheckbox();
    
    //  No data placeholder
    if (typeof($("#linegraphcontainer").highcharts()) === "undefined" && typeof($("#confusionmatrixcontainer").highcharts()) === "undefined" && typeof($("#polarplotcontainer").highcharts()) === "undefined") {
        $("#linegraphcontainer").html("<center><p>No data yet</p></center>");
    }
}

function customStyleText()
{
    if (typeof config.text.textstyle !== "undefined") 
       $('body').css(config.text.textstyle);
    
    if (typeof config.text.tablestyle !== "undefined")
       $('#textcontainer table tr td').css(config.text.tablestyle);
}

/*  ============================================================================
 *  Interface
 *  ============================================================================
 */
 
 function interfaceButtons()
 {
     //  Custom buttons
    $.each(customButtons, function(index, button) {
        //  Draw html
        //$("#custombuttonscontainer").append( "<button id=\""+button.name+"\" name=\""+button.name+"\">"+button.label+"</button>" );
        $("#custombuttonscontainer").append( wrap("button id=\""+button.name+"\" name=\""+button.name+"\"",button.label) );
        
        //  Behavior on click
        $("#"+button.name).click(function () 
        {
            button.behavior();
            if (button.replot)
                plot();
        });
    });
    
    //  Enable/disable buttons
    $.each(plots.types, function(index, type) {
        //  Draw html
        var bt = "<input type=\"checkbox\" class=\"enablebutton\" id=\"enable"+type+"\">"
        bt    += wrap("label", type.toString().capitalizeFirstLetter(), attr("for","enable"+type)); 
        bt     = wrap("div", bt, attr_class("spacer"));
        $("#chartbuttonscontainer").append( bt );
        
        //  Behavior on click
        $("#enable"+type).change(function () 
        {
            config[type].show = this.checked;
            
            if (this.checked) {
                plot();
                $("#"+type+"container").css("display","block");
            }
            else {
                $("#"+type+"container").css("display","none");
                plot();
            }
        });
    });
    
    drawMultiprocCheckbox();
    multiprocCheckbox(); 
    
    //  Set checkboxes to correct state (checks config object)
    updateCheckboxStates();
 }
 
 function updateCheckboxStates()
 {
    $.each(plots.types, function( index, type) {
        if (typeof config[type].show !== "undefined") {
            $("#enable"+type).prop("checked", config[type].show);
            if (config[type].show)
                $("#"+type+"container").css("display","block");
        }
    });
 }
 
 
/* Checks whether config.<type>.show is undefined for all plottypes
 */
function allUndefined()
{
    var b = true;
    $.each(plots.types, function(index, type) {
        b = (typeof config[type].show === "undefined" && b);
        if (!b)
            return false;
    });
    return b;
}

/* Checks whether config.<type>.show is false for all plottypes
 */
function allOff()
{
    var b = true;
    $.each(plots.types, function(index, type) {
        b = (!config[type].show && b);
        if (!b)
            return false;
    });
    return b;
}

/* Only text enabled?
 */
function onlyText() 
{
    return !config.line.show && !config.matrix.show && !config.polar.show && config.text.show;
}

var mpcReady = false;
function drawMultiprocCheckbox()
{
    if (!mpcReady && config.global.multiprocedure) {
    var bt = "<input type=\"checkbox\" class=\"enablebutton\" id=\"cbenablemultiproc\" checked=\"checked\" />"
        bt    += wrap("label", "Multiprocedure", attr("for","cbenablemultiproc")); 
        bt     = wrap("div", bt, attr_id("enablemultiproc"));
        $("#chartbuttonscontainer").append( bt );
        mpcReady = true;
    }
}

function multiprocCheckbox()
{
    $("#cbenablemultiproc").change(function () 
    {
        config.global.multiprocedure = this.checked;
        if (!this.checked) {
            $("#enablemultiproc").css("display","none");
            forceMultiProcOff = true;
            resetGroups();
            removeSeries();
            legendsOff();
            plot();
        }
        else {
            plot();
        }
    });
    
}

function updateMultiprocCheckbox()
{
    
    if (typeof config.global.multiprocedure !== "undefined") {
        if (config.global.multiprocedure) {
            $("#cbenablemultiproc").prop("checked",true);
            $("#enablemultiproc").css("display","block");
        }    
        else {
            $("#enablemultiproc").css("display","none");
        }
    }
}

//============================================================================
//  Javascript internals (do not touch)
//============================================================================

function _defineProperty(obj, key, value) 
{ 
    if (key in obj) { 
        Object.defineProperty(obj, key, 
        { value: value, enumerable: true, configurable: true, writable: true }); 
    } 
    else { 
        obj[key] = value; 
    } 
    return obj; 
}