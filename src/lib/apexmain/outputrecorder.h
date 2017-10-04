#ifndef _APEX_SRC_LIB_APEXMAIN_OUTPUTRECORDER_H_
#define _APEX_SRC_LIB_APEXMAIN_OUTPUTRECORDER_H_

#include "apextools/global.h"

namespace apex
{

namespace data
{
class ExperimentData;
}

APEX_EXPORT bool setupOutputRecording(data::ExperimentData *data,
                                      bool useBertha);
}

#endif
