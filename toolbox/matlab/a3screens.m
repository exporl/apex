function result=a3screens(screens, screen)
% result=a3screens(screens, screen)
% screens is a list of xml screen descriptions
% screen is a text screen

if (nargin<2)
    screen='';
end

lf=sprintf('\n');
tb=sprintf('\t');

result=[ '<screens>' lf '<reinforcement>' lf tb '<progressbar>true</progressbar>' lf tb '<feedback length="300">false</feedback>' lf '</reinforcement>' lf ];


if (~length(screens))
    result=[result screen];
else

    for i=1:length(screens)
        result=[result screens(i)];
    end

end


result=[result '</screens>' lf ];

return;
