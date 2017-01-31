function plot()
{
    m=getConfusionMatrix(results.correctanswers, results.answers);
    showConfusionMatrixTable(m);
    rPlotConfusionMatrix(m);
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
    for (i=0; i<pcorrectanswers.length; ++i) {
	matrix[i] = Array(panswers.length);
	for (j=0; j<panswers.length; ++j) {
	    matrix[i][j]=0;
	}
    }

    for (i=0; i<results.answers.length; ++i) {
	var ia = panswers.indexOf( results.answers[i] );
	var ic = pcorrectanswers.indexOf( results.correctanswers[i] );
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
    xlabel=m.stimulus;
    ylabel=m.response;
    matrix=m.matrix;

    if ( $("#confusionmatrixplot")[0].firstChild !== null)
        r.remove();
    r=Raphael("confusionmatrixplot");

    var xs=[];
    var ys=[];
    var data=[];
    for (x=0; x<matrix.length; ++x) {
	for (y=0; y<matrix[x].length; ++y) {
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


