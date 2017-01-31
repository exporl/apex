function result=a3button(id, col, row, text,shortcut)
% a3button(id, col, row,, text)

if (nargin<5)
    shortcut='';
end

lf=sprintf('\n');
tb=sprintf('\t');

result=[ '<button id="' id '" col="' num2str(col) '" row="' num2str(row) '">' lf];
if (~isempty(shortcut))
    result=[result tb '<shortcut>' shortcut '</shortcut>' lf];
end
result=[result tb '<text>' text '</text>' lf];
result=[result '</button>' lf ];
