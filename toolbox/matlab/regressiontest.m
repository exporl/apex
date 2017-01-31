function regressiontest
% test whether all functions work properly


test_varia
test_experiment
test_results


function test_results
% test functions that have something to to with results analysis
aidtestfile='examples-results/aid1-results.xml';
idntestfile='examples-results/idn1-results.xml';
[stimuli, corrector, sequence,label]=a3aidresults(aidtestfile)
[stimlabels, percent, count, totals]=a3cst2psy(idntestfile, 'stimulus%d')
[stimuli, buttons, corrector]=a3cstresults(idntestfile)
[stimlabels, percent, count, totals]=a3cst2psy_nofile(stimuli,buttons,corrector,  'stimulus%d')

check(a3getmatlabscript(idntestfile));
check(a3getresults(idntestfile));
check(a3getxsltscript(idntestfile));
check(a3transform(idntestfile,'cst.xsl'));

% test confusion matrix
values=abs(round(rand(100,1)*10));
[mat,labels]=confusionmatrix(values(randperm(100)),values(randperm(100)))


function test_experiment
% test functions that have something to to with experiment creation

%% wavdevice
check(a3amplifier(10,'amplifier','device',2,{'gain0', 'gain1'}));
connection=check(a3connection('fromdevice', 0, 'todevice', 1));
check(a3connections(connection));
wavdevice=check(a3wavdevice('wavdevice', 2));
check(a3devices(wavdevice));
dataloop=check(a3dataloop('datablock',-5,'dataloop'));
check(a3filters(dataloop));
check(a3noisegen);
check(a3rme_asio(2));


%% L34
check(a3l34device('l34', 1, 'cic3', 'out', '<map/>'));     % warning: this it not a valid map



%% datablocks
check(a3datablock('datablock','file.wav','soundcard',2));
check(a3datablocks({'file1.wav' 'file2.wav'}, '../stimuli', {'datablock1', 'datablock2'}) );
check(a3datablocks({'file1.wav' 'file2.wav'}, '../stimuli', {'datablock1', 'datablock2'}, 'speechtests'));

%% stimuli
check(a3fixedparameters({'param1', 'param2'}));
varpar.a=1;
fixpar.b=2;
check(a3stimuli({'datablock1', 'datablock2'}, {'stimulus1','stimulus2'},{varpar; varpar},{fixpar; fixpar},1,1));
check(a3stimulus('stimulus',{'datablock1', 'datablock2'} ,varpar,fixpar,1));


%% screens
check(a3buttongroup({'button1','button2'}));
check(a3buttonlayout({'text1','text2'}, 2, 3, {'a', 'b'}));
button=check(a3button('button', 2, 3, 'text'));
arclayout=check(a3arclayout(5,button,'full'));
check(a3buttonscreen({'text1','text2'}, 'screen text', 'buttonscreen', {'a', 'b'}));
gridlayout=check(a3gridlayout(2,3,button));
check(a3spinbox('spinbox', 2, 3, 10,-10, 20, 2, 'gain'));
a3twopartlayout(gridlayout,arclayout, 0.7, 'vertical')




%% procedures
check(a3corrector('isequal'));
check(a3corrector('afc',3));
parameters=check(a3procedureparameters(struct,1));
check(a3trial('trial', 'screen', {'stimulus1','stimulus2'}, 'answer1'));
trials=check(a3trials({'trial1','trial2'}, {'answer1','answer2'}, {'screen1','screen2'}, {'stimulus1','stimulus2'}));
check(a3procedure(parameters, trials,1, 'procedure'));




%% varia
check(a3calibration('profile', 'stimulus', 'parameter', 65));
check(a3description('description'));
check(a3footer);
check(a3general(1,1,1));
check(a3header);
check(a3interactive('start_value'));
check(a3randomgenerator('randomgen', 0, 10, 'uniform', 'gain'));
check(a3reinforcement(1,1000,1));
check(a3results('custom.xsl','script.m'));
check(a3showparams(varpar));
check(a3singletrial('trial','answer','screen', {'stimulus1','stimulus2'},'standard'));
check(a3toxml(varpar));

readfile('regressiontest.m');
% $$replaceme$$
toreplace.replaceme='****** REPLACED ******';
check(readfile_replace('regressiontest.m', toreplace));


function test_varia
s=a3localsettings;
test_local_settings(s);
a3checkfile('examples-create/extrasimple.apx');
bool2xml(1);
wraptag('tag','content');


function test_local_settings(s)
if (~exist(s.xalancmd, 'file'))
    warning('Xalan executable defined in a3localsettings not found');
end
if (~exist(s.apex_path, 'dir'))
    warning('Apex path defined in a3localsettings not found');
end
if (~exist(s.apex_schema, 'file'))
    warning('Apex schema defined in a3localsettings not found');
end
if (~exist(s.xml_check_tool, 'file'))
    warning('xml checking tool defined in a3localsettings not found');
end
if (~exist(s.apex_xslt_scripts, 'dir'))
    warning('Apex xslt scripts directory defined in a3localsettings not found');
end



function st=check(st)
disp(st);
