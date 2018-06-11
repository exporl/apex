function temp=a3stimulus(id, datablocks,fixedparameters,variableparameters,simultaneous)
% a3stimulus(id, datablocks,fixedparameters,variableparameters,simultaneous)
% make simple stimulus with n datablocks
% datablocks is a cell of datablocks
% parameters is a struct of fixed parameters
% if simultaneous==1, datablocks are put in parallel, otherwise in series (default)

if (nargin<5)
    simultaneous=0;
end

if (nargin<=2)
    fixedparameters=struct;
end
if (nargin<=3)
    variableparameters=struct;
end

if (~iscell(datablocks))
    datablocks={datablocks};
end

temp=['<stimulus id="' id '">' lf];
temp=[temp tb '<datablocks>' lf];
temp=[temp tb '<sequential>' lf];
if size(datablocks,2)>1 && simultaneous
    temp=[temp tb '<simultaneous>' lf];
end
for iF=1:length(datablocks)
    temp=[temp tb tb '<datablock id="' datablocks{iF} '"/>' lf];
end
if simultaneous
    temp=[temp tb '</simultaneous>' lf];
end
temp=[temp tb '</sequential>' lf];
temp=[temp tb '</datablocks>' lf];

temp=[temp tb '<variableParameters>' lf];
temp=[temp tb a3showparams(variableparameters) ];
temp=[temp tb '</variableParameters>' lf];

temp=[temp tb '<fixedParameters>' lf];
temp=[temp tb a3showparams(fixedparameters) ];
temp=[temp tb '</fixedParameters>' lf];

temp=[temp '</stimulus>' lf];
