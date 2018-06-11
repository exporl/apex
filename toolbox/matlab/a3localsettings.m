function r=a3localsettings(type)
% return APEX 4 toolbox settings

if (nargin<1)
    type = '';
end

% path to xalan
r.xalancmd='/usr/bin/xalan';
% Main APEX directory, is often c:/Program Files/APEX
r.apex_path='/home/tom/data/apex/';
% APEX experiment schema
r.apex_schema=[r.apex_path 'schemas/experiment.xsd'];
% APEX XSLT scripts, only necessary if APEX does not do the XSLT transformation
r.apex_xslt_scripts=[r.apex_path 'data/xslt/'];
% Tool to check XML files, not required
r.xml_check_tool='/usr/bin/xmllint';

if (length(type))
    r = r.(type);
end