function result=a3connection(from, fromchan, to, tochan)
% result=a3connection(from, fromchan, to, tochan)



lf=sprintf('\n');
tb=sprintf('\t');


result=['<connection>' lf ];
result=[result tb '<from>' lf ];
result=[result tb '<id>' from '</id>' lf ];
if iscell(fromchan)
    result=[result tb '<channel id="' fromchan{1} '">' num2str(fromchan{2}) '</channel>' lf ];
else
    result=[result tb '<channel>' num2str(fromchan) '</channel>' lf ];    
end
result=[result tb '</from>' lf ];
result=[result tb '<to>' lf ];
result=[result tb '<id>' to '</id>' lf ];
if iscell(tochan)
    result=[result tb '<channel id="' tochan{1} '">' num2str(tochan{2}) '</channel>' lf ];
else
    result=[result tb '<channel>' num2str(tochan) '</channel>' lf ];
end
result=[result tb '</to>' lf ];
result=[result tb '</connection>' lf ];
