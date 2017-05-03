/* global xml, params */
/* jshint forin:true, noarg:true, noempty:true, eqeqeq:true, undef:true, curly:true, devel:true, maxerr:50 */

// needs at least one powerup pulse before
function dacsstim(r, amplitude)
{
    return ciseq(r, xml.tag("stimulus", {type: "codacs"},
                xml.tag("parameter", {name: "trigger"}, false),
                xml.tag("parameter", {name: "period"}, 51.0),
                xml.tag("parameter", {name: "amplitude"}, amplitude)));
}

function ciseq(r, pulses)
{
    return xml.tag("sequence", {"repeats" : r}, pulses);
}

function cinicseq(pulses)
{
    return xml.tag("cisequence", ciseq(1, pulses));
}

function db(id, level)
{
    var frameRate = Math.round(1e6 / 51.0),
        frequency = +params.stimulationFrequency,
        clickFrames = Math.round(+params.clickWidth / 1e6 * frameRate),
        silenceFrames = Math.round(frameRate / frequency) - clickFrames,
        pulseCount = Math.round(+params.duration / 2 * frequency),
        powerupDuration = 0.5,
        powerupCount = Math.round(powerupDuration * frameRate),
        amplitude = Math.round(32767 * Math.pow(10.0, level / 20.0)),
        ulaw = Math.floor(Math.min(2047,
                amplitude <  2048 ? ((amplitude -     0) /  2) +    0 :
                amplitude <  4096 ? ((amplitude -  2048) /  8) + 1024 :
                amplitude <  8192 ? ((amplitude -  4096) / 16) + 1280 :
                amplitude < 16384 ? ((amplitude -  8192) / 32) + 1536 :
                                    ((amplitude - 16384) / 64) + 1792)),
        sequence = dacsstim(10000, 0) + ciseq(pulseCount,
                dacsstim(clickFrames, ulaw) +
                dacsstim(silenceFrames, 0) +
                dacsstim(clickFrames, (params.alternatingPolarity ? -1 : 1) * ulaw) +
                dacsstim(silenceFrames, 0));

    return xml.datablock(id, "l34", '', cinicseq(sequence));
}

function getDatablocks()
{
    var i,
        result = "";
    for (i = 0; i < 91; i += 1) {
        result += db("electricdb" + i, -i);
    }
    return result;
}

function getStimuli()
{
    var i,
        result = "";
    for (i = 0; i < 91; i += 1) {
        result += xml.stimulus("electricstim" + i, "electricdb" + i, "level", -i);
    }
    return result;
}

function getTrials()
{
    var i,
        stims = "";
    for (i = 0; i < 91; i += 1) {
        stims += xml.tag("stimulus", {"id" : "electricstim" + i});
    }
    return xml.trial("trial", "screen", stims);
}
