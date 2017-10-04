#include "peakfilter.h"

#include <cmath>
#define PI 3.141592653589793115997963468544185161590576171875

void apex::PeakFilter::peakFilter(QVector<double> &B, QVector<double> &A,
                                  double gain, double center, double bw,
                                  unsigned fs)
{
    Q_ASSERT(A.size() == 3);
    Q_ASSERT(B.size() == 3);

    double h0 = pow(10, gain / 20) - 1;
    double d = -cos(2 * PI * center / fs);
    double c;
    if (gain >= 0)
        c = (tan(PI * bw / fs) - 1) / (tan(PI * bw / fs) + 1);
    else
        c = (tan(PI * bw / fs) - h0 - 1) / (tan(PI * bw / fs) + h0 + 1);

    B[0] = 2 + h0 * (1 + c);
    B[1] = 2 * d * (1 - c);
    B[2] = -2 * c - h0 * (c + 1);

    A[0] = 2;
    A[1] = 2 * d * (1 - c);
    A[2] = -2 * c;
}
