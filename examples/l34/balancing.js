function getDatablocks() {

    misc.checkParams( Array("CL_from", "CL_to", "CL_step", "electrode"));
    
    step = parseInt( params["CL_step"] );
    
    r="";
    cl=parseInt(params["CL_from"]);
    while (cl <= parseInt(params["CL_to"]) ) {
        data = "<nic:sequence xmlns:nic='http://www.cochlear.com/nic' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.cochlear.com/nic nic-stimuli.xsd' >" +
          "<stimulus repeat='450'>" +
            "<parameters>" +
                "<ae>" + params["electrode"] + "</ae><re>-1</re><cl>" + cl + "</cl><t>111</t><pw>25.0</pw><pg>8.0</pg>" +
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
    while (cl <= parseInt(params["CL_to"]) ) {
    
        stim_fwd = "<stimulus id='stimulus_" + cl + "CU_fwd'>" + 
    		"<datablocks>" +
    			"<sequential>" +
    				"<datablock id='datablock_ref'/>" +
    				"<datablock id='datablock_silence'/>" +
    				"<datablock id='datablock_" + cl + "'/>" +
    			"</sequential>" +
    		"</datablocks>" +
    		"<fixedParameters>" +
    			"<parameter id='current_level'>" + cl + "</parameter>" +
    		"</fixedParameters>" +
    	"</stimulus>" ;
    	
    	 stim_rev = "<stimulus id='stimulus_" + cl + "CU_rev'>" + 
    		"<datablocks>" +
    			"<sequential>" +
    			     "<datablock id='datablock_" + cl + "'/>" +
    				"<datablock id='datablock_silence'/>" +
    				"<datablock id='datablock_ref'/>" +
    			"</sequential>" +
    		"</datablocks>" +
    		"<fixedParameters>" +
    			"<parameter id='current_level'>" + cl + "</parameter>" +
    		"</fixedParameters>" +
    	"</stimulus>" ;
    
    
        r += stim_fwd + stim_rev;
        cl += step;
    }
    
    return r;
}

function getTrials () {
    misc.checkParams( Array("CL_from", "CL_to", "CL_step"));
    
    step = parseInt( params["CL_step"] );
        
    st_fwd="";
    st_rev="";
    cl = parseInt(params["CL_from"]);
    while (cl <= parseInt(params["CL_to"]) ) {
         st_fwd += xml.tag("stimulus", {id: "stimulus_"+cl+"CU_fwd"} );         
         st_rev += xml.tag("stimulus", {id: "stimulus_"+cl+"CU_rev"} );
            
         cl += step;
    }
    
    r = xml.tag("trial", {"id" : "trial_forward"},
            xml.tag("answer", "button2"),
            xml.tag("screen", {"id" : "screen"}),
            st_fwd);
            
    r += xml.tag("trial", {"id" : "trial_reverse"},
            xml.tag("answer", "button1"),
            xml.tag("screen", {"id" : "screen"}),
            st_rev);
    
    print(r);
    
    return r;
}