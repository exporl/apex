function spinboxprocedure()
{
    presentedLevel = parseFloat(params["startCurrentLevel"]);
}

spinboxprocedure.prototype = new ScriptProcedureInterface();

spinboxprocedure.prototype.processResult = function(screenresult)
{
    this.button = screenresult["spinbox"];
    return new ResultHighlight;
}

spinboxprocedure.prototype.setupNextTrial = function ()
{

if (this.button) {
        presentedLevel = this.button;

        presentedLevel = presentedLevel > 255 ? 255 : presentedLevel < 0 ? 0 : presentedLevel;
        //console.log("Next gain: " + gain);
    }

    var t = new Trial();
    t.addScreen("screen", true, 0);
    t.addStimulus("electricstim" + presentedLevel, {}, '', 0);
    t.setId("trial");

    console.log("Returning trial");
    return t;
}

spinboxprocedure.prototype.firstScreen = function()
{
    trials = api.trials();
    return trials[0].screen;
}

spinboxprocedure.prototype.resultXml = function()
{
     return xml.tag("procedure",
                {"type": "cifittingrocedure"},
                xml.tag("parameters",
                    xml.tag("parameter",
                        {"name": params["parameter"]},
                        presentedLevel)),
                    xml.tag("stimulus",
                        "electricstim"+presentedLevel)
            );
}

spinboxprocedure.prototype.progress = function()
{
    return 0;
}

spinboxprocedure.prototype.checkParameters = function()
{
    console.log("TestProcedure CheckParameters called");

    if (params['startCurrentLevel'] === null)
        return "spinboxprocedure: parameter startvalue not found";

    return "";
}

function getProcedure()
{
    return new spinboxprocedure();
}