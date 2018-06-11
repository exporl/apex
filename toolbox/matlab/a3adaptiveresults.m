function [sequence,stimuli]=a3adaptiveresults(filename,xsltscript,forcetransform)
% Return adaptive staircase from APEX 4 results file

if (nargin<2)
    xsltscript='apexresult.xsl';
    forcetransform=0;
end
if (nargin==2)
    forcetransform=1;
end

if (~exist(filename, 'file'))
    % look in all data paths
    pa=getpaths;
    paths=pa.results;
    for i=1:length(paths)
        newpath=fixpath([paths{i} '/' filename ]);
       if (exist( newpath ,'file'))
           filename=newpath;
       end
    end
end


%% use new apex version
results=a3getresults(filename,xsltscript,forcetransform);
s=a3parseresults(results);
if (~isfield(s,'adaptiveparameter'))
    error('Adaptiveparameter field not found in results. Did you use an adaptive procedure?');
end

if (~isfield(s(1),'procedure'))

    sequence=zeros(length(s),1);
    stimuli=cell(length(s),1);
    for i=1:length(s)
        sequence(i)=str2num(s(i).adaptiveparameter);
        stimuli{i}=s(i).stimulus;
    end

else        % Multiprocedure
    % Get procedure IDs
%     procid=cell(length(s),1);
%     for i=1:length(s)
%         procid{i} = s(i).procedure;
%     end
%     uproc = sort(unique(procid));

    sequences = struct;
    for i=1:length(s)
        proc = s(i).procedure;
        if (~isfield(sequences, proc))
            sequences.(proc) = [];
        end
        sequences.(proc) = [sequences.(proc) str2num(s(i).adaptiveparameter)];
    end

    sequence = sequences;
end
