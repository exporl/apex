function result=a3buttonscreen(labels, text, id, shortcuts)
% result=a3buttonscreen(labels, text, id)
% create an apex3 screen containing buttons according to labels
% shortcuts is optional

if (nargin<4)
    shortcuts=cell(0,0);
else
   if ( (size(shortcuts,1) ~= size(labels,1)) | (size(shortcuts,2) ~= size(labels,2)) )
       error('Invalid shortcut size');
   end
end
if (nargin<3)
    id='screen';
end
if (nargin<2)
    text='';
end

nx=size(labels,1);
ny=size(labels,2);

lf=sprintf('\n');

result=['<screen id="' id '" >' lf];
if (length(text))
    result=[result sprintf('<gridLayout height="2" width="1" id="main_layout">') lf];
    result=[result '<label x="1" y="1" id="helplabel">' lf];
    result=[result '<text>' text '</text>' lf];
    result=[result '</label>' lf];
end

result = [result a3buttonlayout(labels, 1, 2, shortcuts)];

% result=[result sprintf('<gridLayout height="%d" width="%d" id="buttonlayout" x="1" y="2">', ny, nx) lf];
%
% for x=1:nx
%     for y=1:ny
%         if (length(labels{x,y}))
%             result=[result sprintf('<button x="%d" y="%d" id="button%s">',  x, y, labels{x,y}) lf ];
%             if (length(shortcuts))
%                 result=[result wraptag('shortcut', shortcuts{x,y}) lf];
%             end
%             result=[result sprintf('\t<text>%s</text>\n</button>\n', labels{x,y}) lf];
%         end
%     end
% end
%
% result=[result '</gridLayout>' lf];
if (length(text))
    result=[result '</gridLayout>' lf];
end


result=[result a3buttongroup(labels)];

% result=[result '<buttongroup id="buttongroup">' lf];
% for x=1:nx
%     for y=1:ny
%         if (length(labels{x,y}))
%             result=[result sprintf('<button id="button%s"/>\n', labels{x,y}) ];
%         end
%     end
% end
%
%
% result=[result '</buttongroup>' lf];
% result=[result '<default_answer_element>buttongroup</default_answer_element>' lf];
result=[result '</screen>' lf];
