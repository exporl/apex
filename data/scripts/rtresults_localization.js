"use strict";

var results = {};
results.answers=new Array();
results.correctanswers=new Array();
results.parametervalues=new Array();
results.xml=new Array();
results.trials=new Array();
results.stimuli=new Array();

var defaultx=new Array();
var defaulty=new Array();

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

function newAnswer(xmlstring)
{
/*    t=document.getElementById("result");
    t.firstChild.nodeValue="trial: " + trial + ", answer: " + answer + ", correct: " + correct;*/

    var xml = (new DOMParser()).parseFromString(xmlstring, "application/xml");
    var parameter = $(xml).find("parameter").text();
    var answer = $(xml).find("corrector > answer").text();
    var result = $(xml).find("corrector > result").text();
    var correctanswer = $(xml).find("corrector > correctanswer").text();

    var paramelement = $(xml).find("procedure > parameter");
    if (paramelement !== null) {
	results.parametervalues.push( parseFloat(paramelement.text()) );
    }

    answer = parseFloat(answer.match(/-?\d+/));
    correctanswer =  parseFloat(correctanswer.match(/-?\d+/));

    results.answers.push(answer);
    results.correctanswers.push(correctanswer);
}

function extraPlot()
{
    var rms=0;	    // RMS error
    var abs=0;	    // absolute error

    if (!results.answers.length)
    	return;

    for (var i=0; i<results.answers.length; ++i) {
	rms += Math.pow( results.answers[i] - results.correctanswers[i], 2);
	abs += Math.abs( results.answers[i] - results.correctanswers[i]);
    }
    rms /= results.answers.length;
    rms = Math.sqrt(rms);
    abs /= results.answers.length;

    var table="<tr><td>RMS error</td><td>" + rms.toFixed(1) + "</td></tr>";
    table+="<tr><td>Absolute error</td><td>" + abs.toFixed(1) + "</td></tr>";
    table+="<tr><td>Trials</td><td>" + results.answers.length.toFixed(1) + "</td></tr>";
    table = wrap("table", table);

    if ($("#errormeasures")[0].firstChild !== null)
	$("#errormeasures > table").remove();
    $("#errormeasures").append(table);

}
