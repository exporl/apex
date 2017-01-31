function [result,factor]=a3easwait(rate,setup)
if nargin<2
    setup=0;
end
% result=a3easwait(rate)
%
% result: delay
% factor: correction factor for rate:  goodrate=oldrate*factor

%% -21ppm, RME3
if strcmp(setup,'synapse')
    waitlen=294.1+ 502.5e-3; %% Anneke: 11/11/11
else
    waitlen=294.1+ 547e-3;
end
% waitlen=294.1+ 497.5e-3; %% Anneke: 11/11/11
% if (rate==6300)
% %    waitlen=649.61;
% %    waitlen=649.78;     % meting op electrode 2
% %    waitlen=649.98;      % pulsewidth=20, meting op elec1
% %    waitlen=649.7;      % meting op elec 22, drift gecompenseerd met -446ppm
% %     waitlen=284.622;     % meting 5/2/2009
%
%      waitlen=294.1 + 547e-3;        % theoretische waarde + praktisch verschil (processing delay implant?)
%
%      %% Factor in L34: -445.0378282
% elseif (rate==7200)
% %    waitlen=565;
% %    waitlen=569.97;     % voor pulsewidth=20 van pulsegen
% %    waitlen=569.7;      % meting op elec 22, drift gecompenseerd met
% %    -446ppm
%    waitlen=294.1 + 547e-3;        % theoretische waarde + praktisch verschil (processing delay implant?)
%
% elseif (rate==9600)
%    waitlen=294.1 + 547e-3;
% elseif (rate==10800)
%     warning('Fix elwait');
%
%    waitlen=294.1 + 547e-3;
% else
%     error('Unknown rate');
% end

lf=sprintf('\n');

    result='<datablock id="waitforl34">';
    result=[result lf '<device>soundcard</device>'];
     result=[result lf '<uri>silence:' num2str(waitlen) '</uri>'];
   result=[result lf '</datablock>'];