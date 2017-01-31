"use strict";

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
  return $(xml).find("procedure > parameter").length > 0;
}


//////////////////////////////////////////////////////////////////////////////
// Adaptive staircase plotting
//////////////////////////////////////////////////////////////////////////////

function plot_rtprocedureparameter()
{
    plotProcedureParameter(results.parametervalues);
    tableProcedureParameter(results.parametervalues);
    lastvalueProcedureParameter(results.parametervalues);
    meanrevsProcedureParameter(results.parametervalues);
    meantrialsProcedureParameter(results.parametervalues);
    reversalsProcedureParameter(results.parametervalues);
    valuesProcedureParameter(results.parametervalues);
    if (typeof extraPlot == 'function') {
	   extraPlot();
    }
 }


function tableProcedureParameter(parametervalues)
{
    var table=wrap("tr", "<th>Trial</th><th>Answer</th><th>Parameter</th>");
    for (var i=0; i<results.answers.length; ++i)
	table+=wrap("tr",
		wrap("td", i+1)+
		wrap("td", results.answers[i])+
		wrap("td", parametervalues[i])
		);
    table=wrap("table", table);

    replaceContent("procedureparametertable", table);
}


function plotProcedureParameter(parametervalues)
{
    var test = $("#procedureparameterplot")[0];
    if ( $("#procedureparameterplot")[0].firstChild !== null)
	 $("#procedureparameterplot >*").remove();

    var d = new Array();
    for (var i=0; i<parametervalues.length; ++i)
	d.push( [i+1, parametervalues[i] ]);
    $.jqplot("procedureparameterplot",[ d ] ,{
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

	      }
	    }
	    } );


}


function lastvalueProcedureParameter(parametervalues)
{
    replaceContent("procedureparameterlastvalue",
	    '<p><b>Last value:</b> ' + parametervalues[parametervalues.length-1] + '</p>');
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

    // Last (virtual) trial is not a reversal
    // reversals.push(parametervalues[parametervalues.length-1]);

    return reversals;
}

function meanrevsProcedureParameter(parametervalues)
{
    // Check if reversalsForMean is defined
    var rfm = 6;
    if (typeof(reversalsForMean) != "undefined")
	rfm = reversalsForMean;

    // Get reversals
    var reversals = getReversals(parametervalues);
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

    replaceContent("procedureparametermeanrevs",
	    '<p><b>Mean (std) last '
	    + nrevs + ' reversals:</b> ' + average +
	    ' (&plusmn;' + Math.round(std*1000)/1000 + ')</p>');

}


function meantrialsProcedureParameter(parametervalues)
{
    // Check if reversalsForMean is defined
    var rfm = 6;
    if (typeof(trialsForMean) != "undefined")
	rfm = trialsForMean;

    var nvalues = Math.min(rfm, parametervalues.length);

    // Calculate average
    var sum = 0;
    for (var i=parametervalues.length-nvalues; i<parametervalues.length; ++i) {
	sum += parametervalues[i];
    }
    var average = sum/nvalues;

    // Calculate std
    sum = 0;
    for (var i=parametervalues.length-nvalues; i<parametervalues.length; ++i) {
	sum += Math.pow(parametervalues[i] - average, 2);
    }
    var std = Math.sqrt(sum/nvalues);

    replaceContent("procedureparametermeantrials",
	    '<p><b>Mean (std) last '
	    + nvalues + ' trials:</b> ' + average +
	    ' (&plusmn' + Math.round(std*1000)/1000 + ')</p>');
}




function reversalsProcedureParameter(parametervalues)
{
    if (! $("#procedureparametervalues") ) {
	return;
    }

    var s = "";
    for (var i=0; i<parametervalues.length; ++i) {
	s += parametervalues[i] + " ";
    }

    replaceContent("procedureparametervalues", "<b>Parameter values:</b> " + s);
}

function valuesProcedureParameter(parametervalues)
{
   if (! $("#procedureparameterreversals") ) {
	return;
    }

    var revs = getReversals(parametervalues);

    var s = "";
    for (var i=0; i<revs.length; ++i) {
	s += revs[i] + " ";
    }

    replaceContent("procedureparameterreversals", "<b>Reversals:</b> " + s);

}


