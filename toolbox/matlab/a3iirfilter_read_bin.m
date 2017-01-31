function [B,A] = a3iirfilter_read_bin(filename)
% Re .bin file with filter coefficients like expected by the APEX
% iirfilter plugin
  
% /** IIR filter implemented in the frequency domain. The filter is implicitly
%  * shared (copy-on-write) and cheap to copy. Filters are limited to 16k channels
%  * and 64k taps.
%  *
%  * The filter coefficients can be read from a file with the following format:
%  *
%  * @code
%  * size  type    description
%  *    8  double  number of channels (N)
%  *    8  double  number of filter coefficients per channel for B (P), i.e. feed-forward coefficients
%  *    8  double  number of filter coefficients per channel for A (Q), i.e. feed-back coefficients
%  *  P*8  double  feed-forward filter coefficients of the first channel (B)
%  *  Q*8  double  feed-back filter coefficients of the first channel (A)
%           ...
%           ...
%  *  P*8  double  feed-forward filter coefficients of the N-th channel (B)
%  *  Q*8  double  feed-back filter coefficients of the N-th channel (A)
%  *    .
%  *    .
%  *    .
%  * @endcode
%  */

fid = fopen(filename, 'r');
if (fid==-1)
    error('Could not read file');
end
nchans = fread(fid, 1, 'double');

P = fread(fid, 1, 'double');
Q = fread(fid, 1, 'double');

B = zeros(P, nchans);
A = zeros(Q, nchans);
for chan=1:nchans
    B(:,chan) = fread(fid, P, 'double');
    A(:,chan) = fread(fid, Q, 'double');
end

fclose(fid);
end