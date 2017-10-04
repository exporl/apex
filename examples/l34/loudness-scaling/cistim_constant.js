/* global xml, params */
/* jshint forin:true, noarg:true, noempty:true, eqeqeq:true, undef:true, curly:true, devel:true, maxerr:50 */

// needs at least one powerup pulse before

function cistim(r, phase_width, phase_gap,
        active_electrode, reference_electrode, current_level, period)
{
    return ciseq(r, xml.tag("stimulus", {type: "biphasic"},
                xml.tag("parameter", {name: "trigger"}, false),
                xml.tag("parameter", {name: "phasewidth"}, phase_width),
                xml.tag("parameter", {name: "phasegap"}, phase_gap),
                xml.tag("parameter", {name: "active"}, active_electrode),
                xml.tag("parameter", {name: "reference"}, reference_electrode),
                xml.tag("parameter", {name: "currentlevel"}, current_level),
                xml.tag("parameter", {name: "period"}, period)));
}

function ciseq(r, pulses)
{
    return xml.tag("sequence", {"repeats" : r}, pulses);
}

function cinicseq(pulses)
{
    return xml.tag("cisequence", {"version" : "1.0"}, ciseq(1, pulses));
}

function db(id, currentLevel)
{
    if (params["alternating_polarity"]) {
        error('Do not define alternating polarity');
    }
    var sequence,
        carrierFrequency = +params["carrier_frequency"],
        period = 1e6 / carrierFrequency,
        pulseCount = Math.round(+params["duration"] * carrierFrequency);

    sequence = cistim(pulseCount,
            +params["phase_width"], +params["phase_gap"],
            +params["active_electrode"], +params["reference_electrode"],
            currentLevel, period);

    return xml.datablock(id, "l34", '', cinicseq(sequence));
}


function getDatablocks()
{
    var i,
        result = "";
    for (i = 0; i < 256; i += 1) {
        result += db("electricdb" + i, i);
    }
    return result;
}

function getStimuli() {
    var i,
        result = "";
    for (i = 0; i < 256; i += 1) {
        result += xml.stimulus("electricstim" + i, "electricdb" + i, "currentLevel", i);
    }
    return result;
}

function getTrials()
{
    var i,
        stims = "";
    for (i = 0; i < 256; i += 1) {
        stims += xml.tag("stimulus", {"id" : "electricstim" + i});
    }
    return xml.trial("trial", "screen", stims);
}
