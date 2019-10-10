find_path(LO_INCLUDES lo.h PATHS /usr/include/lo)

find_library(LO_LIBRARIES NAMES lo)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LO DEFAULT_MSG LO_LIBRARIES LO_INCLUDES)

mark_as_advanced(LO_LIBRARIES)
