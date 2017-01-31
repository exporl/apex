// Note: this file should be removed and integrated with resultsviewer.js



function plot_rtmultiprocedureparameter()
{
    var num;
    var i;
    var parametervalues;
    var answers;

    parseProcedureId();

    // get unique procedure ids:
    procedureIds = $.grep(results.procedureId, function(v, k){
        return $.inArray(v ,results.procedureId) === k;
    });

    // Create 2D parameter list:
    parametervalues = new Array(procedureIds.length);
    answers = new Array(procedureIds.length);
    for (i=0; i<procedureIds.length; ++i) {
        parametervalues[i] = new Array();
        answers[i] = new Array();
    }

    // Put each parameter value at correct spot
    for (i=0; i<results.procedureId.length; ++i) {
	num = jQuery.inArray( results.procedureId[i], procedureIds );
	parametervalues[num].push( results.parametervalues[i] );
	answers[num].push( results.answers[i] );
    }

    plotProcedureParameter(parametervalues, procedureIds);
    tableProcedureParameter(parametervalues, answers, procedureIds);
    tableProcedureParameterShort(parametervalues, answers, procedureIds);
    lastvalueProcedureParameter(parametervalues, procedureIds);
    meanrevsProcedureParameter(parametervalues, procedureIds);
    meantrialsProcedureParameter(parametervalues, procedureIds);
    valuesProcedureParameter(parametervalues, procedureIds);

    if (typeof extraPlot == 'function') {
	extraPlot();
    }
}

function parseProcedureId() {
    results.procedureId = [];
    for  (var i=0; i<results.xml.length; ++i) {
        results.procedureId.push(
                    $(results.xml[i]).find("procedure").attr("id") );
    }

}

function tableProcedureParameterShort(parametervalues, answers, ids)
{
    var r = '';
    for (var j=0; j<ids.length; ++j) {
	var line1 = "<td><b>Trial</b></td>";
	var line2 = "<td><b>Value</b></td>";
	for (var i=0; i<parametervalues[j].length; ++i) {
	    line1 += wrap("td", i+1);
	    line2 += wrap("td", parametervalues[j][i]);
	}
	r += wrap("table", "<p><b>" + ids[j] + " staircase: </b><p>" +
		wrap("tr", line1) + wrap("tr", line2));
    }

    replaceContent("shortprocedureparametertable", r);
}



function tableProcedureParameter(parametervalues, answers, ids)
{
    var r = '';
    for (var j=0; j<ids.length; ++j) {
	var table=wrap("tr", "<th>Trial</th><th>Procedure</th><th>Answer</th><th>Parameter</th>");
	for (var i=0; i<parametervalues[j].length; ++i) {
	    table+=wrap("tr",
		    wrap("td", i+1)+
		    wrap("td", answers[j][i])+
		    wrap("td", parametervalues[j][i])   );
	}
	table=wrap("table", table);

	r += table;
    }

    replaceContent("procedureparametertable", r);
}


function plotProcedureParameter(parametervalues, ids)
{
    var test = $("#procedureparameterplot")[0];
    if ( $("#procedureparameterplot")[0].firstChild !== null)
	 $("#procedureparameterplot >*").remove();

    var d = new Array(parametervalues.length);
    for (var j=0; j<parametervalues.length; ++j) {
	d[j] = new Array(parametervalues[j].length);
        for (var i=0; i<parametervalues[j].length; ++i)
		d[j][i] = [i+1, parametervalues[j][i] ];
    }

    // setup legends
    var seriesdata = [];
    for (var j=0; j<ids.length; ++j) {
	seriesdata.push( {label: ids[j]} );
    }

    $.jqplot("procedureparameterplot", d ,{
	    axes:{
	      xaxis:{
		label:'Trial #',
	        autoscale: true,
//	        labelRenderer: $.jqplot.CanvasAxisLabelRenderer,
	        labelOptions: {
	          enableFontSupport: true,
		  fontFamily: 'Georgia',
	          fontSize: '12pt'
		}
	      },
	     yaxis:{
		label:'Parameter value',
	        autoscale: true,
//	        labelRenderer: $.jqplot.CanvasAxisLabelRenderer,
	        labelOptions: {
	          enableFontSupport: true,
		  fontFamily: 'Georgia',
	          fontSize: '12pt'
		}

	      },
	     legend: {
		 show: true,
		 location: 'ne'
	     },
	     series: seriesdata
	    }
	    } );


}


function lastvalueProcedureParameter(parametervalues, ids)
{
    var d='';
    for (var j=0; j<ids.length; ++j) {
	d += '<p>' +  ids[j] + ': <b>Last value:</b> ' + parametervalues[j][parametervalues[j].length-1] + '</p>';
    }

    replaceContent("procedureparameterlastvalue",d);
}

function getReversals(parametervalues)
{
 // Get reversals
    var reversals = Array();
    var up = 0;
    var down = 0;
    for (var i=1; i<parametervalues.length; ++i) {
	if (parametervalues[i]>parametervalues[i-1]) {
	    if (down) {
		reversals.push(parametervalues[i-1]);
		down = 0;
	    }
	    up = 1;
	}

	if (parametervalues[i]<parametervalues[i-1]) {
	    if (up) {
		reversals.push(parametervalues[i-1]);
		up = 0;
	    }
	    down = 1;
	}
    }
    reversals.push(parametervalues[parametervalues.length-1]);

    return reversals;
}

