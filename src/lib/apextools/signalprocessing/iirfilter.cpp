#include "iirfilter.h"

IirFilter::IirFilter()
{
}

void IirFilter::setCoefficients(const QVector<double> &Bn,
                                const QVector<double> &An)
{
    Q_ASSERT(Bn.size());
    Q_ASSERT(An.size());

    B = Bn;
    A = An;
    delayX.resize(B.size());
    delayY.resize(A.size());

    if (A[0] != 1) { // normalize coefficients
        for (int i = 0; i < B.size(); ++i) {
            B[i] = B[i] / A[0];
            //     qCDebug(APEX_RS, "B[%u]=%f", i, B[i]);
        }
        for (int i = 1; i < A.size(); ++i) {
            A[i] = A[i] / A[0];
            //              qCDebug(APEX_RS, "A[%u]=%f", i, A[i]);
        }
        A[0] = 1;
    }

    for (int i = 0; i < B.size(); ++i) {
        delayX[i] = 0;
    }
    for (int i = 0; i < A.size(); ++i) {
        delayY[i] = 0;
    }
}

void IirFilter::process(double *const data, unsigned count)
{
    Q_ASSERT(B.size());
    Q_ASSERT(A.size());

    double *const xValues = delayX.data();
    double *const yValues = delayY.data();
    const double *const xCoeffs = B.data();
    const double *const yCoeffs = A.data();
    const unsigned xCount = B.size();
    const unsigned yCount = A.size();

    for (unsigned i = 0; i < count; ++i) {
        for (unsigned j = 0; j < xCount - 1; ++j)
            xValues[j] = xValues[j + 1];
        xValues[xCount - 1] = data[i];
        for (unsigned j = 0; j < yCount - 1; ++j)
            yValues[j] = yValues[j + 1];

        yValues[yCount - 1] = 0;
        for (unsigned j = 0; j < xCount; ++j)
            yValues[yCount - 1] += xValues[j] * xCoeffs[xCount - j - 1];
        for (unsigned j = 0; j < yCount - 1; ++j)
            yValues[yCount - 1] -= yValues[j] * yCoeffs[yCount - j - 1];

        data[i] = yValues[yCount - 1];
    }
}

QList<IirFilter *> IirFilter::load(const QString &filePath,
                                   unsigned maximumLength)
{
    /*
    IMPORTANT: This function allocates memory for each IIR filter in the QList;
    the caller of this function is responsible for deleting these filters after
    they have been used.

    This static function loads a set of IIR filters from filePath (.bin-file)
    to a QList of IIR filters. The .bin-file has the following structure:
        * size  type    description
        *    8  double  number of channels (N)
        *    8  double  number of filter coefficients per channel for B (P),
    i.e. feed-forward coefficients
        *    8  double  number of filter coefficients per channel for A (Q),
    i.e. feed-back coefficients
        *  P*8  double  feed-forward filter coefficients of the first channel
    (B)
        *  Q*8  double  feed-back filter coefficients of the first channel (A)
                ...
                ...
        *  P*8  double  feed-forward filter coefficients of the N-th channel (B)
        *  Q*8  double  feed-back filter coefficients of the N-th channel (A)

    The .bin-file can be generated with the MATLAB function
    a3iirfilter_write_bin.m,
    which can be found in the APEX 4 MATLAB Toolbox.
    */
    QList<IirFilter *> result;

    // make file object from filePath string
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        throw(QString("Unable to read coefficients from '%1': %2")
                  .arg(file.fileName(), file.errorString()));

    // read coefficients from file to vector "result"
    double doubleChannels;
    if (file.read(reinterpret_cast<char *>(&doubleChannels), 8) != 8)
        throw("Unable to read number of channels");
    if (doubleChannels < 1)
        throw("Invalid number of channels");
    unsigned channels = qRound(doubleChannels);

    double doubleP; // feed-forward filter order +1
    if (file.read(reinterpret_cast<char *>(&doubleP), 8) != 8)
        throw("Unable to read feed-forward filter order");
    if (doubleP < 1 || doubleP >= INT_MAX / 8)
        throw("Invalid feed-forward filter order");
    unsigned P = qRound(doubleP);

    double doubleQ; // feed-back filter order +1
    if (file.read(reinterpret_cast<char *>(&doubleQ), 8) != 8)
        throw("Unable to read feed-forward filter order");
    if (doubleQ < 1 || doubleQ >= INT_MAX / 8)
        throw("Invalid feed-forward filter order");
    unsigned Q = qRound(doubleQ);

    for (unsigned i = 0; i < channels; ++i) {
        QVector<double> B(P);
        QVector<double> A(Q);
        if (file.read(reinterpret_cast<char *>(B.data()), 8 * P) != 8 * P)
            throw("Unable to read filter coefficients");
        if (maximumLength)
            B.resize(qMin(unsigned(B.size()), maximumLength));

        if (file.read(reinterpret_cast<char *>(A.data()), 8 * Q) != 8 * Q)
            throw("Unable to read filter coefficients");
        if (maximumLength)
            A.resize(qMin(unsigned(A.size()), maximumLength));

        IirFilter *iirFilter = new IirFilter;
        iirFilter->setCoefficients(B, A);
        result.append(iirFilter);
    }

    return result;
}
