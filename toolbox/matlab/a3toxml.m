function result=a3toxml(p)
% convert struct to xml element
% fields are reported in order
% .name.ATT gives an attribute


result=printcontent(p);


function result=printcontent(p)
lf=sprintf('\n');
tb=sprintf('\t');

fields=fieldnames(p);
result='';
temp='';
for i=1:length(fields)
    field=fields{i};

    form='';
    temp=getfield(p,field);
    if (isempty( getfield(p,field)))
        continue;
    elseif (ischar( getfield(p,field)))
        form='%s';
    elseif (isnumeric( getfield(p,field)))
        form='%d';
    elseif (isstruct( getfield(p,field)))
        form='%s';
        temp = [lf printcontent( getfield(p,field) )];
    else
        error('Unsupported datatype');
    end
    temp=sprintf(['<%s>' form '</%s>\n'], field,temp, field);
    result=[result temp];

end
