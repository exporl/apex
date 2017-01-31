function result=a3corrector(type,param1)
% result=a3corrector(type,param1)
%   type=['isequal' 'afc']


lf=sprintf('\n');
tb=sprintf('\t');

result='';

if (strcmp(type, 'isequal'))
    result=[result '<corrector xsi:type="apex:isequal" />'];
elseif (strcmp(type, 'afc'))
    if (nargin~=2)
        error('afccorrector: Invalid number of choices');
    end
    result=[result '<corrector xsi:type="apex:alternatives">' lf];
    result=[result tb '<answers>' lf];
    for i=1:param1
        result=[result tb tb '<answer number="' num2str(i) '" value="button' num2str(i) '"/>'];
    end
    result=[result tb '</answers>' lf];
    result=[result '</corrector>' lf];
else
    error('Unsupported corrector type');
end

%result=[result '</corrector>' lf];
