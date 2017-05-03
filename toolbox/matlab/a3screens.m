function result=a3screens(screens, screen, feedback_length)
% result=a3screens(screens, screen)
% screens is a list of xml screen descriptions
% screen is a text screen

if (nargin<2)
    screen='';
end

if (nargin<3)
    feedback_length=300;
end

lf=sprintf('\n');
tb=sprintf('\t');

result=[ '<screens>' lf '<reinforcement>' lf tb '<progressbar>true</progressbar>' lf tb '<feedback length="' num2str(feedback_length) '">false</feedback>' lf '</reinforcement>' lf ];


if (~length(screens))
    result=[result screen];
else

    for i=1:length(screens)
        result=[result screens(i)];
    end

end


result=[result '</screens>' lf ];

return;
