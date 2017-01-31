function result=a3gridlayout(width,height,content)

result=sprintf( '<gridLayout width="%g" height="%g">\n', width, height);
result=[result content '</gridLayout>'];
