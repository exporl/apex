/* global api, params */
/* jshint noarg:true, noempty:true, eqeqeq:true, undef:true, curly:true, devel:true, bitwise:true, onevar:true, maxerr:50 */

/** XML related methods **/
var xml = {};

/** Various methods **/
var misc = {};

xml.stimulus = function(id, db, fp, v) {
    return xml.tag("stimulus", {"id" : id},
            xml.tag("datablocks",
                xml.tag("datablock", {"id" : db})),
            !fp ? '' :
            xml.tag("fixedParameters",
                xml.tag("parameter", {"id" : fp}, v)));
};

xml.trial = function(id, sc, st) {
    return xml.tag("trial", {"id" : id},
            xml.tag("screen", {"id" : sc}),
            st);
};

xml.datablock = function (id, device, uri, data) {
     return xml.tag("datablock", {"id": id},
             !device ? '' : xml.tag("device", device),
             !uri ? '' : xml.tag("uri", uri),
             !data ? '' : xml.tag("data", xml.cdata(data)));
};

misc.checkParams = function(list) {
    for (var i = 0; i < list.length; ++i) {
        if (typeof(params[list[i]]) === 'undefined') {
            api.addWarning("Parameter " + list[i] + " not defined for xml plugin script. Result might be undefined.");
        }
    }
};

misc.shuffleArray = function(myArray) {
    var i = myArray.length,
        j, tempi, tempj;

    if (!i) {
        return false;
    }

    while (--i) {
        j = Math.floor(Math.random() * (i + 1));
        tempi = myArray[i];
        tempj = myArray[j];
        myArray[i] = tempj;
        myArray[j] = tempi;
    }
};

xml.text = function(text)
{
    var result = text.toString();
    result.replace(/&/g, "&amp;");
    result.replace(/</g, "&lt;");
    result.replace(/>/g, "&gt;");
    return result;
};

xml.attribute = function(text) {
    var result = xml.text(text);
    result.replace(/"/g, "&quot;");
    return "\"" + result + "\"";
};

xml.tag = function(name) {
    var result = "<" + name,
        startcontents = 1,
        param, i, j;
    if (arguments.length > 1 && typeof(arguments[1]) === "object") {
        for (param in arguments[1]) {
            result += " " + param + "=" + xml.attribute(arguments[1][param]);
        }
        ++startcontents;
    }
    if (startcontents < arguments.length) {
        result += ">";
        for (i = startcontents; i < arguments.length; ++i) {
            if (typeof(arguments[i]) === "array") {
                for (j = 0; i < arguments[i].length; ++j) {
                    result += arguments[i][j];
                }
            } else {
                result += arguments[i];
            }
        }
        result += "</" + name + ">";
    } else {
        result += "/>";
    }
    return result;
};

xml.cdata = function(text) {
    return "<![CDATA[" + text + "]]>";
};
