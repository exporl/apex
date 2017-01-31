function result=a3multiprocedureconnections(p, type)

nprocs=p.procedures;

result=readfile('multiprocedureconnections_header.xml');

lf=sprintf('\n');

for i=1:nprocs

    add=['<connection>' lf	'<from>'];
    add=[ add '<id>noisegen' num2str(i) '</id>' lf];
    add=[ add '<channel>0</channel>' lf];
    add=[ add '</from>' lf];
    add=[ add '<to>' lf];
    add=[ add '<id>wavdevice</id>' lf];
    add=[ add '<channel id="dataloop' num2str(i) 'channel">' num2str(i-1) '</channel>' lf];
    add=[ add '</to>' lf];
    add=[ add '</connection>' lf];
    result=[result add];
end

result=[ result '</connections>' lf];
