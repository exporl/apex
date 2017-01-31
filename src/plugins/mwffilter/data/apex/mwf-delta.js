function hrtfProcedure()
{
}

hrtfProcedure.prototype = new pluginProcedureInterface();

hrtfProcedure.prototype.NextTrial = function (answer, screenresult)
{
    snr = parseFloat (api.GetParameter ("snr-left"));
    if (screenresult["buttongroup"] == "up")
        snr += 2;
    else if (screenresult["buttongroup"] == "down")
        snr -= 2;
    else if (screenresult["buttongroup"] == "uup")
        snr += 10;
    else if (screenresult["buttongroup"] == "ddown")
        snr -= 10;
    if (snr > 20) 
        snr = 20;
    if (snr < -30)
        snr = -30;
    api.SetParameter ("snr-left", snr);
    api.SetParameter ("snr-right", snr);
    api.SetParameter ("signal-gain", snr);

    onoff = api.GetParameter ("disabled-left");
    if (screenresult["buttongroup"] == "onoff")
        onoff = onoff == "false" ? "true" : "false";
    api.SetParameter ("disabled-left", onoff);
    api.SetParameter ("disabled-right", onoff);

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
