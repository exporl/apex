function result=a3soundlevelmeter(p)

if (nargin<1)
    p=struct;
end

p=ef(p,'plugin', 'slm_2250');
p=ef(p,'transducer', '4189 (2513309)');
p=ef(p,'frequency_weighting', 'A');
p=ef(p,'time_weighting', 'S');
p=ef(p,'type', 'RMS');
p=ef(p,'percentile', 0.99);
p=ef(p,'time', 5);

result=['<soundlevelmeter>' lf];
result=[result '<plugin>' p.plugin '</plugin>' lf];
result=[result '<transducer>' p.transducer '</transducer>' lf];
result=[result '<frequency_weighting>' p.frequency_weighting '</frequency_weighting>' lf];
result=[result '<time_weighting>' p.time_weighting '</time_weighting>' lf];
result=[result '<type>' p.type '</type>' lf];
result=[result '<percentile>' num2str(p.percentile) '</percentile>' lf];
result=[result '<time>' num2str(p.time) '</time>' lf];
result=[result '</soundlevelmeter>' lf];
