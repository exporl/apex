var parametersForm = document.getElementById('parameters-form');
var experimentDiv = document.getElementById('experiment-div');
var resultsDiv = document.getElementById('results-div');
var main_experiment_reset = copyObject(main_experiment);

function loadMain(showForm) {
    main_experiment = copyObject(main_experiment_reset);
    experimentDiv.innerHTML = '';
    resultsDiv.innerHTML = '';
    if ( showForm && main_experiment.form ) {
        parametersForm.style.display = 'block';
        parametersForm.innerHTML = makeForm(main_experiment.form);
        parametersForm.innerHTML += '<input type="button" value="Go!" onclick="loadMain(0);" />';
    } else {
        parametersForm.style.display = 'none';
        var form_response = parametersForm.elements;
        for ( var elementID = 0 ; elementID < form_response.length ; elementID++ ) {
            element = form_response[elementID];
            if (element.type !== "button" && element.id)
                main_experiment.params[element.id] = element.value;
        }
        setMainParameters(main_experiment);
        loadExperiments(main_experiment);
        if ( ! main_experiment.blind )
            resultsDiv.innerHTML = '<h1>Finished experiments</h1><ul></ul>';
    }
}

function makeForm(form) {
    if ( typeof form === "string" ) {
        return form;
    } else {
        var formHTML = "";
        for ( tagID in form ) {
            var tag = form[tagID];
            formHTML += "<" + tag.tag_name;
            for ( attribute in tag ) {
                if ( attribute !== "content" && attribute !== "tag_name" )
                    formHTML += " " + attribute + "='" + tag[attribute] + "'";
            }
            formHTML += ">";
            if ( tag.content )
                formHTML += makeForm(tag.content);
            formHTML += "</" + tag.tag_name + "><br />";
        }
    }
    return formHTML;
}

function loadExperiments(experiment) {
    if ( !experiment.experiments ) {
        setExperimentParameters(experiment); // function to define generic experiment parameters
        makeButton(experiment);
    } else {
        if ( !experiment.parents ) {
            experiment.parents = [];
            experiment.path = "main_experiment";
        }
        experiment.layer = experiment.parents.length + 1;

        experimentDiv.innerHTML += '<h' + experiment.layer + '>' + experiment.title + '</h' + experiment.layer + '>';
        if ( experiment.description )
            experimentDiv.innerHTML += '<p>' + experiment.description + '</p>';

        if ( experiment.layer == 1 && experiment.params ) {
            experimentDiv.innerHTML += '<input type="button" value="Reset experiment" onclick="loadMain(1);" />';
            experimentDiv.innerHTML += '<ul></ul>';
            for ( var param in experiment.params )
                experimentDiv.getElementsByTagName('ul')[0].innerHTML += '<li>' + param + ': ' + experiment.params[param] + '</li>';
        }

        if ( experiment.randomized )
            shuffle(experiment.experiments);

        for ( var experimentKey in experiment.experiments ) {
            for ( var paramKey in experiment ) {
                if ( !experiment.experiments[experimentKey].hasOwnProperty(paramKey) && paramKey != 'experiments' && paramKey != 'description' )
                    experiment.experiments[experimentKey][paramKey] = experiment[paramKey];
            }

            experiment.experiments[experimentKey].parents = experiment.parents.slice();
            experiment.experiments[experimentKey].parents.push(experiment.title);
            experiment.experiments[experimentKey].path += '.experiments[' + experimentKey + ']';
            loadExperiments(experiment.experiments[experimentKey]);
        }
    }
}

function makeButton(experiment) {
    experiment.ID = experiment.parents.join("_").split(" ").join("-") + '_' + experiment.title.split(" ").join("-");
    var button;
    if ( !experiment.description )
        experiment.description = '';
    if ( experiment.blind ) {
        button = '<button onclick="doExperiment(' + experiment.path + ');" id="' + experiment.ID + '-button" title="' + experiment.description + '">Start experiment</button>';
    } else {
        button = '<button onclick="doExperiment(' + experiment.path + ');" id="' + experiment.ID + '-button" title="' + experiment.description + '">' + experiment.title + '</button>';
    }

    experimentDiv.innerHTML += '<span id="' + experiment.ID + '-span">- '
        + button
        + '<span style="display: none;"> '
        + '(<a href="#" onclick="resetExperiment(' + experiment.path + ');">redo</a>)'
        + '</span>'
        + '</span><br />';
}

function doExperiment(experiment) {
    document.getElementById(experiment.ID + '-button').disabled = true;
    document.getElementById(experiment.ID + '-span').getElementsByTagName('span')[0].style.display = "inline";
    if ( !experiment.blind )
        resultsDiv.getElementsByTagName('ul')[0].innerHTML += '<li>' + experiment.file + '</li>';

    if ( experiment.expressions && experiment.resultfile ) {
        api.absoluteFilePath(experiment.file).then(function(path) {
            api.runExperiment(path, experiment.expressions, experiment.resultfile);
        });
    } else if ( experiment.expressions ) {
        api.absoluteFilePath(experiment.file).then(function(path) {
            api.runExperiment(path, experiment.expressions);
        });
    } else if ( experiment.resultfile ) {
        api.absoluteFilePath(experiment.file).then(function(path) {
            api.runExperiment(path, {}, experiment.resultfile);
        });
    } else {
        api.absoluteFilePath(experiment.file).then(function(path) {
            api.runExperiment(path);
        });
    }
}

function resetExperiment(experiment) {
    document.getElementById(experiment.ID + '-button').disabled = false;
    document.getElementById(experiment.ID + '-span').getElementsByTagName('span')[0].style.display = "none";
    setExperimentParameters(experiment);
}

function shuffle(array) {
    var currentIndex = array.length, temporaryValue, randomIndex;
    while (0 !== currentIndex) {
        randomIndex = Math.floor(Math.random() * currentIndex);
        currentIndex -= 1;
        temporaryValue = array[currentIndex];
        array[currentIndex] = array[randomIndex];
        array[randomIndex] = temporaryValue;
    }
    return array;
}

function copyObject(originalObject) {
    return jQuery.extend(true, {}, originalObject);
}
