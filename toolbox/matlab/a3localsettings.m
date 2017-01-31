function r=a3localsettings
% return apex 3 toolbox settings

% path to xalan, only necessary if APEX does not do the XSLT transformation
r.xalancmd='/usr/bin/xalan';
% Main APEX directory, is often c:/Program Files/APEX
r.apex_path='/home/tom/data/apex/';
% APEX experiment schema
r.apex_schema=[r.apex_path 'schemas/experiment.xsd'];
% APEX XSLT scripts, only necessary if APEX does not do the XSLT transformation
r.apex_xslt_scripts=[r.apex_path 'data/xslt/'];
% Tool to check XML files, not required
r.xml_check_tool='/usr/bin/xmllint';
