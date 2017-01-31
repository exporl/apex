function result=a3dataloop(datablock_id,basegain,id,continuous,wavdevice,invertgain)

if (nargin<6)
    invertgain=0;
end

if (nargin<5)
    wavdevice='wavdevice';
end

if (nargin<4)
    continuous=0;
end

if (nargin<3)
    id='noisegen';
    gainid='noisegain';
else
   gainid=[id '_gain'];
end


if (nargin<2)
    basegain=0;
end

lf=sprintf('\n');
tb=sprintf('\t');

result=[ '<filter xsi:type="apex:dataloop" id="' id '">' lf tb '<device>' wavdevice '</device>' lf tb '<channels>1</channels>' lf tb ];
result=[result '<continuous>' bool2xml(continuous) '</continuous>' lf tb];
result=[result '<datablock>' datablock_id '</datablock>' lf];
result=[result tb '<basegain>' num2str(basegain) '</basegain>' lf];
if (invertgain)
    result=[result tb '<invertgain>true</invertgain>' lf];
end
result=[result tb '<gain id="' gainid '">0</gain>' lf tb '<randomjump>true</randomjump>' lf tb '</filter>' lf];

return;
