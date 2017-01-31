function result=a3twopartlayout(part1,part2, ratio, direction)
% result=a3twopartlayout(part1,part2, ratio, direction)
% direction= vertical|horizontal

if (nargin<4)
    direction='vertical';
end
if (nargin<3)
    ratio=0.2;
end
if (nargin<2)
    error('Parts not defined');
end


s1=ratio*100;
s2=(100-ratio*100);
stretchfactor=sprintf('%d,%d', s1, s2);

if (strcmp(direction,'horizontal'))
    result=sprintf('<gridLayout columnstretch="%s" width="2" height="1">', stretchfactor);
else
    result=sprintf('<gridLayout rowstretch="%s" width="1" height="2">', stretchfactor);
end
result=[result lf part1 lf part2];
result=[result lf '</gridLayout>'];
