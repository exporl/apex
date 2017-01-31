function seq=read_aseq(filename)
% seq=read_aseq(filename)
%
% see save_aseq for a format description
%
% Tom Francart, ExpORL, 2009
% tom.francart@med.kuleuven.be

[fields,identifiers,formats,fieldsizes,version]=aseq_info;

seq=struct;

fid=fopen(filename, 'r');
if (fid==-1)
    error(['Cannot open file ' filename ' for reading']);
end

%% Check header
check_l(readid_l(fid),'RIFF');
a=fread(fid, 1, 'uint32');      % file size
fileinfo=dir(filename);
check_l(a, fileinfo.bytes);
check_l(readid_l(fid),'ASEQ');

%totalsize=0;

%% Read Info chunk

check_l(readid_l(fid),'INFO');
infolen=fread(fid, 1, 'uint32');
versionn=fread(fid, 1, 'float32');     % version number
check_l(version,versionn);

fieldlens=struct;
for i=1:infolen/8
    fieldid=readid_l(fid);
    fieldlen=fread(fid,1, 'uint32');

    q=findid_l(identifiers,fieldid);
    fieldlens.(fields{q})=fieldlen;     % consistency check
end

%% Read chunks

while (1)
    fieldid=readid_l(fid);
    if (feof(fid))
        break;
    end
    fieldlen=readlen_l(fid);

    q=findid_l(identifiers,fieldid);
    frames=fieldlen/fieldsizes(q);

    if (~isfield(fieldlens,fields{q}))
        error('Field not found in INFO block');
    end
    if (getfield(fieldlens,fields{q})~=frames)
        error('Field length does not match INFO block');
    end
    seq.(fields{q})= fread(fid, frames, formats{q});
    if (fieldsizes(q)==1 && mod(fieldlen,2))
        fseek(fid,1,0);     % skip padding byte
    end
end


fclose(fid);

function q=findid_l(identifiers, id)
found=0;
for q=1:length(identifiers)
    if (strcmp(id,identifiers{q}))
        found=1;
        break;
    end
end
if (~found)
    error(['ID ' id ' not known']);
end


function check_l(s,t)
if (ischar(s))
    if (~strcmp(s,t))
        error(['Invalid format, expected ' s ', found ' t]);
    end
else
     if (s~=t)
        error('Invalid format');
    end
end

function a=readid_l(fid)
a=fread(fid, 4, 'uint8=>char');
a=a';

function l=readlen_l(fid)
l=fread(fid,1, 'uint32');
