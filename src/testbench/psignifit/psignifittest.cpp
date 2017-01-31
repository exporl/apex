#include "psignifittest.h"
#include "psignifitwrapper.h"


#include <QString>
#include <QDebug>
#include <QList>

using namespace apex;

Q_DECLARE_METATYPE(QList<double>)

void PsignifitTest::testPsignifitWrapper_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn< QList<double> >("expected_thresholds");
    QTest::addColumn< QList<double> >("expected_slopes");

    QTest::newRow("Default example") << QString ("1.00000000     0.50000000     40\n"
                                   "4.00000000     1.00000000     40\n"
                                   "6.00000000     1.00000000     40\n"
                                   "1.60000000     0.57500000     40\n"
                                   "3.50000000     0.86700000     60\n"
                                   "1.80000000     0.58600000     60\n"
                                   "3.00000000     0.81700000     60\n"
                                   "2.50000000     0.66700000     60\n"
                                   "#shape                  logistic\n"
                                 "#n_intervals    2\n"
                                 "#runs                   999\n"
                                 "#random_seed     1352539891\n")
            << ( QList<double> () <<
                2.11401479565334149413
                << 2.78304545915526491129
                << 3.45207612265718832845 )
            <<  ( QList<double> () <<
                    0.33153502504380333304
                << 0.51802347663094272523
                << 0.33153502504380327753 ) ;

    QTest::newRow("Single data point") <<
            QString( "1 0.2 20\n"
                     "#shape logistic\n"
                     "#n_intervals 2\n"
                     "#runs 99\n")
            << QList<double> ()
            << QList<double> ();

}

void PsignifitTest::testPsignifitWrapper()
{
    QFETCH(QString, data);
    QFETCH(QList<double>, expected_thresholds);
    QFETCH(QList<double>, expected_slopes);

    // Data that came with psignifit standalone:
    /*QString dat("1.00000000     0.50000000     40\n"
                "4.00000000     1.00000000     40\n"
                "6.00000000     1.00000000     40\n"
                "1.60000000     0.57500000     40\n"
                "3.50000000     0.86700000     60\n"
                "1.80000000     0.58600000     60\n"
                "3.00000000     0.81700000     60\n"
                "2.50000000     0.66700000     60\n"
                "#shape                  logistic\n"
              "#n_intervals    2\n"
              "#runs                   999\n"
              "#random_seed     1352539891\n");

    QList<double> expected_thresholds, expected_slopes;
    expected_thresholds << 2.11401479565334149413
            << 2.78304545915526491129
            << 3.45207612265718832845;

    expected_slopes <<  0.33153502504380333304
            << 0.51802347663094272523
            << 0.33153502504380327753;*/


    PsignifitWrapper w;

    /*PsignifitResult* result =
            PsignifitWrapper::psignifit(dat);*/

    PsignifitResult* result =
            w.psignifitS(data);

    if (!result) {
        QVERIFY( expected_thresholds.length() == 0);
        return;
    }

    QCOMPARE(result->thresholds().length(), expected_thresholds.length() );
    QCOMPARE(result->slopes().length(), expected_slopes.length() );

    for (int i=0; i<result->thresholds().length(); ++i) {
        //qDebug("%1.20f", result->thresholds()[i]);

        QVERIFY( qAbs( result->thresholds()[i] - expected_thresholds[i] ) < 1e-10 );
        QVERIFY( qAbs( result->slopes()[i] - expected_slopes[i] ) < 1e-10 );
    }


    /*QCOMPARE( result->thresholds(), expected_thresholds );
    QCOMPARE( result->slopes(), expected_slopes );*/

    qDebug() << "TH_LIMS: " << result->thresholdsLims();
    qDebug() << "SL_LIMS: " << result->slopesLims();

    delete(result);

}


void PsignifitTest::testPsignifitSelfTest()
{
    QVERIFY( PsignifitWrapper::selfTest() );
}

QTEST_MAIN(PsignifitTest)

//#include "psignifittest.moc"
