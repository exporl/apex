function a3speech(p)
% create APEX 4 speech (in noise) experiment
%

p.script='a3speech';

p=ensure_field(p, 'apexversion', '3.1.1');
p=ensure_field(p, 'type',   'vu');        % list, list_man, lint, nva, vu
p=ensure_field(p, 'list',       1);         % number of list

p=ensure_field(p,'vu_male', 1);             % use male VU speaker

p=ensure_field(p,'lint_speaker', 'wd');     % speaker to be used with lint (wd, md, jw, ag)

p=ensure_field(p, 'noise',  1);             % add noise?
p=ensure_field(p,'continuous_noise', 1);        % keep playing noise between trials
p=ensure_field(p, 'vary_noise',  0);            % vary the noise or the speech?
p=ensure_field(p,'fixed_noise', 0);
p=ensure_field(p,'fixed_snr',0);                % snr, if the noise is not varied

p=ensure_field(p,'manual_correction', 1);       % have the experimentor correct manually?

p=ensure_field(p, 'filename', 'speech.apx');    % filename of generated file

if (p.prefix_from_main==0)
    warning('Prefix not from main configfile');
end
p=ensure_field(p,'prefix_from_main', 1);        % use the prefix from main configfile
% prefix_name                                  % if using prefix from main
% configfile, you can specify the ID in prefix_name

% everything to do with levels
p=ensure_field(p,'gain', -20);
%p=ensure_field(p,'noise_gain', -20);            % noise gain, must not be
                                                 % set, in dB

p=ensure_field(p,'calibration_profile', '');  % id of calibration profile to be used
                                              % empty if no calibration is
                                              % to be used
p=ensure_field(p,'target_calibration', 65);     % calibrate to 65dBSPL

if (isfield(p,'add_silence'))
    p=ensure_field(p,'add_silence_before',1);              % add silence to every stimulus
    p=ensure_field(p,'add_silence_after',1);              % add silence to every stimulus
else
    p.add_silence=0;
    p=ensure_field(p,'add_silence_before',0);              % add silence to every stimulus
    p=ensure_field(p,'add_silence_after',0);              % add silence to every stimulus
end
p=ensure_field(p,'silence_len',0.5);            % length of silence, in seconds

p=ensure_field(p,'ask_start_value', 0);         % add <interactive> section asking for the start value
p=ensure_field(p,'ask_level', 0);         % add <interactive> section asking for general level
p=ensure_field(p,'ask_snr', 0);         % add <interactive> section asking SNR (for fixed experiments)
p=ensure_field(p,'ask_subject',0);      % ask for the subjects name using <interactive>

if (~p.fixed_noise && p.ask_snr)
    error('Invalid combination of fixed and ask snr');
end

p=ensure_field(p,'exit_after',0);           % exit after experiment is finished
p=ensure_field(p,'addmarkup',0);

p=ensure_field(p,'filter', 0);          % do filtering of speech and noise
p=ensure_field(p,'filter_definition', 'hrtffilter_speech.xml');
p=ensure_field(p,'filter_interactive', 0);
p=ensure_field(p,'filter_type', 0);
p=ensure_field(p,'feedback_length',300);

PROC_ADP=1;
PROC_CST=2;
if (~isfield(p,'procedure'))
    if (p.noise && p.fixed_noise==0)
        p.procedure=PROC_ADP;
    else
        p.procedure=PROC_CST;
    end
end

% adaptive procedure
p=ensure_field(p,'start_value',-8);
p=ensure_field(p,'rev_for_mean', 6 );

if (~isfield(p,'order'))
    if (strcmp(p.type, 'lint'))
        p.order='random';
    else
        p.order='sequential';
    end
end

p=ensure_field(p,'order', 'sequential' );



% constants     WARNING: paths must end in /
p=ensure_field(p, 'LIST_path', 'dutch/list/');
p=ensure_field(p, 'LIST_man_path', 'dutch/LISTman/');
p=ensure_field(p, 'LIST_noise_file', 'dutch/list/noise/wivineruis.wav');
p=ensure_field(p, 'LIST_man_noise_file', 'dutch/LISTman/noise/jwruis.wav');

p=ensure_field(p, 'LINT_path', 'dutch/lint_rescaled/');
p=ensure_field(p, 'LINT_noise_file', 'dutch/lint_rescaled/noise.wav');

