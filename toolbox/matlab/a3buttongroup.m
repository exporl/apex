function result=a3buttongroup(labels)
% result=a3buttongroup(labels)
% create buttongroup containing all buttons from cell labels


nx=size(labels,1);
ny=size(labels,2);

result=['<buttongroup id="buttongroup">' lf];
for x=1:nx
    for y=1:ny
        if (length(labels{x,y}))
            result=[result sprintf('<button id="button%s"/>\n', labels{x,y}) ];
        end
    end
end


result=[result '</buttongroup>' lf];
result=[result '<default_answer_element>buttongroup</default_answer_element>' lf];