function a3speech_multiprocedure(p)
% create apex 3 speech (in noise) experiment using multiple procedures
% (HABIL)

p.script='a3speech';

p=ensure_field(p, 'procedures',  3);        % number of procedures


% we have one procp per procedure
if (length(p.procp)~=p.procedures)
    error('Invalid p.procp length');
end

p=ensure_field(p,'pact',0);                  % do stuff for PACT study
if (p.pact)
    if (p.procedures~=2)
        error('Invalid number of procedures for pact setup');
    end
end


p=ensure_field(p, 'type',   'vu');        % list, lint, nva, vu
                                          % currently the same for all
                                          % procedures

p=ensure_field(p, 'noise',  1);             % add noise?

for nproc=1:p.procedures

    p.procp{nproc}=ensure_field(p.procp{nproc}, 'list',       1);         % number of list
    p.procp{nproc}=ensure_field(p.procp{nproc},'vu_male', 1);             % use male VU speaker

    if (p.pact)
        p.procp{nproc}=ensure_field(p.procp{nproc},'custom_noise_file', 'noise.wav');             % use male VU speaker
    end

    % adaptive procedure
    p.procp{nproc}=ensure_field(p.procp{nproc},'start_value',-8);
    p.procp{nproc}=ensure_field(p.procp{nproc},'rev_for_mean', 6 );
end

p=ensure_field(p, 'vary_noise',  0);            % vary the noise or the speech?
p=ensure_field(p,'fixed_noise', 0);
p=ensure_field(p,'fixed_snr',0);                % snr, if the noise is not varied

p=ensure_field(p,'manual_correction', 1);       % have the experimentor correct manually?

p=ensure_field(p, 'filename', 'speech.xml');    % filename of generated file

% everything to do with levels
p=ensure_field(p,'gain', -20);
%p=ensure_field(p,'noise_gain', -20);            % noise gain, must not be
                                                 % set, in dB

p=ensure_field(p,'ask_start_level', 0);         % ask for the start level using an interactive tag

if (isfield(p,'add_silence'))
    p=ensure_field(p,'add_silence_before',1);              % add silence to every stimulus
    p=ensure_field(p,'add_silence_after',1);              % add silence to every stimulus
else
    p.add_silence=0;
    p=ensure_field(p,'add_silence_before',0);              % add silence to every stimulus
    p=ensure_field(p,'add_silence_after',0);              % add silence to every stimulus
end
p=ensure_field(p,'silence_len',0.5);            % length of silence, in seconds


p=ensure_field(p,'rme_asio', 0);                % use RME with asio drivers


% constants     WARNING: paths must end in /
p=ensure_field(p, 'LIST_path', '/data/woordenlijst/wivine/');
p=ensure_field(p, 'LIST_noise_file', '/calibration/wivineruis.wav');

p=ensure_field(p, 'VU_path', '/data/woordenlijst/vu/');
% if (p.vu_male)
     p=ensure_field(p, 'VU_noise_file', 'noise/man.wav');
% else
     p=ensure_field(p, 'VU_noise_file', 'noise/woman.wav');
% end


p=ensure_field(p, 'NVA_path', '/data/woordenlijst/nva/');

%p.custom_noise_file='bla.wav';                        % custom noise file


p=derive_params(p);


if (isunix)
    delim='/';
else
    delim='\';
end


warning('Equal sentence material assumed for each procedure');  % FIXME
t_answers=cell(p.procedures,1);
all_answers=cell(0,0);
filelist=cell(0,0);
trialnames=cell(p.procedures,1);
screennames=cell(p.procedures,1);
stimulinames=cell(p.procedures,1);

