The vocoder needs 3 files:
* 1 .bin file with filter coefficients for the different bandpass filters,
    i.e. filterbank.bin
* 1 .bin file with filter coefficients for the lowpass filter (for envelope extraction),
    i.e. lowpass.bin
* 1 .wav file with the noise carriers to impose the extracted envelope on
    i.e. carriers.wav

REMARKS:
* The .bin files can be generated with a3iirfilter_write_bin.m, which can be found in the APEX 4 MATLAB Toolbox.
* The .wav file consists of N channels, i.e. 1 channel for each carrier
* The .bin files have the following structure:
    * size  type    description
    *    8  double  number of channels (N)
    *    8  double  number of filter coefficients per channel for B (P), i.e. feed-forward coefficients
    *    8  double  number of filter coefficients per channel for A (Q), i.e. feed-back coefficients
    *  P*8  double  feed-forward filter coefficients of the first channel (B)
    *  Q*8  double  feed-back filter coefficients of the first channel (A)
           ...
           ...
    *  P*8  double  feed-forward filter coefficients of the N-th channel (B)
    *  Q*8  double  feed-back filter coefficients of the N-th channel (A)

