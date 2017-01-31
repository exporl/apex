function result=a3header(version)

if (nargin<1)
    version='3.0';
end

t.encoding = 'UTF-8';

% if (isunix)
% t.encoding='ISO-8859-1';
% else
% t.encoding='Windows-1252';
% end

switch version
    case '3.0'
        result=readfile_replace('header_v30.xml',t);
    case '3.1'
        result=readfile_replace('header_v310.xml',t);
    case '3.1.0'
        result=readfile_replace('header_v310.xml',t);
    case '3.1.1'
        result=readfile_replace('header_v311.xml',t);
end
