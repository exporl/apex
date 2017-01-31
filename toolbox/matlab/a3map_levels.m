function map=a3map_levels(channels, tlevels, clevels, total_rate, pulsewidth,mode)
% map=a3map( electrodes, tlevels, clevels )

if (nargin<3)
    error('Invalid number of parameters');
end
if (nargin<5)
    pulsewidth=25;
end
if (nargin<6)
    mode='MP1+2';
end

lf=sprintf('\n');
map='<number_electrodes>22</number_electrodes>';
map=[map lf '<mode>' mode '</mode>'];
map=[map lf '<pulsewidth>' num2str(pulsewidth) '</pulsewidth>'];
map=[map lf '<pulsegap>8</pulsegap>'];
map=[map lf '<period>' sprintf('%2.20f', 1e6/total_rate)  '</period>'];

for e=1:22
    I=find(channels==e,1);
    if (isempty(I))
        t=0;
        c=0;
    else
        t=tlevels(I);
        c=clevels(I);
    end

    %% electrode=23-e: cochlear numbers the electrodes in the wrong way...
    map=[map lf '<channel number="' num2str(e) '" electrode="' num2str(23-e) '" threshold="' num2str(t) '" comfort="' num2str(c) '"/>'];
end
