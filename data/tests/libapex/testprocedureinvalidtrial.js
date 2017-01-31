// Simple constantProcedure implemented as a plugin procedure

function scriptProcedure() {        // Constructor
}

scriptProcedure.prototype=new ScriptProcedureInterface();

scriptProcedure.prototype.setupNextTrial = function()
{
    console.log("TestProcedure setupNextTrial called.");
    return 5; // The interesting part
}

scriptProcedure.prototype.firstScreen = function() {}

scriptProcedure.prototype.resultXml = function() {}

scriptProcedure.prototype.progress = function() {}

scriptProcedure.prototype.processResult = function(screenResult) {}

scriptProcedure.prototype.checkParameters = function()
{
    console.log("TestProcedure CheckParameters called");
    return "";
}


function getProcedure()
{
    console.log("Returning procedure");
    return new scriptProcedure();
}
