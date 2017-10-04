function result=a3showparams(s)
if isstruct(s)
    fields=fieldnames(s);
    result=[];
    
    for i=1:length(fields)
        result = [ result wraptag('parameter',num2str(getfield(s,fields{i})),'id',fields{i}) sprintf('\n') ];
    end
else
    result = '';
end