function result=a3interactive_entry(fieldtype, description, expression, default)
% Return an <entry> for use within an <interactive> element

if (nargin<4)
    error('Invalid number of arguments');
end

if (isnumeric(default))
    default=num2str(default);
end

result=sprintf('<entry type="%s" description="%s" expression="%s" default="%s"/>', ...
    fieldtype, description, expression, default);
