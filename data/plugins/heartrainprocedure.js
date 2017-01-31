 function shuffle(myArray)
 {
     var i = myArray.length;

     if (i == 0)
         return false;

     while (--i)
     {
         var j = Math.floor( Math.random() * ( i + 1 ) );
         var tempi = myArray[i];
         var tempj = myArray[j];
         myArray[i] = tempj;
         myArray[j] = tempi;
     }
 }
 
 

function pluginProcedure() // Constructor
{
    //trying to mimic an enum :-)
    FIRST_TRIAL = 0;
    AFTER_TRIAL = 1;
    AFTER_CONFIRMATION = 2;
    FINISHED = 3;

    currentState = FIRST_TRIAL;
    currentTrial = -1;
    singleTrials = api.GetTrials();

    confirmationTrial = singleTrials[singleTrials.length - 1];
    singleTrials.splice(singleTrials.length - 1, 1);
    print("conf id: " + confirmationTrial.id);

    print("got " + singleTrials.length + " trials from api:");

    for (i = 0; i < singleTrials.length; i++)
        print("    " + singleTrials[i].id);

    trials = new Array();

    for (i = 0; i < params.presentations; i++)
        trials = trials.concat(singleTrials);

    if (params.order == "random")
    {
        shuffle(trials);
        //trials.sort(function() {return 0.5 - Math.random()});
    }

    nbOfTrials = trials.length;
    api.registerParameter("correct_answer");

    var currentStimulus;

//     confirmationTrial = new ApexTrial();
//     confirmationTrial.id = "confirmation_trial";
//     confirmationTrial.screen = "confirmation_screen";
}

pluginProcedure.prototype = new pluginProcedureInterface();

pluginProcedure.prototype.NextTrial = function(answer, screenresult)
{
    print("HearTrainProcedure NextTrial called");
    print("    currentTrial: " + (currentTrial + 1) + "/" + nbOfTrials);
    print(params.order);

    api.SetProgress(currentTrial + 1);

    //determine which trial to run next
    var trial;
    var confirmationStimulus = 0;

    if (currentState == AFTER_TRIAL)
    {
        trial = confirmationTrial;
        confirmationStimulus = "dummystimulus";
        answer = api.screenElementText(trials[currentTrial].screen,
                                       trials[currentTrial].answerString);
        api.SetParameter("correct_answer", answer);
        currentState = AFTER_CONFIRMATION;
    }
    else
    {
        if (screenresult["buttongroup1"] == "again_button")
        {
            trial = confirmationTrial;
            confirmationStimulus = trials[currentTrial].GetStimulus();
            currentState = AFTER_CONFIRMATION;
        }
        else
        {
            if (++currentTrial >= nbOfTrials)
                return ""; //done

            trial = trials[currentTrial];
            currentState = AFTER_TRIAL;
        }
    }

    targetTrial = trial.id;
    targetScreen = trial.screen;

    if (confirmationStimulus == 0)
        targetStimulus = trial.GetStimulus();
    else
        targetStimulus = confirmationStimulus;

//     print("Testprocedure: last answer to buttongroup1: " + screenresult["buttongroup1"]);
//
//     print("Testprocedure: using stimulus " + targetStimulus);
//     print("Testprocedure: using trial " + targetTrial);
//     print("Testprocedure: using screen " + targetScreen);

    //api.SetScreen(targetScreen);
    //api.CreateOutputList(targetTrial, targetStimulus);
    api.createTrial(targetTrial, targetStimulus, targetScreen);
    currentStimulus = targetStimulus;

    print("HearTrainProcedure returning " + targetTrial);

    return targetTrial;
}


pluginProcedure.prototype.FirstTrial = function()
{
    print("TestProcedure FirstTrial called");

    api.SetNumTrials(nbOfTrials);

    return this.NextTrial(0,0);
}

pluginProcedure.prototype.CheckParameters = function()
{
    print("TestProcedure CheckParameters called");

    for (prop in params)
    {
        print ("Property " + prop + "=" + params[prop] );

        if ( prop == "invalidparameter")
            return "CheckParameters: Invalid parameter " + prop + "=" + params[prop];
    }

    return "";
}


pluginProcedure.prototype.GetResultXML = function()
{
    var xml = "<procedure>\n";
    xml += "    <stimulus>" + currentStimulus + "</stimulus>\n";

    xml += "    <isConfirmationTrial>";
    xml += currentState == AFTER_CONFIRMATION;
    xml += "</isConfirmationTrial>\n";

    return xml + "</procedure>";
}

new pluginProcedure();


