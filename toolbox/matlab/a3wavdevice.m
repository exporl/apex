function result=a3wavdevice(id,chanids)
% result=a3wavdevice(id,chanparam)
% chanparam: define channel parameters ID's (id=cardgain%d)

if (nargin<2)
    chanids={};
end
if (nargin<1)
    id='wavdevice';
end
if (nargin<3)
    blocksize=8192;
end

%result=readfile('wavdevice.xml');

lf=sprintf('\n');

result=['<device id="' id '" xsi:type="apex:wavDeviceType">'];
result=[result '<driver>portaudio</driver>' lf];
result=[result '<card>default</card>' lf];
result=[result '<channels>2</channels>' lf];
if (~isempty(chanids))
    for count=1:length(chanids)
        result=[result sprintf('<gain id="%s" channel="%d">0</gain>',chanids{count},count-1)  lf];
    end
else
    result=[result '<gain id="soundcardgain">0</gain>' lf];
end
result=[result '<samplerate>44100</samplerate>' lf];
%result=[result '<blocksize>' num2str(blocksize) '</blocksize>' lf];
%result=[result '<padzero>1</padzero>' lf];
result=[result '</device>' lf];
