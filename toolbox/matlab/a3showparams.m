function result=a3showparams(s)
fields=fieldnames(s);
result='';
temp='';
for i=1:length(fields)
    field=fields{i};

        temp=['<parameter id="' field '">' num2str(getfield(s,field)) '</parameter>' sprintf('\n')];

    result=[result temp];
end
