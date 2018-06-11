function result=wraptag(tag,content,attributes,values)
% result=wraptag(tag,content)
if nargin<3
    if ~isempty(content)
        result=sprintf('<%s>%s</%s>', tag, content, tag);
    else
        result=sprintf('<%s />', tag);
    end
else
    result = [];
    if ischar(attributes)
        result = sprintf(' %s="%s"', attributes, values);
    else
        for attributeID=1:length(attributes)
            attribute = attributes{attributeID};
            value = values{attributeID};
            result = sprintf('%s %s="%s"', result, attribute, value);
        end
    end
    if ~isempty(content)
        result=sprintf('<%s%s>%s</%s>', tag, result, content, tag);
    else
        result=sprintf('<%s%s />', tag, result);
    end
end