function filename=xmlfilename(filename)
% modify filename for use in xml file

for i=1:length(filename)
   if (filename(i)=='\')
       filename(i)='/';
   end
end