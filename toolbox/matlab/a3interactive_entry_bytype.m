function result=a3interactive_entry_bytype(type,description)
% Return an <entry> for use within an <interactive> element

if (nargin<1)
    error('Invalid number of arguments');
end


if (strcmp(type,'start_value'))
    if (nargin<2)
        description='SNR start value';
    end
    repl.description=description;
    result=readfile_replace('a3interactive_startvalue.xml',repl);
elseif (strcmp(type,'level'))
    result=readfile('a3interactive_level.xml');
elseif (strcmp(type,'snr'))
    result=readfile('a3interactive_snr.xml');
elseif (strcmp(type,'subject'))
    result=a3interactive_entry('string', ...
        'Subject name', ...
        '/apex:apex/results/subject', ...
        '' );
else
    error('Invalid type');
end
