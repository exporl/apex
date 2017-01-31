clear;

[signal,p.fs] = wavread('asa.wav');

%% Initialize parameter struct
p.block_length = 1024;

% filterbank
[B,A] = a3iirfilter_read_bin('filterbank.bin');
p.n_channels = size(B,2);
p.filterbank_order = size(A,1)-1;
for channel = 1:p.n_channels
    p.filterbank(channel,:) = [(1/A(1,channel))*[B(:,channel);-A(2:end,channel)]]';
end

% lowpass filters
[B,A] = a3iirfilter_read_bin('lowpass.bin');
p.lowpass_order = size(B,1)-1;
p.lowpass = (1/A(1))*[B(:)',zeros(1,p.lowpass_order)];

% carriers
carriers = wavread('carriers.wav');
for channel = 1:p.n_channels
    p.carriers{channel} = carriers(:,channel);
end

%% Initialize states
s.filterbank_state = zeros(2*p.filterbank_order+1,p.block_length,p.n_channels);
s.lowpass_state = zeros(2*p.lowpass_order+1,p.block_length,p.n_channels);
s.passed_blocks = 0;

%% Apply vocoder to speech signal
signal_blocks = buffer(signal,p.block_length);
vocoded_signal_blocks = zeros(size(signal_blocks));

for i=1:size(signal_blocks,2)
    [vocoded_signal_blocks(:,i),s] = Vocode_block(signal_blocks(:,i),p,s);
    disp([num2str(round(100*(i/size(signal_blocks,2)))),'%']);
end
vocoded_signal = vocoded_signal_blocks(:);

wavwrite(vocoded_signal,p.fs,'asa_vocoded.wav');

% NOTE: length(vocoded_signal) == ceil(length(signal)/p.block_length)*p.block_length