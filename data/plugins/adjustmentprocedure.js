function gainProcedure()
{
    gain = 0;
    stim = "stimulus1";
}

gainProcedure.prototype = new pluginProcedureInterface();

gainProcedure.prototype.NextTrial = function (answer, screenresult)
{
    result = screenresult["buttongroup"];
    if (result == "stim1")
    	stim = "stimulus1";
    else if (result == "stim2")
    	stim = "stimulus2";
    else if (result == "plus")
    	gain += 5;
    else if (result == "minus")
    	gain -= 5;
    else if (result == "finish")
    	return "";

    api.CreateOutputList ("trial", stim);
    api.SetStimulusParameter (stim, "gain", gain);
    api.SetScreen ("screen");

    return "trial";
}


gainProcedure.prototype.FirstTrial = function() 
{
    return this.NextTrial (0, { buttongroup: "dummy" });
}

gainProcedure.prototype.GetResultXML = function() 
{
    return "";
}

new gainProcedure();
