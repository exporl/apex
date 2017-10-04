function hrtfProcedure()
{
}

hrtfProcedure.prototype = new pluginProcedureInterface();

hrtfProcedure.prototype.NextTrial = function (answer, screenresult)
{
    wienerLength = parseInt (api.GetParameter ("wiener-length"));
    if (screenresult["buttongroup"] == "lengthup")
        wienerLength *= 2;
    else if (screenresult["buttongroup"] == "lengthdown")
        wienerLength /= 2;
    if (wienerLength < 4)
        wienerLength = 4;
    if (wienerLength > 512)
        wienerLength = 512;
    api.SetParameter ("wiener-length", wienerLength);

    offset = parseInt (api.GetParameter ("signal-offset"));
    if (screenresult["buttongroup"] == "offsetup")
        offset += 2;
    else if (screenresult["buttongroup"] == "offsetdown")
        offset -= 2;
    api.SetParameter ("signal-offset", offset);

    angle = parseInt (api.GetParameter ("signal-angle"));
    if (screenresult["buttongroup"].indexOf ("signal") == 0)
        angle = (angle + 360 +
            parseInt (screenresult["buttongroup"].substring (6))) % 360;
    api.SetParameter ("signal-angle", angle);

    angle = parseInt (api.GetParameter ("noise-angle"));
    if (screenresult["buttongroup"].indexOf ("noise") == 0)
        angle = (angle + 360 +
            parseInt (screenresult["buttongroup"].substring (5))) % 360;
    api.SetParameter ("noise-angle", angle);

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

    onoff = api.GetParameter ("wiener-disabled");
    if (screenresult["buttongroup"] == "onoff")
        onoff = onoff == "false" ? "true" : "false";
    api.SetParameter ("wiener-disabled", onoff);

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