function meanrevsProcedureParameter(parametervalues, ids)
{
    // Check if reversalsForMean is defined
    var rfm = 6;
    if (typeof(reversalsForMean) != "undefined")
	rfm = reversalsForMean;

    var d = '';
    var allmeans = new Array(ids.length);

    for (var j=0; j<ids.length; ++j) {

	// Get reversals
	var reversals = getReversals(parametervalues[j]);
	var nrevs = Math.min(rfm, reversals.length);

	// Calculate average
	var sum = 0;
	for (var i=reversals.length-nrevs; i<reversals.length; ++i) {
	    sum += reversals[i];
	}
	var average = sum/nrevs;

	// Calculate std
	sum = 0;
	for (var i=reversals.length-nrevs; i<reversals.length; ++i) {
	    sum += Math.pow(reversals[i] - average, 2);
	}
	var std = Math.sqrt(sum/nrevs);

	allmeans[j] = average;

	d += '<p>' + ids[j] + ': <b>Mean (std) last '
	    + nrevs + ' reversals:</b> ' + average +
	    ' (&plusmn;' + Math.round(std*1000)/1000 + ')</p>';

    }

    var sum = 0;
    for (var i=0; i<allmeans.length; ++i) {
	sum += allmeans[i];
    }
    d += '<p><b>Mean across procedures: ' + sum / allmeans.length;

    replaceContent("procedureparametermeanrevs",d);
}


function meantrialsProcedureParameter(parametervalues, ids)
{
    // Check if reversalsForMean is defined
    var rfm = 6;
    if (typeof(trialsForMean) != "undefined")
	rfm = trialsForMean;

    var d = '';

    for (var j=0; j<ids.length; ++j) {

	var nvalues = Math.min(rfm, parametervalues[j].length);

	// Calculate average
	var sum = 0;
	for (var i=parametervalues[j].length-nvalues; i<parametervalues[j].length; ++i) {
	    sum += parametervalues[j][i];
	}
	var average = sum/nvalues;

	// Calculate std
	sum = 0;
	for (var i=parametervalues[j].length-nvalues; i<parametervalues[j].length; ++i) {
	    sum += Math.pow(parametervalues[j][i] - average, 2);
	}
	var std = Math.sqrt(sum/nvalues);

	d += '<p>' + ids[j] + ':<b>Mean (std) last '
	    + nvalues + ' trials:</b> ' + average +
	    ' (&plusmn' + Math.round(std*1000)/1000 + ')</p>';

    }
    replaceContent("procedureparametermeantrials", d);
}







function valuesProcedureParameter(parametervalues, ids)
{
   if (! $("#procedureparameterreversals") ) {
	return;
    }


   var d = '';
   for (var j=0; j<ids.length; ++j) {

    var revs = getReversals(parametervalues[j]);
       var s = "";
       for (var i=0; i<revs.length; ++i) {
	   s += revs[i] + " ";
       }
       d +=  '<p>' + ids[j] + ": <b>Reversals:</b> " + s;

   }

   replaceContent("procedureparameterreversals",d);

}


//////////////////////////////////////////////////////////////////////////////
// Data structures
//////////////////////////////////////////////////////////////////////////////

// Data obtained from APEX will be parsed into results, with one element per trial
var results = {};
results.answers=new Array();
results.correctanswers=new Array();
results.parametervalues=new Array();
results.trials=new Array();
results.stimuli=new Array();
results.standards=new Array();
results.results=new Array();
results.xml=new Array();		// Full XML code as received from APEX


//////////////////////////////////////////////////////////////////////////////
// Utility functions
//////////////////////////////////////////////////////////////////////////////


Array.prototype.unique =
function() {
  var a = [];
  var l = this.length;
  for(var i=0; i<l; i++) {
   for(var j=i+1; j<l; j++) {
	    // If this[i] is found later in the array
	    if (this[i] === this[j])
        j = ++i;
    }
    a.push(this[i]);
  }
  return a;
};

function wrap(element, content)
{
    return "<" + element + ">" + content + "</" + element + ">";
}

function replaceContent(id, s)
{
    var sel = $("#" + id);
    if (!sel) {
	return;
    }
    sel.html(s);
}


//////////////////////////////////////////////////////////////////////////////
// Basic results processing
//////////////////////////////////////////////////////////////////////////////


function newAnswer(xmlstring)
{
    var xml = (new DOMParser()).parseFromString(xmlstring, "application/xml");
    var parameter = $(xml).find("parameter").text();
    var answer = $(xml).find("answer").text();
    var correctanswer = $(xml).find("correct_answer").text();
    var trial = $(xml).find("trial").attr("id");
    var stimulus = $(xml).find("stimulus").text();
    var standard = $(xml).find("standard").text();

    var result = $(xml).find("correct");
    var result;
    if (!result.length)
        result = $(xml).find("corrector > result");
    result = (result.text() == "true");

    var paramelement = $(xml).find("procedure > parameter");
    if (paramelement.length) {
	var pv = parseFloat(paramelement.text());

       if (typeof convertParameterValue == 'function') {
           pv = convertParameterValue( pv );
       }

       results.parametervalues.push( pv );
   }

   results.answers.push(answer);
   results.correctanswers.push(correctanswer);
   results.trials.push(trial);
   results.stimuli.push(stimulus);
   results.standards.push(stimulus);
   results.results.push(result);

   results.xml.push(xmlstring);

   if (typeof extraNewAnswer == 'function') {
       extraNewAnswer(xmlstring);
   }

}

function containsAdaptive(xml){
  var isAdaptive = false;
  var procedures = $(xml).find("procedure").each(function() {
    var procedureType = $(this).attr("type");
    if(procedureType === "apex:adaptiveProcedure"){
      isAdaptive = true;
    }
  })

  return isAdaptive;
}
