function [script,err]=a3getmatlabscript(filename)
% get matlab script name to be used from processing instruction in results
% file

data=readpartfile(filename,'<?matlab', '?>');

tags=regexp(data, 'script\s*=\s*[''"](\w+)(\.m)?\s*[''"]', 'tokens');

if (length(tags))
    err=0;
    script=char(tags{1}{1}{1});
else
    err=1;
    script='';
end
