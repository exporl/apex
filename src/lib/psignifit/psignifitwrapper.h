#ifndef _APEX_SRC_LIB_PSIGNIFIT_PSIGNIFITWRAPPER_H_
#define _APEX_SRC_LIB_PSIGNIFIT_PSIGNIFITWRAPPER_H_

#include "../apextools/global.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QVariantMap>

namespace apex
{

typedef QList<QList<double>> ListMatrix;

class PSIGNIFIT_EXPORT PsignifitResult : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<double> slopes READ slopes WRITE setSlopes)
    Q_PROPERTY(QList<double> thresholds READ thresholds WRITE setThresholds)
    Q_PROPERTY(
        ListMatrix thresholdsLims READ thresholdsLims WRITE setThresholdsLims)
    Q_PROPERTY(ListMatrix slopesLims READ slopesLims WRITE setSlopesLims)

public:
    PsignifitResult();
    ~PsignifitResult();

    QList<double> &slopes();
    void setSlopes(const QList<double> &p);

    QList<double> &thresholds();
    void setThresholds(const QList<double> &p);

    ListMatrix &thresholdsLims();
    void setThresholdsLims(const ListMatrix &p);

    ListMatrix &slopesLims();
    void setSlopesLims(const ListMatrix &p);

private:
    QList<double> m_slopes;
    QList<double> m_thresholds;

    ListMatrix m_slopes_lims;
    ListMatrix m_thresholds_lims;
};

class PSIGNIFIT_EXPORT PsignifitWrapper
    : public QObject // QObject so we can expose it to javascript
{
    Q_OBJECT

public:
    PsignifitWrapper(); // Needs to be public for javascript
    ~PsignifitWrapper();

    static PsignifitResult *psignifitS(const QString &d);
    static bool selfTest();

public slots:
    QVariantMap psignifit(const QString &d);

private:
    QVariantList listToQVariantList(QList<double> &);
    QVariantList listMatrixToQVariantList(ListMatrix &d);
};
}

#endif
