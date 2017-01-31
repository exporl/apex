function [trials, datablocks, stimuli,screen]=a3parsedir(path)
% parse directory on disk and generate trials, datablocks and stimuli


trials=[];
datablocks=[];
stimuli=[];

list=cell(0,0);

% read wav files
files=dir(path);
for i=1:length(files)
    filename=files(i).name;
    if (length(filename)<4)
        continue;
    end
    basename=filename(1:end-3);
    ext=filename(end-2:end);

    if (strcmp(ext,'wav'))
        device='soundcard';
    elseif (strcmp(ext,'qic'))
        device='l34device';
    else
        disp(['I don''t recognize the file ' filename]);
        continue;
    end

    trial=['t' basename];
    datablock=['d' basename];
    stimulus=['s' basename];
    button=['button' basename];

    trials=[trials a3trial(trial, 'screen', stimulus, button)];
    datablocks=[datablocks a3datablock(datablock, filename, 'soundcard')];
    stimuli=[stimuli a3stimulus(stimulus, datablock)];

    list=cat(1, list, {basename});
end

num=ceil(sqrt(length(list)));
buttons=cell(num,num);
for i=1:length(list)
   buttons{floor(i/num)+1, mod(i,num)+1 } = list{i};
end

screen=a3buttonscreen(buttons);