//////////////////////////////////////////////////////////////////////////////
// Multiprocedure staircase plotting
//////////////////////////////////////////////////////////////////////////////

// Should be integrated with single staircase plotting

// function plot_rtmultiprocedureparameter()
// {
//     var num;
//     var i;
//     var parametervalues;
//     var answers;

//     parseProcedureId();

//     // get unique procedure ids:
//     procedureIds = $.grep(results.procedureId, function(v, k){
//         return $.inArray(v ,results.procedureId) === k;
//     });

//     // Create 2D parameter list:
//     parametervalues = new Array(procedureIds.length);
//     answers = new Array(procedureIds.length);
//     for (i=0; i<procedureIds.length; ++i) {
//         parametervalues[i] = new Array();
//         answers[i] = new Array();
//     }

//     // Put each parameter value at correct spot
//     for (i=0; i<results.procedureId.length; ++i) {
// 	num = jQuery.inArray( results.procedureId[i], procedureIds );
// 	parametervalues[num].push( results.parametervalues[i] );
// 	answers[num].push( results.answers[i] );
//     }

//     plotProcedureParameter(parametervalues, procedureIds);
//     tableProcedureParameter(parametervalues, answers, procedureIds);
//     tableProcedureParameterShort(parametervalues, answers, procedureIds);
//     lastvalueProcedureParameter(parametervalues, procedureIds);
//     meanrevsProcedureParameter(parametervalues, procedureIds);
//     meantrialsProcedureParameter(parametervalues, procedureIds);
//     valuesProcedureParameter(parametervalues, procedureIds);

//     if (typeof extraPlot == 'function') {
// 	extraPlot();
//     }
// }

// function parseProcedureId() {
//     results.procedureId = [];
//     for  (var i=0; i<results.xml.length; ++i) {
//         results.procedureId.push(
//                     $(results.xml[i]).find("procedure").attr("id") );
//     }

// }

// function tableProcedureParameterShort(parametervalues, answers, ids)
// {
//     var r = '';
//     for (var j=0; j<ids.length; ++j) {
// 	var line1 = "<td><b>Trial</b></td>";
// 	var line2 = "<td><b>Value</b></td>";
// 	for (var i=0; i<parametervalues[j].length; ++i) {
// 	    line1 += wrap("td", i+1);
// 	    line2 += wrap("td", parametervalues[j][i]);
// 	}
// 	r += wrap("table", "<p><b>" + ids[j] + " staircase: </b><p>" +
// 		wrap("tr", line1) + wrap("tr", line2));
//     }

//     replaceContent("shortprocedureparametertable", r);
// }



// function tableProcedureParameter(parametervalues, answers, ids)
// {
//     var r = '';
//     for (var j=0; j<ids.length; ++j) {
// 	var table=wrap("tr", "<th>Trial</th><th>Procedure</th><th>Answer</th><th>Parameter</th>");
// 	for (var i=0; i<parametervalues[j].length; ++i) {
// 	    table+=wrap("tr",
// 		    wrap("td", i+1)+
// 		    wrap("td", answers[j][i])+
// 		    wrap("td", parametervalues[j][i])   );
// 	}
// 	table=wrap("table", table);

// 	r += table;
//     }

//     replaceContent("procedureparametertable", r);
// }


// function plotProcedureParameter(parametervalues, ids)
// {
//     var test = $("#procedureparameterplot")[0];
//     if ( $("#procedureparameterplot")[0].firstChild !== null)
// 	 $("#procedureparameterplot >*").remove();

//     var d = new Array(parametervalues.length);
//     for (var j=0; j<parametervalues.length; ++j) {
// 	d[j] = new Array(parametervalues[j].length);
//         for (var i=0; i<parametervalues[j].length; ++i)
// 		d[j][i] = [i+1, parametervalues[j][i] ];
//     }

//     // setup legends
//     var seriesdata = [];
//     for (var j=0; j<ids.length; ++j) {
// 	seriesdata.push( {label: ids[j]} );
//     }

