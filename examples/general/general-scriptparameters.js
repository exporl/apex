// TODO:
// make sure path ends in /
// check for undefined parameters

function getStimuli () {
    misc.checkParams( Array("path", "prefix"));
    files = api.files( params["path"] );

    r="";
    for(i=0; i<files.length; ++i) {
        console.log(files[i]);
        r=r+xml.stimulus( params["prefix"]+files[i], "datablock_" + files[i]);
    }

    return r;
}


function getDatablocks() {
    misc.checkParams( Array("path", "device"));
    files = api.files( params["path"]);
    path=api.stripPath(params["path"]);

    r="";
    for(i=0; i<files.length; ++i) {
        console.log(files[i]);
        r=r+xml.datablock( "datablock_"+files[i], params["device"], path+files[i]);
    }

    return r;
}


function getTrials() {
    misc.checkParams( Array("path", "screen"));
    files = api.files( params["path"]);
    path=api.stripPath(params["path"]);

    r="";
    for(i=0; i<files.length; ++i) {
        console.log(files[i]);
        r=r+xml.trial( "trial_"+files[i], params["screen"],
            xml.tag("stimulus", {id: "stimulus_"+files[i]}));
    }

    return r;
}

