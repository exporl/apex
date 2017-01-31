function [sequence, general]=a3speechresults(filename)
% [stimuli, corrector, sequence,label]=a3aidresults(filename)

% read transformed results from file
[results,~,general]=a3getresults(filename);


% look for the response line
I=strmatch('[Sequence', results);
if (isempty(I))

    %% use new apex version
    results=a3getresults(filename);
    s=a3parseresults(results);
    if (~isfield(s,'adaptiveparameter'))
        error('Adaptiveparameter field not found in results');
    end

    sequence=zeros(length(s),1);
    for i=1:length(s)
       sequence(i)=str2num(s(i).adaptiveparameter);
    end

else
    %sequence=strread(results{I+1}, '%f\t');
    sequence=sscanf(results{I+1}, '%f\t');
end
