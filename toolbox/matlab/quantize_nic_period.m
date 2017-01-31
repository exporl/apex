function t=quantize_nic_period(period)
% quantize period as is done by NICv2 (Cochlear)
% period in s

% bv. periode 'period' als float
%
% T = int ((CYCLES_PER_US * period) + 0.5)
% waarin CYCLES_PER_US = 5                (1 cycle duurt 200ns = 1 RF clock periode)
%
% zelfde voor andere tijdsvariabelen zoals phase width, phase gap

% t=quant(period, 200e-9);

t = round(period/200e-9)*200e-9;


% cycles= floor( 5*period*1e6+0.5 );
% t=cycles/5e6;
