function result=a3general(exitafter)
% result=a3general(exitafter,showresults,saveprocessedresults)

if (nargin~=1)
    error('Invalid number of parameters');
end

lf=sprintf('\n');
result=['<general>' lf];
result=[result '<exitafter>'  bool2xml(exitafter) '</exitafter>' lf];
result=[result '</general>' lf];