p=ensure_field(p, 'VU_path', 'dutch/vu/');
if (p.vu_male)
    p=ensure_field(p, 'VU_noise_file', 'dutch/vu/noise/man.wav');
else
    p=ensure_field(p, 'VU_noise_file', 'dutch/vu/noise/woman.wav');
end

p=ensure_field(p, 'NVA_path', 'dutch/nva/');

p=ensure_field(p, 'BKB_like_path', 'australian/BKB-like/');
p=ensure_field(p, 'BKB_like_noise_file', [p.BKB_like_path 'noise/ILTASS.wav']);

%p.custom_noise_file='bla.wav';                        % custom noise file


% default prefixes:
p=ef(p,'LIST_prefix', 'LIST');
p=ef(p,'LIST_man_prefix', 'LIST_man');
p=ef(p,'LINT_prefix', 'LINT');
p=ef(p,'VU_prefix', 'VU');
p=ef(p,'NVA_prefix', 'NVA');
p=ef(p,'BKB_like_prefix', 'BKB_like');


p=derive_params(p);


p=ensure_field(p, 'matlab_directory','/home/tom/matlab/');
if (isunix)
    delim='/';
else
    delim='\';
end

if (strcmp(p.type, 'list'))
    % generate list of filenames
    dbprefix=['lijst' num2str(p.list)];
    prefix=[p.LIST_path dbprefix delim];
    p.mcprefix=p.LIST_prefix;
%     d=dir( [prefix '*.wav']);
%     list=dir2list(d);

    filelist=getLIST_manfiles(p.list,p.matlab_directory);
    p.noisefile=p.LIST_noise_file;

    t_answers=getLISTanswers(p.list,p.addmarkup,p.matlab_directory);

elseif (strcmp(p.type, 'list_man'))
    % generate list of filenames
    if p.list==39
        prefix=[p.LIST_man_path 'oefenlijst' delim];
    else
        prefix=[p.LIST_man_path];
    end
    p.mcprefix=p.LIST_man_prefix;
%     d=dir( [prefix '*.wav']);
%     list=dir2list(d);
    
    filelist=getLIST_manfiles(p.list,p.matlab_directory);
    p.noisefile=p.LIST_man_noise_file;

    t_answers=getLIST_mananswers(p.list,p.addmarkup,p.matlab_directory);

elseif (strcmp(p.type, 'lint'))
    if (~strcmp(p.lint_speaker, 'ag') &&  ~strcmp(p.lint_speaker, 'md') && ~strcmp(p.lint_speaker, 'jw') && ~strcmp(p.lint_speaker, 'wd'))
        error(['Invalid LINT speaker: ' p.lint_speaker]);
    end
    dbprefix='';
%    postfix=[p.lint_speaker delim num2str(p.list) delim];
    prefix=[p.LINT_path];
     p.mcprefix=p.LINT_prefix;
%     d=dir( [prefix '*.wav']);
%     list=dir2list(d);

    [filelist,t_answers]=getLINTfiles(p);
    p.noisefile=p.LINT_noise_file;

elseif (strcmp(p.type, 'nva'))
    dbprefix=num2str(p.list);
    prefix=[p.NVA_path dbprefix delim];
    p.mcprefix=p.NVA_prefix;
    [filelist,t_answers]=getNVAfiles(p.list,p.matlab_directory);

elseif (strcmp(p.type, 'vu'))

   [filelist,t_answers ]=getVUfiles(p.list,p.vu_male);
   if (p.vu_male)
       dbprefix='man';
   else
       dbprefix='woman';
   end
   prefix=[p.VU_path dbprefix delim];
   p.noisefile=p.VU_noise_file;
   p.mcprefix=p.VU_prefix;

elseif (strcmp(p.type, 'BKB_like'))

   [m,path,dBFS,noisefile]=oz_speech_material('BKB_like');

   filelist = squeeze(cellfun(@(x) x.file(8:end), m(1, p.list, :),'UniformOutput', false))';
   t_answers = squeeze(cellfun(@(x) x.text, m(1, p.list, :),'UniformOutput', false))';

   dbprefix = sprintf('list%02.0f', p.list);
   prefix=[p.BKB_like_path dbprefix delim];
   p.noisefile=p.BKB_like_noise_file;
   p=ensure_field(p, 'noise_gain', dBFS);
   p.mcprefix=p.BKB_like_prefix;


