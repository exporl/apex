/* global xml, params */
/* jshint forin:true, noarg:true, noempty:true, eqeqeq:true, undef:true, curly:true, devel:true, maxerr:50 */

function dacspowerup(r)
{
    return xml.tag("stimulus", {"repeat" : r},
            xml.tag("parameters",
                xml.tag("pw", 12.0),
                xml.tag("pg", 6.0),
                xml.tag("ae", 0),
                xml.tag("re", -1),
                xml.tag("cl", 0),
                xml.tag("t", 51.0)));
}

// needs at least one powerup pulse before
function dacsstim(r, amplitude)
{
    return xml.tag("stimulus", {"repeat" : r},
            xml.tag("parameters",
                xml.tag("ae", (amplitude & 0xf80) >> 7),
                xml.tag("cl", (amplitude & 0x07f) << 1)));
}

function ciseq(pulses, r)
{
    var i,
        result = "";
    for (i = 0; i < r; i += 1) {
        result += pulses;
    }
    return result;
    // Not possible since repeat attributes are ignored on sequences
    // return xml.tag("sequence", {"repeat" : r}, pulses);
}

function cinicseq(pulses)
{
    return xml.tag("nic:sequence", {
        "xmlns:nic" : "http://www.cochlear.com/nic",
        "xmlns:xsi" : "http://www.w3.org/2001/XMLSchema-instance",
        "xsi:schemaLocation" :
            "http://www.cochlear.com/nic nic-stimuli.xsd"},
        pulses);
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
        sequence = dacspowerup(10000) + ciseq
            (dacsstim(clickFrames, ulaw) +
             dacsstim(silenceFrames, 0) +
             dacsstim(clickFrames, (params.alternatingPolarity ? -1 : 1) * ulaw) +
             dacsstim(silenceFrames, 0),
             pulseCount);

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
