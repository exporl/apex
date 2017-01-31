function result=a3randomgenerator(id, minimum, maximum, type, parameter)
% result=a3randomgenerator(id, minimum, maximum, type, parameter)

lf=sprintf('\n');

result=['<randomgenerator xsi:type="apex:uniform" id="' id '">'];
result=[result lf sprintf('<min>%d</min>', minimum) ];
result=[result lf sprintf('<max>%d</max>', maximum) ];
result=[result lf wraptag('type', type) ];
result=[result lf wraptag('parameter', parameter) ];
result=[result '</randomgenerator>'];