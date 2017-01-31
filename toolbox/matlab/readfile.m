function result=readfile(filename)

if (~exist(filename))
    warning(['Can''t open file ' filename ]);
    return;
end

result='';
fid=fopen(filename);
while 1
    tline = fgets(fid);
    if ~ischar(tline), break, end
    result=[result tline];
end
fclose(fid);