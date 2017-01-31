function result=a3fixedparameters(names)
% a3fixedparameters(names)
% names: cell of names

lf=sprintf('\n');
tb=sprintf('\t');
result=['<fixed_parameters>' lf];

    for i=1:length(names)
        result=[result tb '<parameter id="' names{i} '"/>' lf];
    end

result=[result '</fixed_parameters>'];