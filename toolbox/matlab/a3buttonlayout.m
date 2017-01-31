function result=a3buttonlayout(labels, col, row, shortcuts)
% result=a3buttonscreen(labels, text, id)
% create an apex3 screen containing buttons according to labels
% shortcuts is optional
% x and y are the position of the layout itself

if (nargin<3)
    ystr='';
else
   ystr=sprintf('row="%d"', row);
   y=row;
end

if (nargin<2)
    xstr='';
else
   xstr=sprintf('col="%d"', col);
   x=col;
end

if (nargin<4)
    shortcuts=cell(0,0);
else
    if (length(shortcuts)>0)
        if ( (size(shortcuts,1) ~= size(labels,1)) | (size(shortcuts,2) ~= size(labels,2)) )
        error('Invalid shortcut size');
        end
    end
end

nx=size(labels,1);
ny=size(labels,2);

lf=sprintf('\n');



result=[sprintf('<gridLayout height="%d" width="%d" id="buttonlayout" %s %s>', nx, ny, xstr, ystr) lf];

for x=1:nx
    for y=1:ny
        if (length(labels{x,y}))
            result=[result sprintf('<button row="%d" col="%d" id="button%s">',  x, y, labels{x,y}) lf ];
            if (length(shortcuts))
                result=[result wraptag('shortcut', shortcuts{x,y}) lf];
            end
            result=[result sprintf('\t<text>%s</text>\n</button>\n', labels{x,y}) lf];
        end
    end
end

result=[result '</gridLayout>' lf];