else
    error('Unsupported type');
end

if (isfield(p,'custom_noise_file'))
    p.noisefile=p.custom_noise_file;
end


if (~isfield(p,'prefix_name'))
    p.prefix_name=p.mcprefix;
end

stimulinames=genstimulinames(filelist);
trialnames=gentrialnames(filelist);
screennames= repmat( {'screen'}, length(trialnames), 1);


%% create datablocks
datablocknames=gendatablocknames(filelist);

if p.prefix_from_main
datablocks=[  '<uri_prefix source="apexconfig">' p.prefix_name '</uri_prefix>' lf ];
else
    datablocks='';
end
%dbprefix=makedirend(dbprefix);
for i=1:length(filelist)
    datablocks=[datablocks ...
        a3datablock(datablocknames{i}, ...
        [prefix filelist{i}] ) ];
end


% add noise datablock if necessary
newdatablocknames=datablocknames;
if (p.noise)
    datablocks=[datablocks ...
        a3datablock('noisedata', p.noisefile) ];
%     filelist=cat(2,filelist,{p.noisefile});
%     newdatablocknames=cat(2,newdatablocknames,{'noisedata'});
end

% add silence datablock
datablocks=[datablocks ...
    a3datablock('silence', ['silence:' num2str(round(p.silence_len*1000))]) ];
%    newdatablocknames=cat(2,newdatablocknames,{'silence'});



datablocks=[wraptag('datablocks', datablocks)];

%% create stimuli

% make the correct answer a fixed parameter
fixparams=cell(size(stimulinames,1),size(stimulinames,2));
for i=1:length(stimulinames)
    fixparams{i}.sentence=t_answers{i};
end

newstimulinames=stimulinames;
if (length(p.calibration_profile))
   newstimulinames=cat(2,newstimulinames,{'calibrationstimulus'});
   %datablocknames=cat(2,datablocknames,{'calibrationdatablock'});
   datablocknames=cat(2,datablocknames,{'silence'});
   fixparams=cat(2,fixparams,{struct} );
   fixparams{end}.sentence='calibration';
end

stimuli=a3stimuli(datablocknames, newstimulinames,{},fixparams,p.add_silence_before,p.add_silence_after);






%% screens %%

if (p.manual_correction)
    screens=a3screens( a3screen('mao',p.vary_noise) , '' , p.feedback_length );
    answers=repmat({'correct'}, size(trialnames,1),size(trialnames,2));
else
    if (strcmp(p.type, 'lint'))
        screens=a3screens( a3screen('lint') , '' , p.feedback_length );
    else
        screens=a3screens( a3screen('opc') , '' , p.feedback_length );
    end
    answers=t_answers;
end



    trials=a3trials(trialnames, answers, screennames, stimulinames);

    if (p.procedure==PROC_ADP)        % use adaptive procedure
        proceduretype=1;
    else                % use constant procedure
        proceduretype=2;
    end

    procedureparameters=a3procedureparameters(p,proceduretype);
    if (p.procedure==PROC_ADP)
        procedureparameters.parameters.start_value=p.start_value;
%        procedureparameters.parameters.rev_for_mean=p.rev_for_mean;
    end
    procedureparameters.parameters.order=p.order;
    procedureparameters=a3toxml(procedureparameters);
    procedure=a3procedure(procedureparameters,trials,proceduretype, '', p.apexversion);

    if strcmp(p.apexversion, '3.0')
        corrector=a3corrector('isequal');
    else
        corrector='';
    end

    devices=a3devices(a3wavdevice('wavdevice', {'cardgain1' 'cardgain2'}));

    if (p.filter)
       if (~p.noise)
           error('Not implemented');
       end

       connections=[ a3connection('_ALL_', 0, 'amplifier', 0) ...
                                a3connection('amplifier', 0, 'speechfilter', 0) ...
                                a3connection('speechfilter', 0, 'wavdevice', 0) ...
                                a3connection('speechfilter', 1, 'wavdevice', 1) ...
                                a3connection('noisegen', 0, 'noisefilter', 0) ...
                            a3connection('noisefilter', 0, 'wavdevice', 0) ... 
                            a3connection('noisefilter', 1, 'wavdevice', 1) ...
                            ];

    else
        if (p.noise)
            if (p.vary_noise)
               connections=a3connections(p,'');
            else
                connections=[ a3connection('_ALL_', 0, 'amplifier', 0) ...
                                a3connection('amplifier', 0, 'wavdevice', 0) ...
                                a3connection('noisegen', 0, 'wavdevice', 0) ];
    %            connections=a3connections(p,'speech');
            end
        else
                connections=[ a3connection('_ALL_', 0, 'amplifier', 0) ...
                                a3connection('amplifier', 0, 'wavdevice', 0) ];

        end
    end

    connections=a3connections(connections);



