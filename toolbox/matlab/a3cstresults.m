function [stimuli, buttons, corrector,parameters,s]=a3cstresults(filenames,xsltscript)
% [stimuli, buttons, corrector]=a3cstresults(filenames)
% filenames can be a single file name or a cell array of files from which
% the results will be merged

if (~iscell(filenames))
    filenames={filenames};
end
if (nargin<2)
    xsltscript='';
end


stimuli=[];
buttons=[];
corrector=[];


smart=0;            % smart will be set to 1 if we recognize the xslt script


for f=1:length(filenames)
    filename=filenames{f};

    % read transformed results from file
    if (~isempty(xsltscript))
        forcetransform=1;
    else
        forcetransform=0;
    end
    [results,parameters]=a3getresults(filename,xsltscript,forcetransform);

    parseok=0;

    I=strmatch('Header=trial;stimulus;correctanswer;corrector;useranswer', results);
    if (~isempty(I))            %% new format
        s=a3parseresults(results);

        thisstimuli=cell(length(s),1);
        thisbuttons=thisstimuli;
        thiscorrector=zeros(length(s),1);
        for i=1:length(s)
            thisstimuli{i}=s(i).stimulus;
            thisbuttons{i}=s(i).useranswer;
            if (strcmp(s(i).corrector,'true'))
                thiscorrector(i)=1;
            end
        end

        stimuli=cat(1,stimuli,thisstimuli);
        buttons=cat(1,buttons,thisbuttons);
        corrector=[corrector; thiscorrector];
        parseok=1;
        continue;
    end

    I=strmatch('[Stimulus/answer/correct pairs]',results);
    if (~isempty(I))
        smart=1;
    else
        smart=0;
    end

        % look for the stimulus line
    I=strmatch('[Stimulus/answer', results);
    if (isempty(I))
        error(['Stimulus line not found in file ' filename]);
    else
       I=I+1;
    end

    %stimuli=[stimuli; strread(results{I}, '%s\t')];
    res=strsplit(results{I}, tb)
    if (~isempty(res) && isempty(res{end}))
         res=res(1:end-1);
    end
    stimuli=[stimuli res];


    %scorrector=strread(results{I+2}, '%s\t');
    scorrector=strsplit(results{I+2}, tb);
    if (~isempty(scorrector) && isempty(scorrector{end}))
        scorrector=scorrector(1:end-1);
    end
    tcorrector=zeros(length(scorrector), 1);
    for i=1:length(scorrector)
        if (strcmp(scorrector(i),'true'))
            tcorrector(i)=1;
        end
    end
    corrector=[corrector; tcorrector];

    if (isempty(corrector))
        I=strmatch('[Stimulus/correct answer/response/correctorvalue', results);
        if (~isempty(I))
            scorrector=strread(results{I+4}, '%s\t');
            if (~isempty(scorrector) && isempty(scorrector{end}))
                scorrector=scorrector(1:end-1);
            end

            tcorrector=zeros(length(scorrector), 1);
            for i=1:length(scorrector)
                if (strcmp(scorrector(i),'true'))
                    tcorrector(i)=1;
                end
            end
            corrector=[corrector; tcorrector];
        end
    end


    if (smart)
        %buttons=[buttons; strread(results{I+1}, '%s\t')];

        res=strsplit(results{I+1}, tb);
        if (~isempty(res) && isempty(res{end}))
             res=res(1:end-1);
        end

        buttons=[buttons res];
    else

        % look for the response line
        I=strmatch('button', results);
        if (isempty(I))
            warning('Button line not found');
            buttons='';
        else
            %buttons=[buttons; strread(results{I}, '%s\t')];
            buttons=[buttons; strsplit(results{I}, tb)];
        end

    end

end

if (length(stimuli)~=length(buttons) || length(buttons)~=length(corrector))
    error('Internal error');
end
