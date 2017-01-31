"use strict";

function extraPlot ()
{
    doPsignifit();
}


function doPsignifit ()
{
    debugger;
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
    //var r = "";

    replaceContent("psignifit", 
    	    "<p>Thresholds: " + r.thresholds +
    	    "<p>Slopes: " + r.slopes  
    	    );


    plotProcedureParameter(y);
}
