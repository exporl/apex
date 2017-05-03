function result=a3procedure(parameters, trials,type, id, apexversion)
% type 1 = adaptive procedure
% type 2 = constant procedure
% type 2 = training procedure

if (nargin<4 || isempty(id))
    id='';
else
    id=[' id= "' id '" '];
end

if (nargin<5)
    apexversion='3.0';
end

if (isstruct(parameters))
    parameters=a3toxml(parameters);
end

lf=sprintf('\n');
tb=sprintf('\t');

% switch apexversion
if regexp(apexversion,'3.0')

        if (type==1)
            result=['<procedure xsi:type="apex:adaptiveProcedureType"' id '>' lf ];
        elseif (type==2)
            result=['<procedure xsi:type="apex:constantProcedureType"' id '>' lf ];
        elseif (type==3)
            result=['<procedure xsi:type="apex:trainingProcedureType"' id '>' lf ];
        end

elseif regexp(apexversion,'3.1')

         if (type==1)
            result=['<procedure xsi:type="apex:adaptiveProcedure"' id '>' lf ];
        elseif (type==2)
            result=['<procedure xsi:type="apex:constantProcedure"' id '>' lf ];
        elseif (type==3)
            result=['<procedure xsi:type="apex:trainingProcedure"' id '>' lf ];
        end

end

result=[result parameters lf trials lf '</procedure>' lf];
