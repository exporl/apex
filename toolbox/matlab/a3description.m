function result=a3description(text)

if (length(text))
    result=sprintf('<description>\n%s\n</description>\n', text);
else
    result='';
end
