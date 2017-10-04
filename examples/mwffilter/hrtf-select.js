function hrtfProcedure()
{
}

hrtfProcedure.prototype = new pluginProcedureInterface();

hrtfProcedure.prototype.NextTrial = function (answer, screenresult)
{
    api.SetParameter ("angle", screenresult["buttongroup"].substring (5));
    api.CreateOutputList ("trial", "stimulus");
    api.SetScreen ("screen");

    return "trial";
}


hrtfProcedure.prototype.FirstTrial = function()
{
    return this.NextTrial (0, { buttongroup: "angle0" });
}

hrtfProcedure.prototype.GetResultXML = function()
{
    return "";
}

return new hrtfProcedure();
