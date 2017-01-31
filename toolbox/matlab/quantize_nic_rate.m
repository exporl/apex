function t=quantize_nic_rate(rate)
% t=quantize_nic_rate(rate)
% rate: Hz


t=1./quantize_nic_period(1./rate);