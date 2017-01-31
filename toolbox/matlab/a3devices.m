function result=a3devices(dev)

if (nargin<1)
    error('Function refactored');
end

result=wraptag('devices', dev);

return;

%
%
% lf=sprintf('\n');
% tb=sprintf('\t');
%
% result=['<devices>' lf ];
%
% result=[result a3wavdevice];
%
% result=[result lf '</devices>' ];