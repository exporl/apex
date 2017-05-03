#ifndef _EXPORL_SRC_LIB_APEXTOOLS_SIGNALPROCESSING_IIRFILTER_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_SIGNALPROCESSING_IIRFILTER_H_


#include "../global.h"

#include <QList>
#include <QString>
#include <QFile>

/**
 * IIR filter, identical to Matlab implementation (filter.m)
 */
class APEXTOOLS_EXPORT IirFilter
{
public:
    IirFilter();

    /*
     * B and A identical to matlab definition:
     * Y = FILTER(B,A,X) filters the data in vector X with the
     * filter described by vectors A and B to create the filtered
     * data Y.  The filter is a "Direct Form II Transposed"
     * implementation of the standard difference equation:
     * a(1)*y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb)
     *                     - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
     *
     *    If a(1) is not equal to 1, FILTER normalizes the filter
     * coefficients by a(1).
     */
    void setCoefficients(const QVector<double>& B, const QVector<double> &A);

    void process(double* const data, unsigned count);

    /* Load IIR filters from file (1 separate filter per channel) */
    static QList<IirFilter* > load (const QString &filePath,
            unsigned maximumLength = 0);

private:
    QVector<double> B;
    QVector<double> A;

    QVector<double> delayX,delayY;

};



#endif
