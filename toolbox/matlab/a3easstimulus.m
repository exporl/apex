function s=a3easstimulus(id, acdb, eldb, fixedparameters)
% a3easstimulus(id, acdb, eldb, fixedparameters)
if (nargin<4)
    fixedparameters='';
end

 s=['<stimulus id="' id '" >'];
    s=[s lf '<datablocks>'];
    s=[s lf '<simultaneous>'];
    s=[s lf '<sequential>'];
    s=[s lf '<datablock id="waitforl34"/>'];
    s=[s lf '<datablock id="' acdb  '" />'];
    s=[s lf '</sequential>'];
    s=[s lf '<datablock id="' eldb '" />'];
    s=[s lf '</simultaneous>'];
    s=[s lf '</datablocks>'];

    if (~isempty(fixedparameters))
        s=[s lf '<fixedParameters>'];
        s=[s lf a3showparams(fixedparameters) ];
        s=[s lf '</fixedParameters>' ];
    end

    s=[s lf '</stimulus>'];