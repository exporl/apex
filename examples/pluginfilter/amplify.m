function data=amplify(data)
% Matlab function to be used in conjunction with matlabfilter.xml
global apex;

data=data*10^(apex.gain/20);
