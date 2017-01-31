/** XML related methods **/
var xml = {};

/** Various methods **/
var misc = {};

xml.stimulus=function(id, db, fp, v)
{
    var fp_w='';
    if(fp_w.length) {
        fp_w=xml.tag("fixedParameters",
                xml.tag("parameter", {"id" : fp}, v));
    }
    return xml.tag("stimulus", {"id" : id},
            xml.tag("datablocks",
                xml.tag("datablock", {"id" : db})),
            fp_w);
}

xml.trial=function(id, sc, st)
{
    return xml.tag("trial", {"id" : id},
            xml.tag("screen", {"id" : sc}),
            st);
}

xml.datablock=function (id, device, uri) {
     var dev='';
     if (device.length) {
          dev = xml.tag("device", device);
     }
     return xml.tag("datablock", {"id": id},
            dev,
            xml.tag("uri", uri));
}

misc.checkParams=function(list) {
    for (i=0; i<list.length; ++i) {
        if (typeof(params[list[i]])=='undefined') {
            api.addWarning("Parameter " + list[i] + " not defined for xml plugin script. Result might be undefined.");
        }
    }
}


xml.text=function(text)
{
    var result = text.toString();
    result.replace(/&/g, "&amp;");
    result.replace(/</g, "&lt;");
    result.replace(/>/g, "&gt;");
    return result;
}

xml.attribute=function(text)
{
    var result = xml.text(text);
    result.replace(/"/g, "&quot;");
    return "\"" + result + "\"";
}


xml.tag = function (name)
{
    var result = "<" + name;
    var startcontents = 1;
    if (arguments.length > 1 && typeof(arguments[1]) == "object") {
        for (var param in arguments[1])
            result += " " + param + "=" + xml.attribute(arguments[1][param]);
        ++startcontents;
    }
    if (startcontents < arguments.length) {
        result += ">";
        for (var i = startcontents; i < arguments.length; ++i) {
            if (typeof(arguments[i]) == "array") {
                for (var j = 0; i < arguments[i].length; ++j)
                    result += arguments[i][j];
            } else {
                result += arguments[i];
            }
        }
        result += "</" + name + ">";
    } else {
        result += "/>";
    }
    return result;
}


