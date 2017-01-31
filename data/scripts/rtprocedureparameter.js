function plot()
{
    plotProcedureParameter(parametervalues);
    tableProcedureParameter(parametervalues);
    lastvalueProcedureParameter(parametervalues);
    meanrevsProcedureParameter(parametervalues);
}

                                            
function tableProcedureParameter(parametervalues)
{
    var table=wrap("tr", "<th>Trial</th><th>Answer</th><th>Parameter</th>");
    for (var i=0; i<answers.length; ++i)
    	table+=wrap("tr", 
    		wrap("td", i+1)+
		wrap("td", answers[i])+
		wrap("td", parametervalues[i])
		);
    table=wrap("table", table);

   if ($("#procedureparametertable")[0].firstChild !== null)
	$("#procedureparametertable > table").remove();
    $("#procedureparametertable").append(table);
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
    if ( $("#procedureparameterlastvalue").length == 0 ) { 
    	print("----------- procedureparameterlastvalue not found");
    	return;
    }
    if ( $("#procedureparameterlastvalue")[0].firstChild !== null)
	 $("#procedureparameterlastvalue >*").remove();

    $("#procedureparameterlastvalue").append('<p><b>Last value:</b> ' + parametervalues[parametervalues.length-1] + '</p>');
}


function meanrevsProcedureParameter(parametervalues)
{
    // Check if reversalsForMean is defined
    var rfm = 6;
    if (typeof(reversalsForMean) != "undefined")
    	rfm = reversalsForMean;

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

    var nrevs = Math.min(rfm, reversals.length);

    // Calculate average
    var sum = 0;
    for (var i=reversals.length-nrevs; i<reversals.length; ++i) {
	sum += reversals[i];
    }
    var value = sum/nrevs;

    if ( $("#procedureparametermeanrevs").length == 0 ) { 
    	print("----------- procedureparametermeanrevs not found");
    	return;
    }
    if ( $("#procedureparametermeanrevs")[0].firstChild !== null)
	 $("#procedureparametermeanrevs >*").remove();

    $("#procedureparametermeanrevs").append('<p><b>Mean of last ' + nrevs + ' reversals:</b> ' + value + '</p>');
}


