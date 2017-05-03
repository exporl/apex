function result=readpartfile(filename,start,stop)
% read the part of a file between a start and end string

if (~exist(filename, 'file'))
    % look in all data paths
    paths=getpaths;
    paths=paths.results;
    for i=1:length(paths)
        newpath=fixpath([paths{i} '/' filename ]);
       if (exist( newpath ,'file'))
           filename=newpath;
       end
    end
end


FID = fopen(filename,'r');
if (FID==-1)
    error(['Kan ' filename ' niet openen']);
end

line = fgetl(FID);

found=0;
result=cell(0,0);
while( ischar(line) )
    
    if (found)
        I=strfind(line,stop);
        if ( ~isempty(I) )
            if (I>1)
               result=cat(1,result, {line(1:I-1)});
            end
            break;
        else
            result=cat(1,result, {line});
            line = fgetl(FID);
            continue;
        end
    end
    
   
    I=strfind(line,start);
    if ( ~isempty(I) )
        found=1;
        if (I+length(start)<length(line))
              result=cat(1,result, {line(I+length(start):end)});
        end
        
        In=strfind(line,stop);
        if ( ~isempty(In) )
            if (In>1)
               result{end}=line(I+length(start):In-1);
            end
            break;
        end
    end
    
    line = fgetl(FID);
    
end


fclose(FID);

