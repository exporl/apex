function result=a3stimuli(datablocks, stimuli,varpar,fixpar,addsilence_before, addsilence_after)
% list: list of datablocks


if (nargin==5)
    addsilence_after=0;
end

if (nargin<6)
    addsilence_before=0;
    addsilence_after=0;
end

if (nargin>3)
    if (length(varpar)==1)
        varpar=repmat(varpar, size(datablocks,1), size(datablocks,2));
        error('Fixme: convert to cell and not to array');
    end
    if (length(fixpar)==1)
        fixpar=repmat(fixpar, size(datablocks,1), size(datablocks,2));
        error('Fixme: convert to cell and not to array');
    end

else
    varpar={};
    fixpar={};
end


if (length(datablocks)==1)
        datablocks=repmat(datablocks(1), size(stimuli,1),size(stimuli,2));
end

if (length(datablocks)~=length(stimuli))
    error('Lengths don''t match');
end

if (length(fixpar) && length(fixpar)~=length(stimuli))
    error('Lengths of fixed parameters ans stimuli don''t match');
end
if (length(varpar) && length(varpar)~=length(stimuli))
    error('Lengths of variable parameters ans stimuli don''t match');
end


lf=sprintf('\n');
tb=sprintf('\t');
result=['<stimuli>' lf '<fixed_parameters>' lf];
if (length(fixpar))

    fields=fieldnames(fixpar{1});       % we assume that every stimulus has the same fixed parameters
    for i=1:length(fields)
        field=fields{i};
        result=[result tb '<parameter id="' field '"/>' lf];
    end
end
result=[result '</fixed_parameters>'];

for iF=1:length(datablocks)

   temp=['<stimulus id="' stimuli{iF} '">' lf];
   temp=[temp tb '<datablocks>' lf];
   temp=[temp tb '<sequential>' lf];
   if (addsilence_before)
          temp=[temp tb tb '<datablock id="silence"/>' lf];
   end
   temp=[temp tb tb '<datablock id="' datablocks{iF} '"/>' lf];
   if (addsilence_after)
          temp=[temp tb tb '<datablock id="silence"/>' lf];
   end
   temp=[temp tb '</sequential>' lf];
   temp=[temp tb '</datablocks>' lf];

   temp=[temp tb '<variableParameters>' lf];
   if (length(varpar))
     temp=[temp tb lf a3showparams(varpar{iF}) ];
   end
   temp=[temp tb '</variableParameters>' lf];

   temp=[temp tb '<fixedParameters>' lf];
   if (length(fixpar))
       temp=[temp tb lf a3showparams(fixpar{iF}) ];
   end
   temp=[temp tb '</fixedParameters>' lf];

   temp=[temp '</stimulus>' lf];

   result=[result temp];
end


result=[result '</stimuli>'];

return;
