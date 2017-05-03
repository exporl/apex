function temp=a3datablock(id,filename,device,channels);
% result=a3datablock(id,filename,device,chanenls);

if (nargin<4)
    channels=0;
end

if (nargin<3)
    device='';
end

lf=sprintf('\n');
tb=sprintf('\t');

temp=['<datablock id="' id '">' lf];
if (~isempty(device))
    temp=[temp tb '<device>' device '</device>' lf];
end
temp=[temp tb '<file>' xmlfilename(filename) '</file>' lf];
if (channels)
   temp=[temp wraptag('channels', num2str(channels))];
end
temp=[temp '</datablock>' lf];
