function hrtfProcedure()
{
}

hrtfProcedure.prototype = new pluginProcedureInterface();

hrtfProcedure.prototype.NextTrial = function (answer, screenresult)
{
    gain = parseFloat (api.GetParameter ("signal-gain"));
    if (screenresult["buttongroup"] == "up")
        gain += 2;
    else if (screenresult["buttongroup"] == "down")
        gain -= 2;
    else if (screenresult["buttongroup"] == "uup")
        gain += 10;
    else if (screenresult["buttongroup"] == "ddown")
        gain -= 10;
    if (gain > 20)
        gain = 20;
    if (gain < -30)
        gain = -30;
    api.SetParameter ("signal-gain", gain);

    onoff = api.GetParameter ("disabled");
    if (screenresult["buttongroup"] == "onoff")
        onoff = onoff == "false" ? "true" : "false";
    api.SetParameter ("disabled", onoff);

    api.CreateOutputList ("trial", "stimulus");
    api.SetScreen ("screen");

    return "trial";
}


hrtfProcedure.prototype.FirstTrial = function()
{
    return this.NextTrial (0, { buttongroup: "dummy" });
}

hrtfProcedure.prototype.GetResultXML = function()
{
    return "";
}

return new hrtfProcedure();
