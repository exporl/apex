function result=a3connections(p,type)

if (nargin>1)
    error('Refactoring!!');
end

result= wraptag('connections',p);

% if (strcmp(type,'speech'))
%     result=readfile('speechconnections.xml');
% elseif (strcmp(type,'amplifier'))
%     result=['<connections>' readfile('amplifierconnection.xml') '</connections>'];
% elseif (length(type)==0)
%     result='<connections/>';
% end


return;
