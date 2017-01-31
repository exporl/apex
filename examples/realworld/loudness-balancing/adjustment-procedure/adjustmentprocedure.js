function adjustmentProcedure()
{
    gain = parseFloat(params["startvalue"]);
    screen = params["screen"]
    stim = "stimulus";
    hasMax = !(params["maxvalue"] === null);
    this.button='';
    if (hasMax) {
        maxValue = parseFloat(params["maxvalue"]);
    }
}

adjustmentProcedure.prototype = new ScriptProcedureInterface();


adjustmentProcedure.prototype.processResult = function(screenresult)
{
    this.button = screenresult["buttongroup"];

    return new ResultHighlight;

}



adjustmentProcedure.prototype.setupNextTrial = function ()
{
    if (this.button) {
        var m=this.button.match( /button_(.*)/);
        if (m===null)
            api.Abort("Could not parse button");
        var r=m[1];

        if (r=="repeat") {
            // NOP
        } else if (r=="done") {
            return "";
        } else {
            if ( ! r.match(/^-?\d+$/ ) )
                api.Abort("Could not parse button" + button + " , should be button_<stepsize>");
            var stepsize=parseFloat(r);
            console.log("Stepsize: " + stepsize);

            if (hasMax &&  gain+stepsize > maxValue) {
                gain=maxValue;
            } else {
                console.log("Adjusting gain");
                gain=gain+stepsize;
            }

            console.log("Next gain: " + gain);
        }
    }

    /*
    api.CreateOutputList ("trial", stim);
    api.SetParameter (params["parameter" ], gain);
    api.SetScreen (screen);
    */

    var t = new Trial();
    t.addScreen(screen, true, 0);
    var parameters = {};
    parameters[params["parameter"]] = gain;
    t.addStimulus(stim, parameters, "", 0);
    t.setId("trial");

    console.log("Returning trial");
    return t;
}


adjustmentProcedure.prototype.firstScreen = function()
{
    return screen;
}

adjustmentProcedure.prototype.resultXml = function()
{
     return xml.tag("procedure",
                {"type": "adjustmentProcedure"},
                    xml.tag("parameter",
                        {"id": params["parameter"]},
                        gain)
            );
}

adjustmentProcedure.prototype.progress = function()
{
    return 0;
}

adjustmentProcedure.prototype.checkParameters = function()
{
    console.log("TestProcedure CheckParameters called");

    if (params['parameter'] === null)
        return "adjustmentProcedure: parameter not found";
    if (params['startvalue'] === null)
        return "adjustmentProcedure: parameter startvalue not found";
    if (params['screen'] === null)
        return "adjustmentProcedure: parameter screen not found";

    return "";
}

function getProcedure()
{
    return new adjustmentProcedure();
}
