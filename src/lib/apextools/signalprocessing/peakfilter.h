#ifndef _EXPORL_SRC_LIB_APEXTOOLS_SIGNALPROCESSING_PEAKFILTER_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_SIGNALPROCESSING_PEAKFILTER_H_

#include "../global.h"
#include <QVector>

namespace apex
{

class APEXTOOLS_EXPORT PeakFilter
{
public:
    static void peakFilter(QVector<double> &B, QVector<double> &A, double gain,
                           double center, double bw, unsigned fs);
};

} // ns apex

#endif
