function coefficients = a3firfilter_read_bin(filename)
% Reads .bin file with filter coefficients like expected by the APEX
% firfilter plugin

% /** FIR filter implemented in the frequency domain. The filter is implicitly
%  * shared (copy-on-write) and cheap to copy. Filters are limited to 16k channels
%  * and 64k taps.
%  *
%  * The filter coefficients can be read from a file with the following format:
%  *
%  * @code
%  * size  type    description
%  *    8  double  number of channels (c)
%  *    8  double  number of filter coefficients per channel (n)
%  *  n*8  double  filter coefficients of the first channel
%  *  n*8  double  filter coefficients of the second channel
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

ntaps = fread(fid, 1, 'double');

coefficients = zeros(ntaps, nchans);
for chan=1:nchans
    coefficients(:,chan) = fread(fid, ntaps, 'double');
end

fclose(fid);
end