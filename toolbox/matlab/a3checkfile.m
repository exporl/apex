function a3checkfile(filename)
% check apex3 experiment file against apex schema

if (~isunix)
    warning('a3checkfile not implemented on Windows');
    return;
end

s=a3localsettings;
schema=s.apex_schema;


disp(['Checking document ' filename '...']);

system( [s.xml_check_tool ' --schema ' schema ' ' filename '>/dev/null']  );
