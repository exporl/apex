#ifndef _EXPORL_SRC_LIB_APEXMAIN_OUTPUTRECORDER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_OUTPUTRECORDER_H_

#include "apextools/status/errorlogger.h"

namespace apex
{

namespace data
{
class ExperimentData;
}

class OutputRecorder : public ErrorLogger
{
    public:

        bool setupRecording(data::ExperimentData* data);

    private:

        void warning(const QString& msg);
};

}

#endif
