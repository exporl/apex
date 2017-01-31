function result=a3l34device_bilateral(mapL, mapR)
% return 2 L34 devices,

lf=sprintf('\n');

result=['<master>L34-L</master>' lf ];
result=[result a3l34device('L34-L', 1, 'cic3', 'out', mapL) lf ...
               a3l34device('L34-R', 2, 'cic3', 'in', mapR) ];
