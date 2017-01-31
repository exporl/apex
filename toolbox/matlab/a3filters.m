function result=a3filters(types, p)
% types=    noisegen
%

if (nargin~=1)
    error('Refactored');
end

result=wraptag('filters',types);

return;

% if (nargin<2)
%     p.script='a3filters';
% end
%
% lf=sprintf('\n');
% tb=sprintf('\t');
%
% result=['<filters>' lf ];
%
% for i=1:length(types)
%     current=types{i};
%
%    if ( strcmp(current,'noisegen'))
%        result = [result a3noisegen()];
%    elseif ( strcmp(current,'amplifier'))
%        result = [result a3amplifier(p.basegain_speech)];
%    elseif ( strcmp(current,'dataloop'))
%        if (~isfield(p,'noisefile'))
%            error('No noise file given for noiseloop generator');
%        end
%        result = [result a3dataloop(p.noisefile,p.basegain_dataloop)];
%    else
%       error(['Invalid filter type ' current ]);
%    end
% end
%
%
% result=[result '</filters>' lf];
