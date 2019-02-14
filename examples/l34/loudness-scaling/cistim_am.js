/* global xml, params */
/* exported getDatablocks, getStimuli, getTrials */
/* jshint forin:true, noarg:true, noempty:true, eqeqeq:true, undef:true, curly:true, devel:true, maxerr:50 */

// needs at least one powerup pulse before

function cisinglestim(phase_width, phase_gap,
        active_electrode, reference_electrode, current_level, period)
{
    return xml.tag("stimulus", {type: "biphasic"},
        xml.tag("parameter", {name: "trigger"}, false),
        xml.tag("parameter", {name: "phasewidth"}, phase_width),
        xml.tag("parameter", {name: "phasegap"}, phase_gap),
        xml.tag("parameter", {name: "active"}, active_electrode),
        xml.tag("parameter", {name: "reference"}, reference_electrode),
        xml.tag("parameter", {name: "currentlevel"}, current_level),
        xml.tag("parameter", {name: "period"}, period));
}

function ciseq(r, pulses)
{
    return xml.tag("sequence", {"repeats" : r}, pulses);
}

function cinicseq(pulses)
{
    return xml.tag("cisequence", {"version" : "1.0"}, ciseq(1, pulses));
}

function cl2i_cic3(cl)
{
    return 10e-6 * Math.exp(cl * Math.log(175) / 255);
}

function i2cl_cic3(i)
{
    return 255 / Math.log(175) * Math.log(i / 10e-6);
}

function cl2i_cic4(cl)
{
    return 17.5e-6 * Math.pow(100, (cl / 255));
}

function i2cl_cic4(i)
{
    return 255 / Math.log(100) * Math.log(i / 17.5e-6);
}

function cl2i(cl, implantType)
{
    if (implantType == "cic3")
        return cl2i_cic3(cl);
    if (implantType == "cic4")
        return cl2i_cic4(cl);
    throw new Error("Unknown implant type");
}

function i2cl(i, implantType)
{
    if (implantType == "cic3")
        return i2cl_cic3(i);
    if (implantType == "cic4")
        return i2cl_cic4(i);
    throw new Error("Unknown implant type");
}

function db(id, currentLevel)
{
    var phaseWidth = Number(params["phase_width"]),
        phaseGap = Number(params["phase_gap"]),
        activeElectrode = Number(params["active_electrode"]),
        referenceElectrode = Number(params["reference_electrode"]),
        minimumCurrentLevel = Number(params["minimum_currentlevel"]),
        carrierFrequency = Number(params["carrier_frequency"]),
        modulationFrequency = Number(params["modulation_frequency"]),
        period = Math.round(5e6 / carrierFrequency) / 5,
        alpha = Number(params["currentlevel_angle"]) * Math.PI / 180,
        omega = 2 * Math.PI * modulationFrequency,
        pulseCount = Math.round(Number(params["duration"]) * carrierFrequency),
        t = 1 / carrierFrequency,
        implantType = params["implant_type"],
        minimumCurrent = cl2i(minimumCurrentLevel, implantType),
        maximumCurrent = cl2i(currentLevel, implantType),
        meanCurrent = (minimumCurrent + maximumCurrent) / 2,
        modulationIndex = (maximumCurrent - minimumCurrent) / (maximumCurrent + minimumCurrent),
        sequence = new Array(pulseCount);

    for (var i = 0; i < pulseCount; ++i) {
        var cl = Math.round(i2cl(meanCurrent * (1 + modulationIndex * Math.sin(alpha + omega * i * t)), implantType));
        sequence[i] = cisinglestim(phaseWidth, phaseGap, activeElectrode, referenceElectrode, cl, period);
    }

    return xml.datablock(id, "l34", "", cinicseq(sequence.join("")));
}


function getDatablocks()
{
    var minimumCurrentLevel = Number(params["minimum_currentlevel"]),
        result = new Array(256 - minimumCurrentLevel);
    for (var i = minimumCurrentLevel; i < 256; i += 1) {
        result[i - minimumCurrentLevel] = db("electricdb" + i, i);
    }
    return result.join("");
}

function getStimuli() {
    var minimumCurrentLevel = Number(params["minimum_currentlevel"]),
        result = new Array(256 - minimumCurrentLevel);
    for (var i = minimumCurrentLevel; i < 256; i += 1) {
        result[i - minimumCurrentLevel] = xml.stimulus("electricstim" + i, "electricdb" + i, "currentLevel", i);
    }
    return result.join("");
}

function getTrials()
{
    var minimumCurrentLevel = Number(params["minimum_currentlevel"]),
        stims = new Array(256 - minimumCurrentLevel);
    for (var i = minimumCurrentLevel; i < 256; i += 1) {
        stims[i - minimumCurrentLevel] = xml.tag("stimulus", {"id" : "electricstim" + i});
    }
    return xml.trial("trial", "screen", stims.join(""));
}
