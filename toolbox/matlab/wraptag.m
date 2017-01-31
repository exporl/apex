function result=wraptag(tag,content)
% result=wraptag(tag,content)
result=sprintf('<%s>%s</%s>', tag, content, tag);
