function result=delim

if (isunix)
    result='/';
else
    result='\';
end
