function plot()
{
    var num;
    var i;
    var parametervalues;

    parseProcedureId();

    // get unique procedure ids:
    procedureIds = $.grep(results.procedureId, function(v, k){
        return $.inArray(v ,results.procedureId) === k;
    });

    // Create 2D parameter list:
    parametervalues = new Array(procedureIds.length);
    for (i=0; i<procedureIds.length; ++i) {
        parametervalues[i] = new Array();
    }

    // Put each parameter value at correct spot
    for (i=0; i<results.procedureId.length; ++i) {
        num = jQuery.inArray( results.procedureId[i], procedureIds );
        parametervalues[num].push( results.parametervalues[i] );
    }

    plotProcedureParameter(parametervalues, procedureIds);
    tableProcedureParameter(results.parametervalues);
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



function tableProcedureParameter(parametervalues)
{
    var table=wrap("tr", "<th>Trial</th><th>Procedure</th><th>Answer</th><th>Parameter</th>");
    for (var i=0; i<results.answers.length; ++i)
        table+=wrap("tr",
                wrap("td", i+1)+
                wrap("td", results.procedureId[i])+
                wrap("td", results.answers[i])+
                wrap("td", parametervalues[i])
                );
    table=wrap("table", table);

    replaceContent("procedureparametertable", table);
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
//              labelRenderer: $.jqplot.CanvasAxisLabelRenderer,
                labelOptions: {
                  enableFontSupport: true,
                  fontFamily: 'Georgia',
                  fontSize: '12pt'
                }
              },
             yaxis:{
                label:'Parameter value',
                autoscale: true,
//              labelRenderer: $.jqplot.CanvasAxisLabelRenderer,
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
    var rfm = 12;
    if (typeof(trialsForMean) != "undefined")
        rfm = trialsForMean;

    var d = '';

    var averages = new Array();
    for (var j=0; j<ids.length; ++j) {

        var nvalues = Math.min(rfm, parametervalues[j].length);

        // Calculate average
        var sum = 0;
        for (var i=parametervalues[j].length-nvalues; i<parametervalues[j].length; ++i) {
            sum += parametervalues[j][i];
        }
        var average = sum/nvalues;
        averages[j] = average;

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


        // Jonas: The difference of the mean should be outside the for loop of line 214
        // Callculate differences between the averages
    var diffMean = Math.abs(averages[0] - averages[1]);
          d+='<p> :diff between averages:' + diffMean + '<p>';
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