//     $.jqplot("procedureparameterplot", d ,{
// 	    axes:{
// 	      xaxis:{
// 		label:'Trial #',
// 	        autoscale: true,
// //	        labelRenderer: $.jqplot.CanvasAxisLabelRenderer,
// 	        labelOptions: {
// 	          enableFontSupport: true,
// 		  fontFamily: 'Georgia',
// 	          fontSize: '12pt'
// 		}
// 	      },
// 	     yaxis:{
// 		label:'Parameter value',
// 	        autoscale: true,
// //	        labelRenderer: $.jqplot.CanvasAxisLabelRenderer,
// 	        labelOptions: {
// 	          enableFontSupport: true,
// 		  fontFamily: 'Georgia',
// 	          fontSize: '12pt'
// 		}

// 	      },
// 	     legend: {
// 		 show: true,
// 		 location: 'ne'
// 	     },
// 	     series: seriesdata
// 	    }
// 	    } );


// }


// function lastvalueProcedureParameter(parametervalues, ids)
// {
//     var d='';
//     for (var j=0; j<ids.length; ++j) {
// 	d += '<p>' +  ids[j] + ': <b>Last value:</b> ' + parametervalues[j][parametervalues[j].length-1] + '</p>';
//     }

//     replaceContent("procedureparameterlastvalue",d);
// }

// function getReversals(parametervalues)
// {
//  // Get reversals
//     var reversals = Array();
//     var up = 0;
//     var down = 0;
//     for (var i=1; i<parametervalues.length; ++i) {
// 	if (parametervalues[i]>parametervalues[i-1]) {
// 	    if (down) {
// 		reversals.push(parametervalues[i-1]);
// 		down = 0;
// 	    }
// 	    up = 1;
// 	}

// 	if (parametervalues[i]<parametervalues[i-1]) {
// 	    if (up) {
// 		reversals.push(parametervalues[i-1]);
// 		up = 0;
// 	    }
// 	    down = 1;
// 	}
//     }
//     reversals.push(parametervalues[parametervalues.length-1]);

//     return reversals;
// }

// function meanrevsProcedureParameter(parametervalues, ids)
// {
//     // Check if reversalsForMean is defined
//     var rfm = 6;
//     if (typeof(reversalsForMean) != "undefined")
// 	rfm = reversalsForMean;

//     var d = '';
//     var allmeans = new Array(ids.length);

//     for (var j=0; j<ids.length; ++j) {

// 	// Get reversals
// 	var reversals = getReversals(parametervalues[j]);
// 	var nrevs = Math.min(rfm, reversals.length);

// 	// Calculate average
// 	var sum = 0;
// 	for (var i=reversals.length-nrevs; i<reversals.length; ++i) {
// 	    sum += reversals[i];
// 	}
// 	var average = sum/nrevs;

// 	// Calculate std
// 	sum = 0;
// 	for (var i=reversals.length-nrevs; i<reversals.length; ++i) {
// 	    sum += Math.pow(reversals[i] - average, 2);
// 	}
// 	var std = Math.sqrt(sum/nrevs);

// 	allmeans[j] = average;

// 	d += '<p>' + ids[j] + ': <b>Mean (std) last '
// 	    + nrevs + ' reversals:</b> ' + average +
// 	    ' (&plusmn;' + Math.round(std*1000)/1000 + ')</p>';

//     }

//     var sum = 0;
//     for (var i=0; i<allmeans.length; ++i) {
// 	sum += allmeans[i];
//     }
//     d += '<p><b>Mean across procedures: ' + sum / allmeans.length;

//     replaceContent("procedureparametermeanrevs",d);
// }


// function meantrialsProcedureParameter(parametervalues, ids)
// {
//     // Check if reversalsForMean is defined
//     var rfm = 6;
//     if (typeof(trialsForMean) != "undefined")
// 	rfm = trialsForMean;

//     var d = '';

//     for (var j=0; j<ids.length; ++j) {

// 	var nvalues = Math.min(rfm, parametervalues[j].length);

// 	// Calculate average
// 	var sum = 0;
// 	for (var i=parametervalues[j].length-nvalues; i<parametervalues[j].length; ++i) {
// 	    sum += parametervalues[j][i];
// 	}
// 	var average = sum/nvalues;

