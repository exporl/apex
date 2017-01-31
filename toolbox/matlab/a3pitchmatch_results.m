function a3pitchmatch_results(filename)

[labels,percent,count,totals]=a3cst2psy(filename, 'stimulus_%d');

figure;
%plotpsy(labels,percent,0,[],[],method);
fitpsy(labels,percent/100,totals,0);
title(['Pitch matching results - ' getshortfilename(filename)]);
text(min(labels)+10, 100, 'ac highest');