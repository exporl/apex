function plot() {
    parseAnswers();
    loudnessTable();
    loudnessPlot();
    avgPlot();
    if (typeof extraPlot == 'function') {
        extraPlot();
    }
}

function parseAnswers() {
    results.answers = [];
    for (var i = 0; i < results.xml.length; ++i) {
        results.answers.push(parseFloat(
            eval($(results.xml[i]).find("answer").text())));
    }
    maxVal = Math.max.apply(null, results.answers);
}

function loudnessTable(parametervalues) {
    var table = wrap("tr", "<th>Trial</th><th>Stimuli</th><th>Response</th>");
    for (var i = 0; i < results.answers.length; ++i)
        table += wrap("tr",
            wrap("td", i + 1) +
            wrap("td", results.trials[i]) +
            wrap("td", results.answers[i])
        );
    table = wrap("table", table);

    replaceContent("loudnesstable", table);
}


function loudnessPlot(parametervalues) {
    var test = $("#loudnessplot")[0];
    if ($("#loudnessplot")[0].firstChild !== null)
        $("#loudnessplot >*").remove();

    var d = new Array();
    for (var i = 0; i < results.trials.length; ++i) {
        var intensity = parseTrial(results.trials[i]);
        d.push([intensity, 1 - results.answers[i]]);
    }
    $.jqplot("loudnessplot", [d], {
        axes: {
            xaxis: {
                label: 'Intensity (dB)',
                autoscale: true,
                labelOptions: {
                    enableFontSupport: true,
                    fontFamily: 'Georgia',
                    fontSize: '12pt'
                }
            },
            yaxis: {
                label: 'Loudness rating',
                autoscale: true,
                labelOptions: {
                    enableFontSupport: true,
                    fontFamily: 'Georgia',
                    fontSize: '12pt'
                }

            },
            series: [{
                showLine: false,
                markerOptions: {
                    size: 7,
                    style: "x"
                }
            }]
        }
    });
}

function parseTrial(id) {
    var re = /-?\d+$/;
    return parseFloat(re.exec(id)) + 65;
}

function avgPlot() {
    var bins = new Array();
    for (var c = 0; c < nCategories; ++c) {
        bins[c] = [];
    }

    for (var i = 0; i < results.trials.length; ++i) {
        bins[getscale(results.answers[i])].push(parseTrial(results.trials[i]));
    }

    var meanbins = new Array();
    for (var c = 0; c < nCategories; ++c) {
        meanbins[c] = 0;
        for (var i = 0; i < bins[c].length; ++i)
            meanbins[c] += bins[c][i];
        meanbins[c] /= bins[c].length;
    }

    var table = wrap("tr", "<th>Category</th><th>Intensity</th>");
    for (var c = 0; c < meanbins.length; ++c)
        table += wrap("tr",
            wrap("td", c + 1) +
            wrap("td", meanbins[c])
        );
    table = wrap("table", table);

    replaceContent("avgplot", table);
}

function getscale(val) {
    return Math.floor((val/(maxVal+1)*nCategories));
}
