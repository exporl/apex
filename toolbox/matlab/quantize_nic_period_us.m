function t=quantize_nic_period_us(period)
% quantize period as is done by NICv2 (Cochlear)
% period in us

% bv. period 'period' als float
%
% T = int ((CYCLES_PER_US * period) + 0.5)
% waarin CYCLES_PER_US = 5                (1 cycle duurt 200ns = 1 RF clock periode)
%
% zelfde voor andere tijdsvariabelen zoals phase width, phase gap

% t=quant(period, 200e-3);
t = round(period/200e-3)*200e-3;

% cycles= floor( 5*period+0.5 );
% t=cycles/5;
