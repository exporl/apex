function a3firfilter_write_bin(coefficients, filename)
% Writes .bin file with filter coefficients like expected by the APEX
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


nchans = size(coefficients, 2);
ntaps = size(coefficients, 1);

fid = fopen(filename, 'w');
if (fid==-1)
    error('Could not write to file');
end

fwrite(fid, nchans, 'double');
fwrite(fid, ntaps, 'double');
fwrite(fid, coefficients, 'double');

fclose(fid);