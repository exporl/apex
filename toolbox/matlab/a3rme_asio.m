function result=a3rme_asio(channels)
% show APEX 4 device for rme using the asio drivers

if (nargin<1)
    channels=2;
end

lf=sprintf('\n');

result=['<device id="wavdevice" xsi:type="apex:wavDeviceType">' lf];
result=[result '<driver>asio</driver>' lf];
result=[result '<card>default</card>' lf];
result=[result '<channels>' num2str(channels) '</channels>' lf];
result=[result '<gain>0</gain>' lf];
result=[result '<samplerate>44100</samplerate>' lf];
result=[result '<buffersize>4096</buffersize>' lf];
result=[result '</device>' lf];

return;
