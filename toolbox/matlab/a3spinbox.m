function result=a3spinbox(id, x, y, value, min, max, step, parameter)

result=sprintf('<spinBox id="%s" x="%g" y="%g">\n', id, x, y);
result=[result wraptag('value', num2str(value)) lf];
result=[result wraptag('min', num2str(min)) lf];
result=[result wraptag('max', num2str(max)) lf];
result=[result wraptag('step', num2str(step)) lf];
result=[result wraptag('parameter', parameter) lf];
result=[result '</spinBox>'];