%% filters %%
if (p.fixed_noise)
    if (p.vary_noise)
        error('Not implemented');
    else
        p.basegain_speech=p.basegain_speech+p.fixed_snr;
    end
end

filters='';
if (p.noise)
    filters=[filters ...
        a3dataloop('noisedata',p.basegain_dataloop,'noisegen', p.continuous_noise, 'wavdevice', p.vary_noise) ...
        a3amplifier(p.basegain_speech) ];
else
    filters=[filters ...
        a3amplifier(p.basegain_speech) ];
end

if (p.filter_type)
    replaces.filter_type = p.filter_type;
    p.filter_definition = readfile_replace(p.filter_definition,replaces);
else
    p.filter_definition = readfile(p.filter_definition);
end

if (p.filter)
   filters=[filters p.filter_definition];
end

filters=a3filters(filters);

%% calibration
if ( length(p.calibration_profile))
    if (p.filter)
        calibration= a3calibration(p.calibration_profile, 'calibrationstimulus', {'cardgain1' 'cardgain2'}, p.target_calibration);
    else
        calibration= a3calibration(p.calibration_profile, 'calibrationstimulus', 'cardgain', p.target_calibration);
    end
else
   calibration='';
end

interactive='';

if (p.ask_start_value)
    if (p.noise)
        interactive=a3interactive_entry_bytype('start_value');
    else
        interactive=a3interactive_entry_bytype('start_value', 'Level start value (relative to 65dBSPL)');
    end
elseif (p.ask_level)
    interactive=a3interactive_entry_bytype('level');
elseif (p.ask_snr)
    interactive=a3interactive_entry_bytype('snr');
end

if (p.ask_subject)
   interactive=[interactive a3interactive_entry_bytype('subject')];
end

if (p.filter_interactive)
    interactive=[interactive readfile(p.filter_interactive)];
end

interactive=a3interactive(interactive);


% if (p.exit_after)
%     general=readfile('exitafter.xml');
% else
%     general='';
% end



if (p.manual_correction)
    results=a3results('apex:resultsviewer.html','','',1);
else
    results=a3results('apex:resultsviewer.html','','',0);
end


general=a3general(p.exit_after);


%% write everything to file in correct order %%
F = fopen(p.filename, 'w');
if (F==-1)
   error(['Can''t open file for writing: ' p.filename]);
end
fprintf(F,'%s', a3header(p.apexversion) );
fprintf(F,'%s', procedure);
fprintf(F,'%s', corrector );
fprintf(F, screens );
fprintf(F,'%s', datablocks );
fprintf(F,'%s', devices );
fprintf(F,'%s', filters );
fprintf(F,'%s', stimuli );
fprintf(F,'%s', connections );
fprintf(F,'%s', calibration );
fprintf(F,'%s', results );
fprintf(F,'%s', interactive );
fprintf(F,'%s', general );
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
end




function list=getLISTfiles(listnr,matlab_directory)
    load([matlab_directory 'zinnen' delim 'zinnen.mat'])
    load([matlab_directory 'zinnen' delim 'selectzinnen.mat'])
    load([matlab_directory 'zinnen' delim 'lijstjes.mat'])

    list=ZinnenLists(listnr,:);

function list=getLIST_manfiles(listnr,matlab_directory)
    load([matlab_directory 'zinnen' delim 'lijstjes_man.mat'])

     list=ZinnenLists(listnr,:);





