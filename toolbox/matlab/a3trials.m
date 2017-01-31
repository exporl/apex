function result=a3trials(names, answers, screens, stimuli)


if (length(answers)==1)
        answers=repmat(answers(1), size(names,1),size(names,2));
end
if (~iscell(screens))
        screens=repmat(screens, size(names,1),size(names,2));
        error('FIXME: Convert to cell');
end

if (length(names)~=length(answers) && length(answers)~=0)
    error('Length of answers incorrect');
end
if (length(names)~=length(screens) )
    error('Length of screens incorrect');
end
if (length(names)~=length(stimuli))
    error('Length of stimuli incorrect');
end


lf=sprintf('\n');
tb=sprintf('\t');
result='<trials>';

for iF=1:length(names)

   temp=a3trial(names{iF}, screens{iF}, stimuli{iF}, answers{iF});

   result=[result temp];
end


result=[result '</trials>'];

return;
