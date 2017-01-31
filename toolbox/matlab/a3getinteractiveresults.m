function parameters=a3getinteractiveresults(filename)
% Get user selected values of interactive parameters

parameters.expressions={};
parameters.values={};

part=readpartfile(filename,'<interactive>', '</interactive>');
for l=1:length(part)
    line=part{l};
    names=regexp(line, '<entry expression="(?<expression>[^"]*)" new_value="(?<value>[^"]*)"', 'names');
    if (length(names))
        parameters.expressions{end+1}=names.expression;
        parameters.values{end+1}=names.value;
    end
end
