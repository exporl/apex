#ifndef OUTPUTRECORDER_H
#define OUTPUTRECORDER_H

#include <status/errorlogger.h>

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