// 	// Calculate std
// 	sum = 0;
// 	for (var i=parametervalues[j].length-nvalues; i<parametervalues[j].length; ++i) {
// 	    sum += Math.pow(parametervalues[j][i] - average, 2);
// 	}
// 	var std = Math.sqrt(sum/nvalues);

// 	d += '<p>' + ids[j] + ':<b>Mean (std) last '
// 	    + nvalues + ' trials:</b> ' + average +
// 	    ' (&plusmn' + Math.round(std*1000)/1000 + ')</p>';

//     }
//     replaceContent("procedureparametermeantrials", d);
// }







// function valuesProcedureParameter(parametervalues, ids)
// {
//    if (! $("#procedureparameterreversals") ) {
// 	return;
//     }


//    var d = '';
//    for (var j=0; j<ids.length; ++j) {

//     var revs = getReversals(parametervalues[j]);
//        var s = "";
//        for (var i=0; i<revs.length; ++i) {
// 	   s += revs[i] + " ";
//        }
//        d +=  '<p>' + ids[j] + ": <b>Reversals:</b> " + s;

//    }

//    replaceContent("procedureparameterreversals",d);

// }




//////////////////////////////////////////////////////////////////////////////
// Confusion matrix plotting
//////////////////////////////////////////////////////////////////////////////

function plot_rtconfusionmatrix()
{
    var m=getConfusionMatrix(results.correctanswers, results.answers);
    showConfusionMatrixTable(m);
    rPlotConfusionMatrix(m);
    showSummary();
    if (typeof extraPlot == 'function') {
       extraPlot();
   }
}

function sortNumber(a,b)
{
    return a - b;
}


function getConfusionMatrix(stimulus,response)
{
    var panswers=response;
    var pcorrectanswers=stimulus;

    if (typeof(defaultx) != "undefined")
	pcorrectanswers=pcorrectanswers.concat(defaultx);

    if (typeof(defaulty) != "undefined")
	panswers=panswers.concat(defaulty);

    panswers=panswers.unique();                    // possible answers
    pcorrectanswers=pcorrectanswers.unique();    // possible correct answers

    if (panswers.length) {
       if (typeof panswers[0] == "number")
           panswers=panswers.sort(sortNumber);
       else
           panswers=panswers.sort();
   }

   if (pcorrectanswers.length) {
       if (typeof pcorrectanswers[0] == "number")
           pcorrectanswers=pcorrectanswers.sort(sortNumber);
       else
           pcorrectanswers=pcorrectanswers.sort();
   }

   var matrix = Array(pcorrectanswers.length);
   for (var i=0; i<pcorrectanswers.length; ++i) {
       matrix[i] = Array(panswers.length);
       for (var j=0; j<panswers.length; ++j) {
           matrix[i][j]=0;
       }
   }

   for (var i=0; i<response.length; ++i) {
       var ia = panswers.indexOf( response[i] );
       var ic = pcorrectanswers.indexOf( stimulus[i] );
       matrix[ic][ia] += 1;
   }

   var r = {stimulus: pcorrectanswers,
       response: panswers,
       matrix: matrix };

       return r;
   }

   function showConfusionMatrixTable(m) {
    var matrix=m.matrix;
    var pcorrectanswers=m.stimulus;
    var panswers=m.response;

    var table="<tr><th></th>";
    for (var i=0; i<m.stimulus.length; ++i)
	table+=wrap("th", m.stimulus[i]);
    for (i=0; i<m.response.length; ++i) {
       var row=wrap("th", m.response[i]);
       for (var j=0; j<m.stimulus.length; ++j) {
          row+=wrap("td", m.matrix[j][i]);
      }
      table+=wrap("tr", row);
  }
  table="<table border='1'>"+table+"</table>";


  if ($("#confusionmatrixtable")[0].firstChild !== null)
   $("#confusionmatrixtable > table").remove();
    $("#confusionmatrixtable").append(table);
}

