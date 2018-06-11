function procedure=a3procedureparameters(p,type)
% Create APEX4 procedure/parameters element

TYPE_ADAPTIVE=1;
TYPE_CONSTANT=2;
TYPE_TRAINING=3;

p=ensure_field(p,'vary_noise',0);

% general procedure parametrs
parameters.presentations=1;
parameters.skip=0;
parameters.order='sequential';
parameters.defaultstandard='';
% parameters.choices=1;

if (type==TYPE_ADAPTIVE)
    parameters.nUp=1;
    parameters.nDown=1;
    if (p.vary_noise)
        parameters.adapt_parameter='noisegen_gain';
    else
        parameters.adapt_parameter='gain';
    end
    parameters.start_value=0;
    parameters.stop_after_type='presentations';
    parameters.stop_after=1;
%    parameters.rev_for_mean=6;
    parameters.larger_is_easier='true';
    parameters.repeat_first_until_correct='true';
    parameters.stepsizes='<stepsize begin="0" size="2"/>';
elseif (type==TYPE_CONSTANT)

elseif (type==TYPE_TRAINING)

else
   error('Type not supported');
end


procedure.parameters=parameters;

return;