for nproc=1:p.procedures

    if (strcmp(p.type, 'list'))
        % generate list of filenames
        prefix=[p.LIST_path ];
    %     d=dir( [prefix '*.wav']);
    %     list=dir2list(d);

        tfilelist=getLISTfiles(p.procp{nproc}.list);

        for c=1:length(tfilelist)
           tfilelist{c} = ['lijst' num2str(p.procp{nproc}.list) delim tfilelist{c} ];
        end

        filelist=cat(2,filelist, tfilelist );
        p.noisefile=p.LIST_noise_file;


        t_answers{nproc}=getLISTanswers(p.procp{nproc}.list,0);

    elseif (strcmp(p.type, 'lint'))

    elseif (strcmp(p.type, 'nva'))
        prefix=[p.NVA_path num2str(p.list) delim];
        [tfilelist,t_answers{nproc}]=getNVAfiles(p.procp{nproc}.list);

        filelist=cat(2,filelist, tfilelist);

    elseif (strcmp(p.type, 'vu'))

       [tfilelist,t_answers{nproc} ]=getVUfiles(p.procp{nproc}.list,p.procp{nproc}.vu_male);

       if (p.procp{nproc}.vu_male)
           bla='man';
       else
           bla='woman';
       end
       % add prefix to every file
       for l=1:length(tfilelist)
           tfilelist{l}=[bla '/' tfilelist{l}];
       end

       filelist=cat(2,filelist, tfilelist);


       prefix=p.VU_path;
       p.noisefile=p.VU_noise_file;

    else
        error('Unsupported type');
    end

    all_answers=cat(2,all_answers,t_answers{nproc});
    trialnames{nproc} = gentrialnames(tfilelist);
    screennames{nproc}= repmat( {'screen'}, length(trialnames{nproc}), 1);
    stimulinames{nproc}=genstimulinames(tfilelist);

end

if (isfield(p,'custom_noise_file'))
    p.noisefile=p.custom_noise_file;
end


allstimulinames=genstimulinames(filelist);
datablocknames=gendatablocknames(filelist);

% make the correct answer a fixed parameter

fixparams=cell(size(allstimulinames,1),size(allstimulinames,2));
varparams=cell(size(allstimulinames,1),size(allstimulinames,2));
for i=1:length(allstimulinames)
    fixparams{i}.sentence=all_answers{i};
end

allcount=1;
for nproc=1:p.procedures
   for c=1:length(stimulinames{nproc})
       fixparams{allcount}.procedure = num2str(nproc);
       fixparams{allcount}.list = num2str(p.procp{nproc}.list);
       varparams{allcount}.sentencechannel=num2str(nproc-1);    % zero based

       if (p.pact)
           if (nproc==1)
                varparams{allcount}.dataloop1channel=1;
                varparams{allcount}.dataloop2channel=-1;     % do not play
           elseif (nproc==2)
                varparams{allcount}.dataloop1channel=-1;     % do not play
                varparams{allcount}.dataloop2channel=0;
           else
               error('internal error');
           end
       end
        allcount=allcount+1;
   end
end

stimuli=a3stimuli(datablocknames, allstimulinames,varparams,fixparams,p.add_silence_before,p.add_silence_after);

% add noise datablock if necessary
if (p.noise)
    % add a noise datablock for each procedure
    for nproc=1:p.procedures
        if (p.pact)
            p.noisefile=p.procp{nproc}.custom_noise_file;
        end
        filelist=cat(2,filelist,{p.noisefile});
        datablocknames=cat(2,datablocknames,{['noisedata' num2str(nproc)]});
    end
end
% add silence datablock if necessary
if (p.noise)
    filelist=cat(2,filelist,{['silence:' num2str(round(p.silence_len*1000))]});
    datablocknames=cat(2,datablocknames,{'silence'});
end

datablocks=a3datablocks(filelist, prefix, datablocknames);



%% screens %%
if (p.manual_correction)
    screens=a3screens( a3screen('multiproc',p.vary_noise) );
else
    screens=a3screens( a3screen('opc') );
end


lf=sprintf('\n');
procedure=[ '<procedure xsi:type="apex:multiProcedureType">' lf];
procedure=[ procedure  '<parameters>' lf];
procedure=[ procedure '<order>random</order>' lf];
procedure=[ procedure '</parameters>' lf lf];


for nproc=1:p.procedures


if (p.manual_correction)
    answers=repmat({'correct'}, size(trialnames{nproc},1),size(trialnames{nproc},2));
else
    answers=t_answers{nproc};
end


    trials=a3trials(trialnames{nproc}, answers, screennames{nproc}, stimulinames{nproc});

    if (p.noise && p.fixed_noise==0)        % use adaptive procedure
        proceduretype=1;
    else                % use constant procedure
        proceduretype=2;
    end

    procedureparameters=a3procedureparameters(p.procp{nproc},proceduretype);
    if (p.noise && p.fixed_noise==0)
        procedureparameters.parameters.start_value=p.procp{nproc}.start_value;
        procedureparameters.parameters.rev_for_mean=p.procp{nproc}.rev_for_mean;
    end
    procedureparameters=a3toxml(procedureparameters);
    procedure=[procedure a3procedure(procedureparameters,trials,proceduretype, ['procedure' num2str(nproc)]) lf];


