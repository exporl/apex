function [stimlabels, percent, count, totals]=a3results2psy(filename, regexs, correctanswer)
% Read results from apex results file and accumulate them into a
% psychometric function
%
% regexs: list of regular expressions to get parameter from stimulus id.
% correctanswer: if defined, the answer will be corrected according to this
% variable, instead of using the apex corrector values
%
% stimlabels: all found labels (x-axis)
% percent: percent correct
% count: number of times 1 was encountered
% totals: total number of presentations

if (nargin<3)
    correctanswer='';
end

[stimuli,buttons,corrector]=a3cstresults(filename);

% ignore the trial when no answer was given in an AFC experiment
keep=find(~strcmp(buttons,'-1'));
stimuli=stimuli(keep);
buttons=buttons(keep);
corrector=corrector(keep);

% get all stimulus parameter values
stimval=zeros(length(stimuli),1);
iserr=0;
for i=1:length(stimuli)
    if (~iscell(regexs))
        [tokens]=regexp(stimuli{i}, regexs, 'tokens');
    else
       for a=1:length(spattern)
           [t,n,err]=sscanf(stimuli{i}, spattern{a});
           if (length(err)==0)
               break;
           end
       end
    end

    if (isempty(tokens))
        if (~iserr)
            disp(['Warning: none of the patterns worked on stimulus ' stimuli{i}]);
        end
        iserr=iserr+1;
        stimval(i)=Inf;
    else
        stimval(i)=str2num(tokens{1}{1});
    end
    if (length(tokens)>1)
        warning('Regexp yielded more than one result');
    end

end

if (iserr)
   disp(['Warning: none of the patterns worked on ' num2str(iserr) ' stimuli']);
end

stimlabels=removeduplicate(stimval);

if (isinf(stimlabels(end)))
    stimlabels=stimlabels(1:end-1);
end

count=zeros(1,length(stimlabels));      % count the number of ones in corrector
totals=zeros(1,length(stimlabels));      % count the number of times a stimlus was presented

for i=1:length(stimlabels)
   I=find(stimval==stimlabels( i));
   if (~length(correctanswer))
       count(i)=sum(corrector(I));
   else
      count(i)=sum(correctit(buttons(I), correctanswer));
   end
   totals(i)=length(I);
end


% try to get number of presentations
% if (~numpres)
%     if (ceil(length(stimuli)/length(stimlabels)) ~= length(stimuli)/length(stimlabels))
%         warning('Number of presentations probably wrong');
%     end
%    numpres=ceil(length(find(~isinf(stimval)))/length(stimlabels));
%    disp(['Estimating number of presentations: ' num2str(numpres) ]);
% end
%
% percent=100*count/numpres;

percent=100*count./totals;



function result=correctit(buttons, correctanswer)
result=zeros(length(buttons),1);
for i=1:length(buttons)
    if (strcmp(buttons{i}, correctanswer))
        result(i)=1;
    end
end
