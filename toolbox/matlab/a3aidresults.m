function [stimuli, corrector, sequence,label]=a3aidresults(filename)
% [stimuli, corrector, sequence,label]=a3aidresults(filename)

% read transformed results from file
results=a3getresults(filename);

% look for the stimulus line
I=strmatch('[Stimulus', results);
if (isempty(I))
    error('Stimulus line not found');
end

%stimuli=strread(results{I+1}, '%s\t');
%scorrector=strread(results{I+2}, '%s\t');
stimuli=amt_strsplit(results{I+1},tb);
scorrector=amt_strsplit(results{I+2}, tb);
corrector=zeros(length(scorrector), 1);
for i=1:length(scorrector)
    if (strcmp(scorrector(i),'true'))
        corrector(i)=1;
    end
end

%% reversals can be calculated later
% look for the response line
% I=strmatch('[Reversals', results);
% if (isempty(I))
%     warning('Reversals line not found');
%     reversals='';
% else
%     reversals=strread(results{I+1}, '%f\t');
% end

% look for the response line
I=strmatch('[Sequence', results);
if (isempty(I))
    warning('Sequence line not found');
    sequence='';
else
    %sequence=strread(results{I+1}, '%f\t');
    sequence=sscanf(results{I+1}, '%f\t');
end

% look for the response line
I=strmatch('[Label', results);
if (isempty(I))
    warning('Label line not found');
    sequence='';
else
    label=sscanf(results{I+1}, '%f\t');
end
