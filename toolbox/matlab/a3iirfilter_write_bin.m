function a3iirfilter_write_bin(B,A, filename)
% Writes .bin file with filter coefficients like expected by the APEX
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

if (size(B, 2)~=size(A, 2))
    error('Number of channels in B and A does not agree: %d (B) ~= %d (A)',size(B, 2),size(A, 2));
end
nchans = size(B, 2);
P = size(B, 1);
Q = size(A, 1);

fid = fopen(filename, 'w');
if (fid==-1)
    error('Could not write to file');
end

fwrite(fid, nchans, 'double');
fwrite(fid, P, 'double');
fwrite(fid, Q, 'double');
for chan=1:nchans
    fwrite(fid, B(:,chan), 'double');
    fwrite(fid, A(:,chan), 'double');
end

fclose(fid);