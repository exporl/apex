function a3pitchmatch(p)

if (nargin<1)
    p=struct;
end

p=ef(p,'targetpath', '/home/data/apex/pitchmatch_ac/');
p=ef(p,'expfile','pitchmatch.apx');

p=ef(p,'parameter', 'freq');            % parameter of acoustic stimulus to vary
p=ef(p,'reference', 250);               % value of acoustic stimulus parameter for the reference stimulus
p=ef(p,'time_between_intervals', 500);  % in ms

p=ef(p,'values', [100:100:500]);        % comparison values (for the other stimuli)

%% acoustic stimulus parameters
p=ef(p,'ac', struct);
p.ac=ef(p.ac, 'fs', 44100);
p.ac=ef(p.ac,'stimulus_type', 1);  % sinus
p.ac=ef(p.ac,'normalize_rms', -20);
p.ac=ef(p.ac,'ramp_type', 2);     % cosinus


p.targetpath=makedirend(p.targetpath,1);

%% Set items to be replaced in template
rd.screenquestion='Klinkt het 2de geluid HOGER of LAGER dan het eerste?';
rd.buttonhighertext='Hoger';
rd.buttonlowertext='Lager';

rd.referencedatablockid='db_ref';
rd.silencedatablockid='db_silence';
rd.screenid='screen';


%% Create a stimulus and XML structures for each parameter value
rd.trials='';
rd.datablocks='';
rd.stimuli='';
rd.connections='';


for value=p.values
    ac=setfield(p.ac, p.parameter, value);
    value=round(value);         %% avoid ugly names

    filename=sprintf('stimulus_%d.wav', value);
    datablock=sprintf('db_%d', value);
    stimulus=sprintf('stimulus_%d', value);
    trial=sprintf('trial_%d', value);

    w=genstimulus(ac);
    wavwrite(w,p.ac.fs, [p.targetpath filename]);

    rd.trials=[rd.trials lf a3trial(trial,rd.screenid,stimulus,'buttonlower') ];
    rd.datablocks=[rd.datablocks lf a3datablock(datablock,filename) ];
    rd.stimuli=[rd.stimuli lf ...
        a3stimulus(stimulus, {rd.referencedatablockid rd.silencedatablockid datablock}, struct, struct, 0)];
    rd.connections=[rd.connections lf a3connection(datablock, 0, 'filter_rove_stimulus', 0)];
end

%% Create reference datablock
value=p.reference;
ac=setfield(p.ac, p.parameter, value);

filename=sprintf('stimulus_%d.wav', value);
w=genstimulus(ac);
wavwrite(w,p.ac.fs, [p.targetpath filename]);
rd.datablocks=[rd.datablocks lf a3datablock(rd.referencedatablockid,filename) ];
rd.connections=[rd.connections lf a3connection(rd.referencedatablockid, 0, 'filter_rove_standard', 0)];


%% Create silence datablock
rd.datablocks=[rd.datablocks lf ...
    a3datablock(rd.silencedatablockid, ...
    ['silence:' num2str(p.time_between_intervals)])];
rd.connections=[rd.connections lf a3connection(rd.silencedatablockid, 0, 'soundcard', 0)];

f=readfile_replace('a3pitchmatch-template.xml', rd);
fid=fopen([p.targetpath p.expfile], 'w');
fwrite(fid,f);
fclose(fid);
