#include "iirfilter.h"


IirFilter::IirFilter()
{

}

void IirFilter::setCoefficients(const QVector<double> &Bn,
        const QVector<double> &An)
{
    Q_ASSERT(Bn.size());
    Q_ASSERT(An.size());

    B=Bn;
    A=An;
    delayX.resize(B.size());
    delayY.resize(A.size());

    if (A[0]!=1) {      // normalize coefficients
        for (int i=0; i<B.size(); ++i) {
              B[i]=B[i]/A[0];
         //     qDebug("B[%u]=%f", i, B[i]);
         }
         for (int i=1; i<A.size(); ++i) {
              A[i]=A[i]/A[0];
//              qDebug("A[%u]=%f", i, A[i]);
         }
         A[0]=1;

    }

    for (int i=0; i<B.size(); ++i) {
         delayY[i]=0;
    }
    for (int i=0; i<A.size(); ++i) {
         delayX[i]=0;
    }
}

void IirFilter::process(double* const data, unsigned count)
{
    Q_ASSERT(B.size());
    Q_ASSERT(A.size());

    double * const xValues = delayX.data();
    double * const yValues = delayY.data();
    const double * const xCoeffs = B.data();
    const double * const yCoeffs = A.data();
    const unsigned xCount = A.size();
    const unsigned yCount = B.size();

     for (unsigned i = 0; i < count; ++i) {
        for (unsigned j = 0; j < xCount - 1; ++j)
            xValues[j] = xValues[j + 1];
        xValues[xCount - 1] = data[i];
        for (unsigned j = 0; j < yCount - 1; ++j)
            yValues[j] = yValues[j + 1];

        yValues[yCount - 1] = 0;
        for (unsigned j = 0; j < xCount; ++j)
            yValues[yCount - 1] += xValues[j] * xCoeffs[xCount-j-1];
        for (unsigned j = 0; j < yCount - 1; ++j)
            yValues[yCount - 1] -= yValues[j] * yCoeffs[yCount-j-1];

        data[i] = yValues[yCount - 1];
    }

}

