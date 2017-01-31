function save_aseq(seq, filename)
% save_aseq(seq, filename)
%
% Save electric sequence (cfr Nucleus Matlab Toolbox) in aseq format
%
% An aseq file is a RIFF file. It starts with the string "RIFF", followed
% by the length of the file, followed by the string "ASEQ". RIFF files
% contain multiple chunks that start with a 4 byte chunk identifier and a
% 4 byte unsigned int chunk length. The first chunk is an INFO chunk.
%
%   "RIFF"      (4 bytes)
%   Length      (uint32, 4bytes)
%   "ASEQ"      (4 bytes)
%   "INFO"
%   Length      (uint32, 4bytes)
%   ...
%   ID
%   Length
%   ...
%
% A chunk has the following components:
%   Identifier      (4 bytes)
%   Length in bytes (uint32, 4bytes)
%   Data            (Identifier dependent, Length bytes)
%
% An INFO chunk contains the version number of the ASEQ file and a list
% of chunks that it contains and their length (in number of frames).
% It has the following structure:
%   "INFO"
%   Length          (uint32, 4bytes)
%   Version number  (float32, 4 bytes)
%   Identifier 1    (4 bytes)
%   Length in frames (uint32, 4 bytes)
%   Identifier 2
%   Length in frames (uint32, 4bytes)
%   ...
%
% Each identifier occurs no more than once in a file
%
% Identifiers
% -----------
% INFO  See above
% CHAN  signed char8 (1byte)
%       channel number, will be mapped by APEX to electrode number
%       as specified in the map
%       if channel number ==0, a powerup frame will be generated
% AELE  signed char8 (1byte)
%       active electrode number (1=most basal, 22=most apical)
% RELE  signed char8 (1byte)
%       return electrode number
% MAGN  float32 (4 bytes)
%       magnitude, between 0 and 1, will be mapped by APEX to
%       current units, as specified in the map
%       if magnitude<0, a powerup frame will be generated
% CURL  unsigned char8 (1byte)
%       current level, in Current Units (clinical units), between 0 and 255
% PERI  float32 (4 bytes)
%       period, in microseconds
% PHWI  float32 (4 bytes)
%       phase width, in microseconds
% PHGA  float32 (4 bytes)
%       inter phase gap, in microseconds
%
% The following restrictions should be taken into account for an aseq
% file to be usable:
%   The following identifiers cannot occur together
%       CHAN and ELEC
%       MAGN and CURL
%   Every block that has the M batch indicator set, should have the same
%       number of pulses defined
%
% Tom Francart, ExpORL, 2009
% tom.francart@med.kuleuven.be

[fields,identifiers,formats,fieldsizes,version]=aseq_info;

%% Sanity check
if (isfield(seq, 'electrodes') && isfield(seq,'channels'))
    error('electrodes and channels cannnot be defined simultaneously');
end
if (isfield(seq, 'current_levels') && isfield(seq,'magnitudes'))
    error('current_levels and magnitudes cannnot be defined simultaneously');
end
if (isfield(seq,'modes'))
    error('Modes not supported, please use return_electrodes');
end

if (isfield(seq,'electrodes'))
    if (sum( seq.electrodes<-3 | seq.electrodes>22))
        error('Invalid electrode value');
    end
end

if (isfield(seq,'return_electrodes'))
    if (sum( seq.return_electrodes<-3 | seq.return_electrodes>22))
        error('Invalid return electrode value');
    end
end

if (isfield(seq,'channels'))
    if (sum( seq.channels<0 ))
        error('Invalid channel value');
    end
end

if (isfield(seq,'periods'))
    if (sum( seq.periods<0 | seq.periods>13078))
        error('Invalid period value');
    end
end

if (isfield(seq,'electrodes') && isfield(seq,'channels'))
    error('You should define either electrodes or channels');
end

if (isfield(seq,'magnitudes') && isfield(seq,'current_levels'))
    error('You should define either electrodes or channels');
end

% if (xor(isfield(seq,'magnitudes'), isfield(seq,'channels')))
%     error('Magnitudes and channels should both be defined');
% end

% if (xor(isfield(seq,'electrodes'), isfield(seq,'current_levels')))
%     error('electrodes and current_levels should both be defined');
% end

check_quantisation_l(seq, 'periods');
check_quantisation_l(seq, 'phase_widths');
check_quantisation_l(seq, 'phase_gaps');


%% [Tom] if extension is not present, add extension
if (~ (length(filename)>5 && strcmp(filename(end-4:end),'.aseq')))
    filename = strcat(filename, '.aseq');
end

fid=fopen(filename, 'w');
if (fid==-1)
    error(['Cannot open file ' filename ' for writing']);
end


%% Write header
fwrite(fid, 'RIFF');
fwrite(fid, 0, 'uint32');      % file size
fwrite(fid, 'ASEQ');

%% Write Info chunk
fn=fieldnames(seq);

infolen=length(fn)*8+4;
fwrite(fid, 'INFO');
fwrite(fid, infolen, 'uint32');
fwrite(fid, version, 'float32');     % version number

mfieldlen=NaN;
for f=1:length(fn)
    fieldname=fn{f};
    q=findfield_l(fields, fieldname);

    fwrite(fid,identifiers{q});
    len = length(getfield(seq,fieldname));
    fwrite(fid, len, 'uint32');

    if (len~=1)
        if (~isnan(mfieldlen))
            if (mfieldlen~=len)
                error(['Field lengths do not match (' fieldname ')']);
            end
        else
            mfieldlen=len;
        end
    end
end

%% Write chunks
fn=fieldnames(seq);
for f=1:length(fn)
    fieldname=fn{f};

    q=findfield_l(fields, fieldname);
    data=getfield(seq,fieldname);
    if (size(data,1)>1 && size(data,2)>1)
        error('Matrices not allowed');
    end
    writechunk_l(fid, identifiers{q}, data, ...
        formats{q}, fieldsizes(q));
end


%% Update file size
s=ftell(fid);
fseek(fid,4, -1);
fwrite(fid, s, 'uint32');      % file size

fclose(fid);


function writechunk_l(fid, identifier, data, format,  fieldsize)
if (length(identifier)~=4)
    error('Invalid identifier');
end

fwrite(fid, identifier);
fwrite(fid, length(data)*fieldsize, 'uint32');
fwrite(fid, data, format);
if (fieldsize==1 && mod(length(data),2))
    fwrite(fid,0, format);          % write padding byte
end


function q=findfield_l(fields, fieldname)
found=0;
for q=1:length(fields)
    if (strcmp(fieldname,fields{q}))
        found=1;
        break;
    end
end
if (~found)
    error(['Field ' fieldname ' not known']);
end


function check_quantisation_l(seq, field)
if (isfield(seq,field))
   if ( sum( abs(seq.(field) - quantize_nic_period_us( seq.(field)))>1e-13 ) )
       warning([field ' not quantized to 200ns, will be quantized when sent to NIC']);
   end
end
