function result=a3amplifier(basegain, id, device,channels,gainids)
% result=a3amplifier(basegain, id, device,channels,gainids)

if (nargin<1)
    basegain=0;
end
if (nargin<2)
    id='amplifier';
end
if (nargin<3)
    device='wavdevice';
end
if (nargin<4)
    channels=1;
end
if (nargin<5)
    gainids=cell(0,0);
end

%result=readfile('templates/amplifier.xml');

lf=sprintf('\n');
tb=sprintf('\t');

result= [ '<filter xsi:type="apex:amplifier" id="' id '" >' lf ];
result= [ result tb '<device>' device '</device>' lf];
result= [ result tb '<channels>' num2str(channels) '</channels>' lf];
result= [ result tb '<basegain>' num2str(basegain) '</basegain>' lf];
if (channels==1 || length(gainids)==0 )
    result= [ result tb '<gain id="gain">0</gain>' lf ];
else
    if (length(gainids)==1)
            result= [ result tb '<gain id="' gainids{1} '">0</gain>' lf ];
    else

        for i=1:length(gainids)
            result= [ result tb sprintf('<gain id="%s" channel="%d">0</gain>', gainids{i}, i-1) lf ];
        end
    end
end
result= [ result '</filter>' lf];

return;
