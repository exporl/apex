function temp=a3trial(id, screen, stimuli, answer,standards)
% result=a3trial(id, screen, stimuli, answer, standards)

if (nargin<4)
    answer='';
end
if (nargin<5)
    standards='';
end

lf=sprintf('\n');
tb=sprintf('\t');

temp=['<trial id="' id '">' lf];
if (length(answer)~=0)
    temp=[temp tb '<answer>' answer '</answer>' lf];
end
temp=[temp tb '<screen id="' screen '"/>' lf];
if (iscell(stimuli))
    for i=1:length(stimuli)
        temp=[temp tb '<stimulus id="' stimuli{i} '"/>' lf];
    end
else
    temp=[temp tb '<stimulus id="' stimuli '"/>' lf];
end
if (iscell(standards))
    for i=1:length(standards)
        temp=[temp tb '<standard id="' standards{i} '"/>' lf];
    end
else
    if (~isempty(standards))
        temp=[temp tb '<standard id="' standards '"/>' lf];
    end
end
temp=[temp '</trial>' lf];