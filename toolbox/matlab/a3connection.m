function result=a3connection(from, fromchan, to, tochan)
% result=a3connection(from, fromchan, to, tochan)


lf=sprintf('\n');
tb=sprintf('\t');


result=['<connection>' lf ];
result=[result tb '<from>' lf ];
result=[result tb '<id>' from '</id>' lf ];
result=[result tb '<channel>' num2str(fromchan) '</channel>' lf ];
result=[result tb '</from>' lf ];
result=[result tb '<to>' lf ];
result=[result tb '<id>' to '</id>' lf ];
result=[result tb '<channel>' num2str(tochan) '</channel>' lf ];
result=[result tb '</to>' lf ];
result=[result tb '</connection>' lf ];
