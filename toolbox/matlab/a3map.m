function map=a3map(subject, stimparam, side, lowert, increasec, fitting_type )
% map=a3map(subject, stimparam )
%
% if lowert is defined, the T value will be decreased by lowert
% if increasec is defined, the C value will be increased by increasec

if (nargin<2)
    error('Invalid number of parameters');
end
if (nargin<3)
    side='L';
end
if (nargin<4)
    lowert=0;
end
if (nargin<5)
    increasec=0;
end
if (nargin<6)
    fitting_type='';
end

if (side~='L' && side~='R')
    error('Invalid side, warning: order of parameters changed, lowert is now 4th');
end

p.stimparam_el=stimparam;
channels=1:22;
tlevels=zeros(22,1);
clevels=tlevels;

for e=1:22

    found=0;
    for a=1:length(p.stimparam_el.electrodes)
        if (e==p.stimparam_el.electrodes(a))
            found=1;
            break;
        end
    end

    if (found)
        [t,c]=getfitting(subject,e,p.stimparam_el.rate,side,fitting_type);
        t=t-lowert;
        c=c+increasec;
        if (t<0)
            error('Cannot lower t level');
        end
        if (c>255)
            error('Cannot increase c level');
        end
    else
        t=0;
        c=0;
    end

    tlevels(e)=t;
    clevels(e)=c;
end

map=a3map_levels(channels, tlevels, clevels, p.stimparam_el.total_rate);