end

    procedure=[procedure '</procedure>'];


    corrector=a3corrector('isequal');

    if (p.rme_asio)
       devices=['<devices>' lf];
       if (p.pact)
           chans=p.procedures;        % use one channel to divert unneeded dataloop
       else
           chans=p.procedures;
       end
       devices=[devices a3rme_asio(chans) lf];   % rme asio device with p.procedures channels
       devices=[devices '</devices>' lf];
    else
        devices=a3devices();
    end

%     if (p.pact)
%         keepnproc=p.procedures;
%         p.procedures=1;
%     end
    if (p.noise)
        if (p.vary_noise)
           connections=a3multiprocedureconnections(p,'');
        else
            connections=a3multiprocedureconnections(p,'speech');
        end
    else
        connections=a3multiprocedureconnections(p,'amplifier');
    end
%     if (p.pact)
%         p.procedures=keepnproc;
%     end



%% filters %%
% filterlist=cell(0,0);
% if (p.noise)
%     filterlist=cat(1,filterlist,{'dataloop'});
%     filterlist=cat(1,filterlist,{'amplifier'});
% else
%     filterlist=cat(1,filterlist,{'amplifier'});
% end


if (p.fixed_noise)
    if (p.vary_noise)
        error('Not implemented');
    else
        p.basegain_speech=p.basegain_speech+p.fixed_snr;
    end
end

%filters=a3filters( filterlist, p );

%% add nproc dataloops
filters = ['<filters>' lf];
filters = [filters a3amplifier(p.basegain_speech) lf];
% if (p.pact)
%     tnum=1;
% else
    tnum=p.procedures;
% end
for nproc=1:tnum
    filters = [filters a3dataloop(p.noisefile,p.basegain_dataloop, ['noisegen' num2str(nproc) ], ['noisedata' num2str(nproc)])];
end
filters = [filters '</filters>' lf];


% create interactive stuff
if (p.ask_start_level)
    interactive=['<interactive>' lf];
    for nproc=1:p.procedures
        interactive=[interactive '<entry type="int" description="SNR start value procedure ' num2str(nproc) ' " expression="/apex:apex/procedure/procedure[' num2str(nproc) ']/parameters/start_value" default="-10" />' lf ];
    end
    interactive=[interactive '</interactive>' lf];

else
   interactive = '';
end



%% write everything to file in correct order %%
F = fopen(p.filename, 'w');
if (F==-1)
   error(['Can''t open file for writing: ' p.filename]);
end
fprintf(F,'%s', a3header() );
fprintf(F,'%s', procedure);
fprintf(F,'%s', corrector );
fprintf(F, screens );
fprintf(F,'%s', datablocks );
fprintf(F,'%s', devices );
fprintf(F,'%s', filters );
fprintf(F,'%s', stimuli );
fprintf(F,'%s', connections );
fprintf(F,'%s', interactive );
fprintf(F,'%s', a3footer() );

fclose(F);


function list=dir2list(d)

list=cell(size(d));
for i=1:length(d)
    list{i}=d(i).name;
end


function names=gendatablocknames(files)
% create datablock names from filenames
names=gennames(files, 'datablock_');


function names=genstimulinames(files)
% create stimulus names from filenames
names=gennames(files, 'stimulus_');

function names=gentrialnames(files)
% create datablock names from filenames
names=gennames(files, 'trial_');

