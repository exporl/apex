function result=a3datablocks(list, prefix, names, mcprefix)
% list: list of filenames
% prefix: file prefix
% mcprefix: use prefix from main configfile with id=mcprefix
%           the prefix parameter will be ignored when using this option

if (nargin<4)
    mcprefix='';
end

if (nargin<3)
    names=list;
end

if (nargin<2)
    prefix='';
else
    prefix=makedirend(prefix);
end

if (length(names)~=length(list))
    error('Number of names incorrect');
end

lf=sprintf('\n');
tb=sprintf('\t');
result=['<datablocks>' lf ];

if (length(mcprefix))
    result=[result '<uri_prefix source="configfile">' mcprefix '</uri_prefix>' lf];
elseif (length(prefix))
    result=[result '<uri_prefix>file:' xmlfilename(prefix) '</uri_prefix>' lf];
end

for iF=1:length(list)
   filename= list{iF};
   fullfilename= [prefix filename];

   temp=['<datablock id="' names{iF} '">' lf];
   temp=[temp tb '<device>wavdevice</device>' lf];
   temp=[temp tb '<uri>' xmlfilename(filename) '</uri>' lf];
   temp=[temp '</datablock>' lf];

   result=[result temp];
end


result=[result '</datablocks>'];

return;
