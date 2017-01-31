function plot()
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
    reversals.push(parametervalues[parametervalues.length-1]);

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
