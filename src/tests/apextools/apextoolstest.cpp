#include "apextools/apexpaths.h"

#include "apextools/signalprocessing/iirfilter.h"
#include "apextools/signalprocessing/peakfilter.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"
#include "common/testutils.h"

#include "apextoolstest.h"

using namespace apex;

void ApexToolsTest::testXmlRichText()
{
    TEST_EXCEPTIONS_TRY

    QString xml = QSL("<a param='c'><e>This is a <b param=\"d\">Test</b></e></a>");
    QDomDocument doc = XmlUtils::parseString(xml);
    QCOMPARE(XmlUtils::richText(doc.documentElement().firstChildElement(QSL("e"))),
            QSL("This is a <b param=\"d\">Test</b>"));

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::testXmlCreateRichText()
{
    TEST_EXCEPTIONS_TRY

    QString richText = QSL("This <i>is</i> a <b param=\"d\">Test</b>!");
    QDomDocument doc;
    QDomElement element =
        XmlUtils::createRichTextElement(&doc, QSL("tag"), richText);
    QCOMPARE(XmlUtils::nodeToString(element),
             QSL("<tag>This <i>is</i> a <b param=\"d\">Test</b>!</tag>\n"));

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::testIirFilter()
{
    TEST_EXCEPTIONS_TRY

    const unsigned count=20;

    {       // test FIR
        IirFilter filter;

        QVector<double> B,A;
        B.resize(3);
        A.resize(3);

        B[0]=1;   B[1]=2;    B[2]=3;
        A[0]=1;        A[1]=0;          A[2]=0;

        filter.setCoefficients(B,A);

        double data[count];
        data[0]=1;
        for (unsigned i=1; i<count; ++i) {
            data[i]=0;
        }

        filter.process(data,count);

        //FIXME nothing is tested here

/*        for (unsigned i=0; i<count; ++i) {
            qCDebug(APEX_RS, "data[%u]=%f", i, data[i]);
        }*/


    }


    {       // test IIR
        IirFilter filter;

        QVector<double> B,A;
        B.resize(3);
        A.resize(3);

        B[0]=2.1273;   B[1]=-3.9315;    B[2]=1.8444;
        A[0]=2.0000;   A[1]=-3.9315;    A[2]=1.9717;

        filter.setCoefficients(B,A);


        double data[count];
        data[0]=0.9;
        for (unsigned i=1; i<count; ++i) {
            data[i]=0;
        }

        filter.process(data,count);

        double data_matlab[] = {0.9573,
            0.1126,
            0.1076,
            0.1005,
            0.0915,
            0.0807,
            0.0685,
            0.0551,
            0.0408,
            0.0259,
            0.0106,
            -0.0046,
            -0.0196,
            -0.0339,
            -0.0474,
            -0.0597,
            -0.0706,
            -0.0800,
            -0.0876,
            -0.0933};


/*        for (unsigned i=0; i<count; ++i) {
            qCDebug(APEX_RS, "data[%u]=%f", i, data[i]);
        }*/

        for (unsigned i=0; i<count; ++i) {
//            qCDebug(APEX_RS, "i=%u", i);
            QCOMPARE(std::abs(data[i]-data_matlab[i])<0.0001,true);
        }
    }

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::testPeakFilter()
{
    TEST_EXCEPTIONS_TRY

    QVector<double> B,A, B_matlab, A_matlab;
    B.resize(3);
    A.resize(3);

    apex::PeakFilter::peakFilter(B,A, 20, 1000, 200, 44100);

    B_matlab.resize(3);
    A_matlab.resize(3);

    B_matlab[0]=2.2529; B_matlab[1]=-3.9038; B_matlab[2]=1.6909;
    A_matlab[0]=2.0000; A_matlab[1]=-3.9038; A_matlab[2]=1.9438;

    //FIXME is this supposed to be a fuzzy compare? QCOMPARE can do that
    for (int i=0; i<3; ++i) {
        QVERIFY(B[i]-B_matlab[i]<0.0001 );
        QVERIFY(A[i]-A_matlab[i]<0.0001 );
    }

    TEST_EXCEPTIONS_CATCH
}

//generate standalone binary for the test
QTEST_MAIN(ApexToolsTest)

