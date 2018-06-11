# Uncomment to override make files that want precompiled headers
CLEBS -= pch

# Disable matlab filter as it gives problems with clang
CLEBS_DISABLED *= src/plugins/matlabfilter

# Dependencies for Unix:
unix:MATLABROOT=/opt/matlab

DEFINES=WITH_WEBENGINE
