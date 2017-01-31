function result=a3arclayout(width,content,type)
% result=a3arclayout(width,content,type)

if (nargin<3)
    type='upper';
end

result=sprintf( '<arcLayout width="%g" type="%s">\n', width, type);
result=[result content '</arcLayout>'];
