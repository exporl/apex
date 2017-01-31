#ifndef PEAKFILTER_h
#define PEAKFILTER_h

#include "../global.h"
#include <QVector>

namespace apex {

class APEXTOOLS_EXPORT PeakFilter
{
public:

    static void peakFilter(QVector<double> &B, QVector<double> &A,
        double gain, double center, double bw, unsigned fs);
};



} // ns apex

#endif
