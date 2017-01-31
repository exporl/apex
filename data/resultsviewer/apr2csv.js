function process(xmlString) {
	var xml = (new DOMParser()).parseFromString(xmlString, "application/xml");
	// TODO: multiprocedures should have 2 headers

	results = '';
	results += printExperimentFile(xml) + '\n';
	results += printDuration(xml) + '\n';
	results += printHeader(xml) + '\n';
	results += 'START' + '\n';
	results += printTrials(xml);
	results +='END';

	return results;
}

function printExperimentFile(xml){
    var experimentFile = $(xml).find("results").attr("experiment_file");

    return 'ExperimentFile='+experimentFile;
}

function printDuration(xml){
	var duration = $(xml).find("duration");
	var durationValue = duration.text();
	var durationUnit = duration.attr("unit");

	return 'Duration='+durationValue+' '+durationUnit;
}

function showExtraParametersHeader(xml){
	extraParameters = '';
	var parameters = $(xml).find("parameters > parameter").each(function(){
		if($(this).attr("name") === "showextra"){
			extraParameters += ';' + $(this).text();
		}
	});

	return extraParameters;
}

function printHeader(xml){
	header = '';
	if(showProcedureID(xml)) {
		header += 'procedure;'
	}
	header += 'trial;stimulus;correctanswer;corrector;useranswer';
	if(containsAdaptive(xml)){
		header += ';adaptiveparameter';
	}
	if(showClickPosition(xml)){
		header += ';xclickposition;yclickposition';
	}
	header += showExtraParametersHeader(xml);

	return 'Header='+header;
}

function showExtraParameters(trial, xml){
	ids = [];
	var parameters = $(xml).find("parameters > parameter").each(function(){
		if($(this).attr("name") === "showextra"){
			ids.push($(this).text());
		}
	});

	extraParameters = '';
	$(trial).find("*[id]").each(function(){
		currentId = $(this).attr("id");

		if($.inArray(currentId, ids) > -1) {
			extraParameters += ';' + $(this).text();
		}
	});

	return extraParameters;
}

function printTrials(xml){
	var trialData = '';
	$(xml).find("trial").each(function() {
		if(showProcedureID(xml)) {
			trialData = $(this).find("procedure").attr("id")+';';
		}
		trialData += $(this).attr("id") + ';';
		trialData += $(this).find("stimulus").text() + ';';
		trialData += $(this).find("correct_answer").text() + ';';
		trialData += $(this).find("correct").text() + ';';
		trialData += $(this).find("answer").text();
		if(containsAdaptive(xml)){
			trialData = ';'+$(this).find("procedure > parameter").text();
		}
		if(showClickPosition(xml)){
			trialData += ';'+$(this).find("screenresult").attr("xclickposition");
			trialData += ';'+$(this).find("screenresult").attr("yclickposition");
		}

		trialData += showExtraParameters($(this), xml);
		trialData += '\n';

	});

	return trialData;
}

function containsAdaptive(xml){
	isAdaptive = false;
	var procedures = $(xml).find("procedure").each(function() {
		var procedureType = $(this).attr("type");
		if(procedureType === "apex:adaptiveProcedure"){
			isAdaptive = true;
		}
	})

	return isAdaptive;
}

function showProcedureID(xml){
	return ($(xml).find("procedure[id]").length > 0);
}

function showClickPosition(xml){
	return ($(xml).find("screenresult[xclickposition]").length > 0);
}