function [list,answers]=getNVAfiles(listnr,matlab_directory)
% zoek lijst op
if (isunix)
    load([matlab_directory 'zinnen' delim 'nva.mat'])
else
    error('Not implemented');
end

answers=nvawoorden(listnr,:);
list=cell(size(answers,1),size(answers,2));
for i=1:length(answers)
   list{i}=[answers{i} '.wav'];
end


function [list,answers]=getLINTfiles(p)
list=cell(0,0);
answers=cell(0,0);

% for cfile=1:length(files)
%     filename=files(cfile).name;
%
%     r='^\w{2}(\d+)\.wav$';
%
%     [s,f,t]=regexp(filename,r);
%     if (length(t)==0)
%         warning(['Unknown LINT file: ' filename]);
%         continue;
%     end
%
%     nr=str2num(filename(t{1}(1):t{1}(2)));
%     list=cat(2, list, {filename} );
%     answers=cat(2, answers, {num2str(nr)} );
% end
%
% if (length(answers)==0)
%     error('Can''t find LINT files on this PC');
% end

load LINT.mat
mat=getfield(LINTlists,p.lint_speaker);
for i=1:10
    number=mat(p.list,i);
    filename=[p.lint_speaker num2str(number) '.wav'];
    list=cat(2, list, {filename} );
    answers=cat(2, answers, {num2str(number)} );
end



function answers=getLISTanswers(listnr,addmarkup,matlab_directory)
% addmarkup determines whether to label keywords etc.

    if (isunix)
        load([matlab_directory 'zinnen' delim 'zinnen.mat'])
        load([matlab_directory 'zinnen' delim 'selectzinnen.mat'])
        load([matlab_directory 'zinnen' delim 'splitsbaar.mat'])
        load([matlab_directory 'zinnen' delim 'lijstjes.mat'])
    else
       error('windows not supported');
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
            result=xmlzin(zinnen{nr},selectzinnen{nr},splitsbaar{nr},'b','i');
        else
            result=zinnen{nr};
        end
        answers{c}=result;
    end


     
function answers=getLIST_mananswers(listnr,addmarkup,matlab_directory)
% addmarkup determines whether to label keywords etc.

    if (isunix)
        load([matlab_directory 'zinnen' delim 'zinnen.mat'])
        load([matlab_directory 'zinnen' delim 'selectzinnen.mat'])
        load([matlab_directory 'zinnen' delim 'splitsbaar_man.mat'])
        load([matlab_directory 'zinnen' delim 'lijstjes_man.mat'])
    else
       error('windows not supported');
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
            result=xmlzin(zinnen{nr},selectzinnen{nr},splitsbaar{nr},'b','i');
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

    switch p.type
        case 'list'
            g=-25;

        case 'lint'
            g=-25;
            warning('Check that we are using the rescaled LINT files');
        case 'vu'
            if (p.vu_male)
                g=-24;
            else
                g=-21.6;
            end
        case 'BKB_like'
            g=-25;

        otherwise
            disp('Calculating noise gain...');
            r=wavread(p.noisefile);
            g=rmsdb(r);
            disp(g);
    end

%    warning('fixed noise intensity');
end
return;

function g=getspeechgain(p)

if (strcmp(p.type,'list'))
    g=-25;
elseif (strcmp(p.type,'list_man'))
    g=-27.9122; % to be verified
elseif strcmp(p.type,'lint')
    %% [Tom] was -25, 28/3/7
    %    g=-32.6;
    g=-25;      % for rescaled LINT files
elseif (strcmp(p.type,'vu'))
    %     if (p.vu_male)
    %         g=-23.8223;
    %     else
    %         g=-21.6182;
    %     end
    %
    %     warning('check VU speech gains');
    if (p.vu_male)
        g=-24;
    else
        g=-21.6;
    end
elseif (strcmp(p.type,'nva'))
    g=-25.9686;
elseif (strcmp(p.type,'BKB_like'))
    g=-25;
else
    error('Can''t get speech gain for this type');
end
return;



function filelist=addprefixtofilelist(filelist,dbprefix)
if (length(dbprefix)==0)
    return;
end
for i=1:length(filelist)
    if (~strncmp('silence', filelist{i}, length('silence')))
        filelist{i}=[dbprefix '/' filelist{i}];
    end
end
return;
