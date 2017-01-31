function result=a3calibration(profile_id, stimulus, parameter, target_loudness,p)
% result=a3calibration(profile_id, stimulus, parameters, target_loudness)

if (nargin<5)
    p=struct;
end

p=ef(p,'calibration_amplitude', 75);
p=ef(p,'min', -100);
p=ef(p,'max', 10);
p=ef(p,'autocalibration',struct);

lf=sprintf('\n');

result=['<calibration profile="' profile_id '">' lf];

if (length(fieldnames(p.autocalibration)))
    result=[result a3soundlevelmeter(p.autocalibration)];
end

result=[result '<stimuli>' lf];
result=[result '<stimulus id="' stimulus '" />' lf];
result=[result '</stimuli>' lf];
result=[result '<parameters>' lf];
if (~iscell(parameter))
    parameter={parameter};
end
for i=1:length(parameter)
    result=[result '<parameter id="' parameter{i} '">' lf];
    result=[result '<targetamplitude>' num2str(target_loudness) '</targetamplitude>' lf];
    result=[result '<calibrationamplitude>' num2str(p.calibration_amplitude) '</calibrationamplitude>' lf];
    result=[result '<mute>-150</mute>' lf];
    result=[result '<min>' num2str(p.min) '</min>' lf];
    result=[result '<max>' num2str(p.max) '</max>' lf];
    result=[result '</parameter>' lf];

end

result=[result '</parameters>' lf];
result=[result '</calibration>' lf];


return;