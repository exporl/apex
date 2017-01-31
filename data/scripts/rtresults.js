answers=new Array();
correctanswers=new Array();
parametervalues=new Array();

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
	parametervalues.push( parseFloat(paramelement.text()) );
    }

    answers.push(answer);
    correctanswers.push(correctanswer);
}


