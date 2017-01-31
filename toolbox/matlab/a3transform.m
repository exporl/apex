function result=a3transform(filename, script)

r=a3localsettings();

%% use external transformer
if (0)
    cmdline=[r.xalancmd ' -text -nh -param target \''parser\'' -xsl file:' r.apex_xslt_scripts script ' -in "file:' filename '"']; %'" -out "tempfile2388.txt"'];
    [s, result]=system(cmdline);
end

%% use java transformer

tempfile=tempname;
a3xslt(filename,[r.apex_xslt_scripts script],tempfile);
result=readfile(tempfile);
delete(tempfile);
