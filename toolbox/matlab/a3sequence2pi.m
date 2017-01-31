function [intensity, performance] = a3sequence2pi(sequence)
% Convert sequence (result from adaptive procedure) to
% performance-intensity values.

%% Find first reversal
for i=1:length(sequence)-1
    if (sequence(i+1)<sequence(i))
        break;
    end
end
if (i == length(sequence)-1)
    error('No first reversal found');
end

snrs=zeros(length(sequence)-i,1);
correct=snrs;

count=1;
for q=i:length(sequence)-1
    snrs(count)=sequence(q);
    correct(count)=sequence(q)>sequence(q+1);
    count=count+1;
end

intensity=snrs;
performance=correct;