function names=gennames(files, prefix)
% create datablock names from filenames
names=cell(size(files));
for i=1:length(files)
    names{i}=[prefix files{i}(1:end-4) ];

    % remove invalid xml name chars from names FIXME
    for ch=1:length(names{i})
       if (names{i}(ch) == '/' || names{i}(ch) == '\')
           names{i}(ch) = '_';
       end
    end
end




function list=getLISTfiles(listnr)

    if (isunix)
        load /home/tom/matlab/zinnen/zinnen.mat
        load /home/tom/matlab/zinnen/selectzinnen.mat
        load /home/tom/matlab/zinnen/lijstjes.mat
    else
        load zinnen.mat
        load selectzinnen.mat
        load lijstjes.mat
    end


     list=ZinnenLists(listnr,:);


function [list,answers]=getVUfiles(listnr,male)

if (male)
    listfile='/data/woordenlijst/vu/man/ManZin.txt';
else
    listfile='/data/woordenlijst/vu/woman/VrouwZin.txt';
end

fid=fopen(listfile);
if (fid==-1)
    error(['Can''t open ' listfile]);
end

list=cell(0,0);
answers=cell(0,0);

found=0;
while (1)
    tline = fgetl(fid);
    if ~ischar(tline), break, end

    r='^\s+(\d+)\s+(\d+)\s+(\d+)\s+(.*)$';

    [s,f,t]=regexp(tline,r);
    if (length(t)==0)
        continue;
    end

    lijst=str2num(tline(t{1}(1,1):t{1}(1,2)));
    index=str2num(tline(t{1}(2,1):t{1}(2,2)));
    zinnr=str2num(tline(t{1}(3,1):t{1}(3,2)));
    zin=tline(t{1}(4,1):t{1}(4,2));

    if (zin(end)=='\n')
        zin=zin(1:end-1);
    end
    if (zin(end)=='\r')
        zin=zin(1:end-1);
    end

    if (lijst==listnr)
       answers=cat(2, answers, {zin} );

       if (male)
           filename='man';
       else
           filename='vrouw';
       end
       filename=[filename sprintf('%03d', zinnr) '.wav'];
       list=cat(2, list, {filename} );

       found=1;
    elseif (found)
        break;
    end

end

fclose(fid);


function [list,answers]=getNVAfiles(listnr)
% zoek lijst op
if (isunix)
        load /home/tom/matlab/zinnen/nva.mat
else
    error('Not implemented');
end

answers=nvawoorden(listnr,:);
list=cell(size(answers,1),size(answers,2));
for i=1:length(answers)
   list{i}=[answers{i} '.wav'];
end


function answers=getLISTanswers(listnr,addmarkup)
% addmarkup determines whether to label keywords etc.

    if (isunix)
        load /home/tom/matlab/zinnen/zinnen.mat
        load /home/tom/matlab/zinnen/selectzinnen.mat
        load /home/tom/matlab/zinnen/splitsbaar.mat
        load /home/tom/matlab/zinnen/lijstjes.mat
    else
       load zinnen.mat
        load selectzinnen.mat
        load splitsbaar.mat
        load lijstjes.mat
    end

    list=ZinnenLists(listnr,:);

    answers=cell(size(list));

    for c=1:length(list)
         p='(\d+).wav';

        [s,f,t]=regexp(list{c},p);
        if (length(t)==0)
            error('Can''t parse sentence list');
        end

        nr=str2num(list{c}(t{1}(1):t{1}(2)));

        if (addmarkup)
            result=xmlzin(zinnen{nr},selectzinnen{nr},splitsbaar{nr});
        else
            result=zinnen{nr};
        end
        answers{c}=result;
    end


function p=derive_params(p)
    if (p.noise)
        p.basegain_dataloop=p.gain-getnoisegain(p);
    end
    p.basegain_speech=p.gain-getspeechgain(p);

    return;


function g=getnoisegain(p)

% >> noise=wavread('/home/tom/data/apex/apex/tests/regression/stimuli/wivineruis.wav');
% >> rmsdb(noise)
%
% ans =
%
%   -25.4049

if (isfield(p,'noise_gain'))
   g=p.noise_gain;
else

    if (strcmp(p.type,'list'))
        g=-25;
    elseif (strcmp(p.type,'vu'))
        g=-24;
    else
        disp('Calculating noise gain...');
        r=wavread(p.noisefile);
        g=rmsdb(r);
        disp(g);
    end

    warning('fixed noise intensity');
end
return;

function g=getspeechgain(p)

if (strcmp(p.type,'list'))
    g=-25;
elseif (strcmp(p.type,'vu'))
%     if (p.vu_male)
%         g=-23.8223;
%     else
%         g=-21.6182;
%     end

    g=-24;      % idem as VU noise ???

    warning('check VU speech gains');
elseif (strcmp(p.type,'nva'))
    g=-25.9686;
else
    error('Can''t get speech gain for this type');
end
return;
