function getDatablocks() {

    misc.checkParams( Array("CL_from", "CL_to", "CL_step", "electrode"));
    files = api.files( params["path"]);
    path=api.stripPath(params["path"]);
    
    step = parseInt( params["CL_step"] );
    
    r="";
    cl=parseInt(params["CL_from"]);
    while (cl < parseInt(params["CL_to"]) ) {
        data = "<nic:sequence xmlns:nic='http://www.cochlear.com/nic' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.cochlear.com/nic nic-stimuli.xsd' >" +
          "<stimulus>" +
            "<parameters>" +
                "<ae>" + params["electrode"] + "</ae><re>-1</re><cl>" + cl + "</cl><t>1000.0</t><pw>25.0</pw><pg>8.0</pg>" +
            "</parameters>" +
          "</stimulus>" +
          "</nic:sequence>";

        db = "<datablock id='datablock_" + cl + "'>" +
        "<device>l34device</device>" + 
        "<data>" + data + "</data>" +
        "</datablock>";

        r=r+db; 
        
        cl += step;
        print( cl );
    }
    
    print(r);
    
    return r;

}

function getStimuli () {
    misc.checkParams( Array("CL_from", "CL_to", "CL_step"));
    
    step = parseInt( params["CL_step"] );
        
    r="";
    cl=parseInt(params["CL_from"]);
    while (cl < parseInt(params["CL_to"]) ) {
        r=r+xml.stimulus("stimulus_" + cl, "datablock_" + cl);
        cl += step;
    }
    
    return r;
}

function getTrials () {
    misc.checkParams( Array("CL_from", "CL_to", "CL_step"));
    
    step = parseInt( params["CL_step"] );
        
    r="";
    cl=parseInt(params["CL_from"]);
    while (cl < parseInt(params["CL_to"]) ) {
         r = r + xml.tag("trial", {"id" : "trial_"+cl},
            xml.tag("answer", "first"),
            xml.tag("screen", {"id" : "screen"}),
            xml.tag("stimulus", {id: "stimulus_"+cl}));
            
         cl += step;
    }
    
    print(r);
    
    return r;
}