function rPlotConfusionMatrix(m)
{
    var xlabel=m.stimulus;
    var ylabel=m.response;
    var matrix=m.matrix;

    $("#confusionmatrixplot").html("");

    var r=Raphael("confusionmatrixplot");

      var xs=[];
      var ys=[];
      var data=[];
      for (var x=0; x<matrix.length; ++x) {
       for (var y=0; y<matrix[x].length; ++y) {
         xs.push(x);
         ys.push(y);
         data.push( matrix[x][y] );
       }
     }

  //    r.txtattr.font = "11px 'Fontin Sans', Fontin-Sans, sans-serif";

  r.dotchart(10, 10, 620, 260, xs, ys, data,
   {symbol: "o", max: 10,
   heat: true,
   axis: "0 0 1 1",
   axisxstep: xlabel.length-1,
   axisystep: ylabel.length-1,
   axisxlabels: xlabel,
   axisxtype: " ",
   axisytype: " ",
   axisylabels: ylabel});
}


function showSummary()
{
    // calculate percentage correct
    var totalcorrect = 0;
    for (var i=0; i<results.results.length; ++i) {
        if (results.results[i])
            totalcorrect += 1;
    }

    var percentage = 100 * totalcorrect / results.results.length;

    var result = "<p><b>Total percentage correct:</b> " + percentage + " </p>";

    replaceContent("confusionmatrixsummary",  result);

}

//////////////////////////////////////////////////////////////////////////////
// Localisation experiment plotting
//////////////////////////////////////////////////////////////////////////////

function plot_rtlocalisation()
{
    var m=getConfusionMatrix(results.targetangles, results.answerangles);
    showConfusionMatrixTable(m);
    rPlotConfusionMatrix(m);
    showSummary();
    extraPlot_localisation();
}


function extraNewAnswer(xmlstring)
{
    if (results.answerangles == undefined)
        results.answerangles = new Array();
    if (results.targetangles == undefined)
        results.targetangles = new Array();

    var answerangle = parseFloat(results.answers[results.answers.length-1].match(/-?\d+/));
    var targetangle =  parseFloat(results.stimuli[results.stimuli.length-1].match(/-?\d+/));

    results.answerangles.push(answerangle);
    results.targetangles.push(targetangle);
}


function extraPlot_localisation()
{
    var rms=0;	    // RMS error
    var abs=0;	    // absolute error

    if (!results.answers.length)
	return;

    for (var i=0; i<results.answerangles.length; ++i) {
	rms += Math.pow( results.answerangles[i] - results.targetangles[i], 2);
	abs += Math.abs( results.answerangles[i] - results.targetangles[i]);
    }
    rms /= results.answerangles.length;
    rms = Math.sqrt(rms);
    abs /= results.answerangles.length;

    var table="<tr><td>RMS error</td><td>" + rms.toFixed(1) + "&deg;</td></tr>";
    table+="<tr><td>Absolute error</td><td>" + abs.toFixed(1) + "&deg;</td></tr>";
    table+="<tr><td>Trials</td><td>" + results.answers.length.toFixed(1) + "</td></tr>";
    table = wrap("table", table);

    if ($("#errormeasures")[0].firstChild !== null)
	$("#errormeasures > table").remove();
    $("#errormeasures").append(table);

}



//////////////////////////////////////////////////////////////////////////////
// Psignifit
//////////////////////////////////////////////////////////////////////////////



function doPsignifit ()
{
    // Prepare data structure for psignifit
    var d = [];

    var ntrials=results.trials.length;
    var x=[]; var y=[];
    for (var i=0; i<ntrials; ++i) {
	x.push(parseInt(results.trials[i].substr(-1,1)));
	y.push(parseFloat(results.answers[i]));

	d.push(x[i] + " " + y[i] + " " + "20");	    // 20 presentations
    }

    d.push("#shape logistic");
    d.push("#n_intervals 2");
    d.push("#runs 99" );

    print("typeof(psignifit) = " + typeof(psignifit));


    var s = d.join("\n");

    var r = psignifit.psignifit(s); // Call APEX/psignifit function

    replaceContent("psignifit",
	    "<p>Thresholds: " + r.thresholds +
	    "<p>Slopes: " + r.slopes
	    );


    //plotProcedureParameter(y);
}
