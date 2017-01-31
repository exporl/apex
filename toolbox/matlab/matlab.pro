!isEmpty(_PRO_FILE_):include(../../clebs/clebs.pri)

TEMPLATE=subdirs

matlab.path = $$DATADIR/toolbox/matlab
matlab.files = toolbox/matlab/*

matlaba3templates.path = $$DATADIR/amt/a3templates
matlaba3templates.files = toolbox/matlab/a3templates/*

matlabexamplescreate.path = $$DATADIR/amt/examples-create
matlabexamplescreate.files = toolbox/matlab/examples-create/*

matlabexamplesresults.path = $$DATADIR/amt/examples-results
matlabexamplesresults.files = toolbox/matlab/examples-results/*

INSTALLS *= matlab matlaba3templates matlabexamplescreate matlabexamplesresults
