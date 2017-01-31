// TODO:
// make sure path ends in /
// check for undefined parameters

function getStimuli() {
    misc.checkParams(Array("level_min", "level_max", "level_step", "datablocks"));

    var datablocks = eval(params["datablocks"]);

    var r = "";
    for (db = 0; db < datablocks.length; ++db) {
        var datablock = datablocks[db];

        for (var i = parseFloat(params["level_min"]); i <= parseFloat(params["level_max"]); i += parseFloat(params["level_step"])) {

            r = r + xml.tag("stimulus", {
                    "id": "stim_" + datablock + "_" + i,
                },
                xml.tag("datablocks",
                    xml.tag("datablock", {
                        "id": datablock
                    })),
                xml.tag("variableParameters",
                    xml.tag("parameter", {
                        "id": "gain"
                    }, i))
            );
        }
    }

    return r;
}



function getTrials() {
    misc.checkParams(Array("level_min", "level_max", "level_start", "level_max_diff", "repetitions", "level_step", "datablocks"));

    var datablocks = eval(params["datablocks"]);
    var threshold = parseFloat(params["level_max_diff"]);

    // Create pseudorandom list
    var single = new Array();
    var count = 0;
    for (var i = parseFloat(params["level_min"]); i <= parseFloat(params["level_max"]); i += parseFloat(params["level_step"])) {
        single[count] = i;
        ++count;
    }
    var full = new Array();
    for (var r = 0; r < parseFloat(params["repetitions"]); ++r) {
        full = full.concat(single);
    }
    while (1) {
        var from = full.slice();
        var to = new Array();
        to[0] = parseFloat(params["level_start"]);
        //var index = Math.floor(Math.random()*from.length);
        //      debugger;
        var index = Math.floor((parseFloat(params["level_start"]) - parseFloat(params["level_min"])) / parseFloat(params["level_step"]));
        to[0] = from[index];
        from.splice(index, 1);
        for (i = 1; i < full.length; ++i) {
            // check if appropriate element exists
            var found = 0;
            for (var j = 0; j < from.length; ++j) {
                if (Math.abs(from[j] - to[i - 1]) < threshold) {
                    found = 1;
                    break;
                }
            }
            if (!found)
                break;

            while (1) {
                index = Math.floor(Math.random() * from.length);
                if (Math.abs(from[index] - to[i - 1]) < threshold) {
                    to[i] = from[index];
                    from.splice(index, 1);
                    break;
                }
            }
        }
        if (i == full.length)
            break;
    }

    //    debugger;

    var r = "";
    for (db = 0; db < datablocks.length; ++db) {
        var datablock = datablocks[db];

        for (i = 0; i < to.length; ++i) {
            r = r + xml.tag("trial", {
                    "id": "trail_" + datablock + "-" + i + "_" + to[i],
                },
                xml.tag("screen", {
                    "id": "screen"
                }),
                xml.tag("stimulus", {
                    "id": "stim_" + datablock + "_" + to[i]
                })
            );
        }
    }


    return r;
}
