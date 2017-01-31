function hrtfProcedure()
{
}

hrtfProcedure.prototype = new pluginProcedureInterface();

hrtfProcedure.prototype.NextTrial = function (answer, screenresult)
{
    angle = parseInt (api.GetParameter ("angle"));
    angle = (angle + 360 +
         parseInt (screenresult["buttongroup"].substring (5))) % 360;
    api.SetParameter ("angle", angle);
    api.CreateOutputList ("trial", "stimulus");
    api.SetScreen ("screen");

    return "trial";
}


hrtfProcedure.prototype.FirstTrial = function()
{
    return this.NextTrial (0, { buttongroup: "delta0" });
}

hrtfProcedure.prototype.GetResultXML = function()
{
    return "";
}

return new hrtfProcedure();
