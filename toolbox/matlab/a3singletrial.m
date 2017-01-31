function result=a3singletrial(name,answer,screen, stimuli,standard)
% show a trials block with a single trial with multiple stimuli


lf=sprintf('\n');
tb=sprintf('\t');
result=['<trials>' lf];

result=[result '<trial id="' name '">' lf];

if (length(answer)~=0)
    result=[result tb '<answer>' answer '</answer>' lf];
end

result=[result tb '<screen id="' screen '"/>' lf];


for iF=1:length(stimuli)
  result=[result tb '<stimulus id="' stimuli{iF} '"/>' lf];
end

result=[result '</trial>' lf];
result=[result '</trials>'];

return;
