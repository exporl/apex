function result=a3screen(type,param1)
% type= opc
%
% type=mao
%   param1 = vary_noise
%
% type=afc
%   param1 = number of choices

lf=sprintf('\n');
tb=sprintf('\t');


result='';

if (strcmp(type, 'opc'))
   result=readfile('opcscreen.xml');
elseif (strcmp(type, 'mao'))
   result=readfile('maoscreen-header.xml');
   if (param1)
       result=[result '<parameter name="snr" expression="-1*x+0">noisegain</parameter>'];
   else
       result=[result '<parameter name="snr">gain</parameter>'];
   end
   result=[result readfile('maoscreen-footer.xml')];

elseif (strcmp(type, 'multiproc'))
   result=readfile('maoscreen-header.xml');
   if (param1)
       result=[result '<parameter name="snr" expression="-1*x+0">noisegain</parameter>'];
   else
       result=[result '<parameter name="snr">gain</parameter>'];
   end
   result=[result lf '<parameter name="procedure">procedure</parameter>'];
   result=[result lf '<parameter name="sentence list">list</parameter>'];
   result=[result lf '<parameter name="loudspeaker" expression="1*x+1">sentencechannel</parameter>'];
   result=[result readfile('maoscreen-footer.xml')];


elseif (strcmp(type, 'afc'))        % n alternatives forced choice

    if (nargin<2)
        error('Number of buttons not defined for afc');
    end

    result=[result '<screen id="screen" >' lf];
    result=[result '<gridLayout height="2" width="1" id="main_layout">' lf];
    result=[result '<label x="1" y="1" id="helplabel">' lf];
    result=[result '<text>In what position was the stimulus?</text>' lf];
    result=[result '</label>' lf];
    result=[result '<gridLayout height="1" width="' num2str(param1) '" id="answer_layout" x="1" y="2">' lf];

    for i=1:param1
        result=[result '<button x="' num2str(i) '" y="1" id="button' num2str(i) '">' lf];
        result=[result '<text>' num2str(i) '</text>' lf];
        result=[result '</button>' lf];
    end

    result=[result '</gridLayout>' lf];
    result=[result '</gridLayout>' lf];

    result=[result '<buttongroup id="buttongroup">' lf];
    for i=1:param1
        result=[result tb '<button id="button' num2str(i) '"/>' lf];
    end
    result=[result '</buttongroup>' lf];

    result=[result '<default_answer_element>buttongroup</default_answer_element>' lf];
    result=[result '</screen>' lf];
elseif (strcmp(type, 'lint'))
   result=readfile('lintscreen.xml');
else
    error('Unsupported screen type');
end

return;
