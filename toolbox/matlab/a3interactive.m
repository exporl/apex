function result=a3interactive(type,description)


if (nargin>1)
   type=a3interactive_entry_bytype(type,description);
end

result=wraptag('interactive', type);
