function script=a3getxsltscript(filename)

FID = fopen(filename,'r');
if (FID==-1)
    error(['Kan ' filename ' niet openen']);
end

line = fgetl(FID);
while( ischar(line) )

    re='<xsltscript>\s*(?<script>.*)\s*</xsltscript>';

    names=regexp(line,re,'names');
    if (length(names))
        script=names.script;
        fclose(FID);
        return;
    end

    line = fgetl(FID);
end


fclose(FID);

script='';
return;