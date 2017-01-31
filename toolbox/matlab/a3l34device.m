function result=a3l34device(id, device_id, implant, trigger, map, volume)
% function result=a3l34device(id, device_id, implant, trigger, map, volume)

if (nargin<6)
    volume=100;
end


result=['<device id="' id '"  xsi:type="apex:L34DeviceType">' lf];
result=[result sprintf('<device_id>%d</device_id>', device_id) lf];
result=[result sprintf('<implant>%s</implant>', implant) lf];
result=[result sprintf('<trigger>%s</trigger>', trigger) lf];
result=[result sprintf('<volume>%d</volume>', volume) lf];
result=[result wraptag('defaultmap', wraptag('inline', map)) lf ];
result=[result '</device>' lf];
