#include "psignifitwrapper.h"


#include "universalprefix.h"

#include <stdlib.h>
#include <string.h>

#include "batchfiles.h"
#include "mathheader.h"
#include "matrices.h"
#include "psignifit.h"

#include <QDebug>

#ifdef Q_CC_MSVC
#include <CRTDBG.h>
#endif


namespace apex {

    apex::ListMatrix matrixToQList(matrix m);

    PsignifitResult::PsignifitResult()
    {

    }

    PsignifitResult::~PsignifitResult()
    {

    }

    QList<double> &PsignifitResult::slopes()
    {
        return m_slopes;
    }

    void PsignifitResult::setSlopes(const QList<double> &p)
    {
        m_slopes = p;
    }

    QList<double> &PsignifitResult::thresholds()
    {
        return m_thresholds;
    }

    void PsignifitResult::setThresholds(const QList<double> &p)
    {
        m_thresholds = p;
    }

    ListMatrix &PsignifitResult::thresholdsLims()
    {
        return m_thresholds_lims;
    }

    void PsignifitResult::setThresholdsLims(const ListMatrix &p)
    {
        m_thresholds_lims = p;
    }

    ListMatrix &PsignifitResult::slopesLims()
    {
        return m_slopes_lims;
    }

    void PsignifitResult::setSlopesLims(const ListMatrix &p)
    {
        m_slopes_lims = p;
    }

    PsignifitWrapper::PsignifitWrapper()
    {

    }

    PsignifitWrapper::~PsignifitWrapper()
    {

    }

    PsignifitResult* PsignifitWrapper::psignifitS(const QString& d)
    {
        qDebug() << QString("PsignifitWrapper::psignifit: ") << d;


        ModelPtr model = NULL;
        DataSetPtr data = NULL;
        GeneratingInfoPtr gen = NULL;
        OutputPtr out = NULL;
        BatchPtr input = NULL;

//        _CrtDbgBreak();

        TabulaRasa();
        m_init();

        // Get string with data, and preferences, append #data in front
        QByteArray temp = d.toLatin1();
        input = LoadPrefs(0, temp.data(), temp.length() +1, 0);

        // Parse preferences and data
        InitPrefs(input, &model, &data, &gen, &out, NULL);
        if (!data)
            return 0;

        DisposeBatch(input);

        bool success = Core(data, model, gen, out);

        if (!success)
            return 0;

        PsignifitResult* r = new PsignifitResult;

        r->setSlopes( matrixToQList(out->slopes.est).at(0) );        // only one row
        r->setThresholds( matrixToQList(out->thresholds.est).at(0) );        // only one row

        r->setSlopesLims( matrixToQList(out->slopes.lims) );
        r->setThresholdsLims( matrixToQList(out->thresholds.lims) );

        CleanUp(data, model, gen, out);

        return r;
    }

    bool PsignifitWrapper::selfTest()
    {
        TabulaRasa();
        m_init();
         SelfTest();
         return true;
    }

    QVariantMap PsignifitWrapper::psignifit(const QString& d)
    {
        QVariantMap q;
        PsignifitResult* r = psignifitS(d);

        if (r==0)
            return q;

        q.insert("thresholds", listToQVariantList(r->thresholds()));
        q.insert("slopes",listToQVariantList( r->slopes() ));
        q.insert("thresholdsLims", listMatrixToQVariantList( r->thresholdsLims() ));
        q.insert("slopesLims", listMatrixToQVariantList(  r->slopesLims() ));

        delete(r);
        return q;
    }


ListMatrix matrixToQList(matrix m)
{
    // code structure from matrices.c/m_report
    boolean started;
    QList< QList<double> > r;

    //if(m->description && strlen(m->description) > 0) nc += fprintf(file, "#%s\n", m->description);
    if(m_mass(m) == 0 || !m_setpos(m, 1, 0) || !m_setpos(m, 2, 0)) return r;
    do {
        started = FALSE;
        do {
            if (!started)
                r.push_back(QList<double>());
            r.last().push_back(m_val(m));
            started = TRUE;
        } while(m_step(m, 2, 1));
    } while(m_step(m, 1, 1));

    return r;
}

QVariantList PsignifitWrapper::listToQVariantList(QList<double>& d)
{
    QVariantList r;
    for (int i=0; i<d.length(); ++i) {
        r.push_back(QVariant(d[i]));
    }
    return r;
}

QVariantList PsignifitWrapper::listMatrixToQVariantList(ListMatrix& d)
{
    QVariantList r;
    for (int i=0; i<d.length(); ++i) {
        r.push_back(listToQVariantList(d[i]));
    }
    return r;
}

}
