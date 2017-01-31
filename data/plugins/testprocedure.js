function pluginProcedure() {        // Constructor
    ntrial=0;   
}

pluginProcedure.prototype=new pluginProcedureInterface();

pluginProcedure.prototype.NextTrial = function(answer, screenresult) {
    print("TestProcedure NextTrial called");   
    
    api.SetProgress(++ntrial);
    
    var targettrial;
    var targetstimulus;
    var targetscreen;
    
    // Get first trial from api
    trials = api.GetTrials();
    for (t in trials) {
        if (t==0) {
            targettrial = trials[t].id;
            targetstimulus = trials[t].GetStimulus();
            targetscreen = trials[t].screen;
            break;
        }
    }
    
    print("Testprocedure: last answer to buttongroup1: " + screenresult["buttongroup1"]);
    
    print("Testprocedure: using stimulus " + targetstimulus);
    print("Testprocedure: using trial " + targettrial);
    print("Testprocedure: using screen " + targetscreen);
    
    if (ntrial-1==params.presentations)
        api.Finished();
    
    api.SetParameter("gain", -10+ntrial);
    
    api.CreateOutputList(targettrial, targetstimulus);
    api.SetScreen(targetscreen);
    return "trial1";
}


pluginProcedure.prototype.FirstTrial = function() {
    print("TestProcedure FirstTrial called");   
    
    api.SetNumTrials(params.presentations);
    
    //api.MessageBox("PluginProcedure::FirstTrial");
    
    return this.NextTrial(0,0);
}

pluginProcedure.prototype.CheckParameters = function() {
    print("TestProcedure CheckParameters called");   
    
    for (prop in params) {
        print ("Property " + prop + "=" + params[prop] );
        
        if ( prop == "invalidparameter") {
            return "CheckParameters: Invalid parameter " + prop + "=" + params[prop];
        }
    }
    
    return "";
}


pluginProcedure.prototype.GetResultXML = function() {
    return "<procedure>\n   Result from testprocedure\n</procedure>";
}



new pluginProcedure();


