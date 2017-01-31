function test_save_aseq
% testbench for save_aseq

seq=struct;
seq.channels=[1 2 3];
seq.magnitudes=[5 6 7];
ref=[82 73 70 70 72 0 0 0 65 83 69 81 73 78 70 79 20 0 0 0 0 0 128 63 67 72 65 78 3 0 0 0 77 65 71 78 3 0 0 0 67 72 65 78 3 0 0 0 1 2 3 0 77 65 71 78 12 0 0 0 0 0 160 64 0 0 192 64 0 0 224 64]';
testseq_l(seq,ref);


seq=struct;
seq.electrodes=[10 11 12 13];
seq.return_electrodes=[-1];
seq.current_levels=[22 23 24 25];
seq.phase_widths=10;
%seq.phase_gaps
seq.periods=[110 120 130 140];
%seq.channels
%seq.magnitudes
ref=[82 73 70 70 134 0 0 0 65 83 69 81 73 78 70 79 44 0 0 0 0 0 128 63 65 69 76 69 4 0 0 0 82 69 76 69 1 0 0 0 67 85 82 76 4 0 0 0 80 72 87 73 1 0 0 0 80 69 82 73 4 0 0 0 65 69 76 69 4 0 0 0 10 11 12 13 82 69 76 69 1 0 0 0 255 0 67 85 82 76 4 0 0 0 22 23 24 25 80 72 87 73 4 0 0 0 0 0 32 65 80 69 82 73 16 0 0 0 0 0 220 66 0 0 240 66 0 0 2 67 0 0 12 67]';
testseq_l(seq,ref);

%% Test error handling
seq=struct;
seq.electrodes=[-1 2 3];
testerror_l(seq)

seq=struct;
seq.channels=[-1 0 1 2 3];
testerror_l(seq)

seq=struct;
seq.return_electrodes=[-1 0 1];
testerror_l(seq)

seq=struct;
seq.channels=[1 2 3];
seq.electrodes=[1 2 3];
testerror_l(seq)

seq=struct;
seq.channels=[1 2];
seq.magnitudes=[0 1];
seq.periods=102.123;
lastwarn('');
disp('** start warning **');
save_aseq(seq,'/tmp/__85484zerf4z84ff.aseq');
if (isempty(lastwarn))
    error('No warning');
end
disp('** end warning **');

%% Save several structures to validate APEX
targetpath='/home/tom/data/apex/data/tests/libapex/';
seq=struct;
seq.electrodes=[1 2 3];
seq.current_levels=[10 20 30];

seqt=seq;
seqt.phase_widths=99;
save_aseq(seqt, [targetpath 'phwi.aseq']);

seqt=seq;
seqt.phase_gaps=99;
save_aseq(seqt, [targetpath 'phga.aseq']);

seqt=seq;
seqt.return_electrodes=-2;
save_aseq(seqt, [targetpath 'rele.aseq']);

seqt=seq;
seqt.periods=99;
save_aseq(seqt, [targetpath 'peri.aseq']);

seq=struct;
seq.channels=[1 2 3];
seq.magnitudes=[0.1 0.2 0.3];
save_aseq(seq, [targetpath 'chanmag.aseq']);

seq=struct;
seq.periods=[1 2 3];
save_aseq(seq, [targetpath 'invalid1.aseq']);

seq=struct;
seq.channels=[0 0];
seq.magnitudes=[1 2];
save_aseq(seq, [targetpath 'powerup1.aseq']);

seq=struct;
seq.channels=[0 0];
seq.magnitudes=[-0.0001 -2];
save_aseq(seq, [targetpath 'powerup2.aseq']);

seq=struct;
seq.channels=[0 0];
seq.magnitudes=[-0.0001 -2];
seq.periods=99;
save_aseq(seq, [targetpath 'powerup3.aseq']);

seq=struct;
seq.electrodes=[0 0];
seq.current_levels=[20 30];
save_aseq(seq, [targetpath 'powerup4.aseq']);



function testseq_l(seq,ref)

tempfile='__tmp35353554.aseq';
save_aseq(seq, tempfile);

fid=fopen(tempfile);
d=fread(fid);
if (sum(d-ref))
    error('test_save_aseq failed');
end

seq_r=read_aseq(tempfile);
if (length( comp_struct(seq,seq_r)))
    error('test_save_aseq failed');
end


disp('Tests successful');

function testerror_l(seq)
err=0;
try
    save_aseq(seq,'bla');
catch e
    err=1;
end
if (~err)
    error('Error handling');
end
