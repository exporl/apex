#include "apextoolstest.h"

#include "signalprocessing/iirfilter.h"
#include "signalprocessing/peakfilter.h"

#include "status/statusreporter.h"
#include "status/statusdispatcher.h"
#include "status/screenstatusreporter.h"

#include <typeinfo>

using namespace apex;

QString ApexToolsTest::name() const
{
    return "libtools";
}

void ApexToolsTest::testIirFilter()
{
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
            qDebug("data[%u]=%f", i, data[i]);
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
            qDebug("data[%u]=%f", i, data[i]);
        }*/

        for (unsigned i=0; i<count; ++i) {
//            qDebug("i=%u", i);
            QCOMPARE(abs(data[i]-data_matlab[i])<0.0001,true);
        }
    }

}

void ApexToolsTest::testPeakFilter()
{
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

}

void ApexToolsTest::testStatusItem()
{
    TEST_COPY_INIT(StatusItem, initStatusItem);

    StatusItem item = StatusItem().setSource("job").setMessage("hello");
    QVERIFY(item.level() == StatusItem::UNUSED);
    QVERIFY(item.source() == "job");
    QVERIFY(item.message() == "hello");
}

void ApexToolsTest::initStatusItem(StatusItem* item)
{
    item->setLevel(StatusItem::MESSAGE);
    item->setSource("ApexToolsTest");
    item->setMessage("Test started");
}

void ApexToolsTest::testStatusReporter()
{
    TEST_COPY_INIT(StatusReporter, initStatusReporter);

    //this class is used to check if StatusReporter calls report() when needed.
    class Tester : public StatusReporter
    {
        public:

            Tester() : reported(false) {}

            bool hasReported()
            {
                bool ret = reported;
                reported = false;
                return ret;
            }

        private:

            bool reported;

            void report(const StatusItem&)
            {
                reported = true;
            }
    } reporter;

    QCOMPARE(reporter.numberOfErrors(), 0u);
    QCOMPARE(reporter.numberOfWarnings(), 0u);
    QCOMPARE(reporter.numberOfMessages(), 0u);
    QCOMPARE(reporter.items().size(), 0);

    reporter.addMessage("test", "test started");
    QVERIFY(reporter.hasReported());

    reporter << StatusItem(StatusItem::ERROR).setSource("here")
                                             .setMessage("blabla");
    QVERIFY(reporter.hasReported());

    StatusItem::List items;
    items << StatusItem(StatusItem::WARNING) << StatusItem();
    reporter << items;
    QVERIFY(reporter.hasReported());

    QCOMPARE(reporter.numberOfErrors(), 1u);
    QCOMPARE(reporter.numberOfWarnings(), 1u);
    QCOMPARE(reporter.numberOfMessages(), 1u);
    QCOMPARE(reporter.items().size(), 4);

    reporter.clear();

    QCOMPARE(reporter.numberOfErrors(), 0u);
    QCOMPARE(reporter.numberOfWarnings(), 0u);
    QCOMPARE(reporter.numberOfMessages(), 0u);
    QCOMPARE(reporter.items().size(), 0);

    QVERIFY(!reporter.hasReported());
}

void ApexToolsTest::initStatusReporter(StatusReporter* reporter)
{
    reporter->addError("bla", "foo");
}

void ApexToolsTest::testStatusDispatcher()
{
    {
    //counts the number of times Tests::~Tester() is called
    uint counter = 0;

    //this class is used to check if a reporter gets deleted when needed
    class Tester : public StatusReporter
    {
        public:

            Tester(uint* counter) : counter(counter) {}
            ~Tester() {(*counter)++;}

        private:

            uint* counter;
    };

    Tester  repNoDel1(&counter);
    Tester  repNoDel2(&counter);
    Tester  repNoDel3(&counter);
    Tester* repDel1 = new Tester(&counter);
    Tester* repDel2 = new Tester(&counter);

    StatusDispatcher* dispatch = new StatusDispatcher();
    dispatch->addReporter(repDel1);
    dispatch->addReporter(repDel2);
    dispatch->addReporter(&repNoDel1, false);
    dispatch->addReporter(&repNoDel2, false);
    dispatch->addReporter(&repNoDel3, false);

    dispatch->addMessage("", "");
    QCOMPARE(repDel1->numberOfMessages(), 1u);
    QCOMPARE(repDel2->numberOfMessages(), 1u);
    QCOMPARE(repNoDel1.numberOfMessages(), 1u);
    QCOMPARE(repNoDel2.numberOfMessages(), 1u);
    QCOMPARE(repNoDel3.numberOfMessages(), 1u);

    dispatch->addWarning("", "");
    QCOMPARE(repDel1->numberOfWarnings(), 1u);
    QCOMPARE(repDel2->numberOfWarnings(), 1u);
    QCOMPARE(repNoDel1.numberOfWarnings(), 1u);
    QCOMPARE(repNoDel2.numberOfWarnings(), 1u);
    QCOMPARE(repNoDel3.numberOfWarnings(), 1u);

    dispatch->addError("", "");
    QCOMPARE(repDel1->numberOfErrors(), 1u);
    QCOMPARE(repDel2->numberOfErrors(), 1u);
    QCOMPARE(repNoDel1.numberOfErrors(), 1u);
    QCOMPARE(repNoDel2.numberOfErrors(), 1u);
    QCOMPARE(repNoDel3.numberOfErrors(), 1u);

    delete dispatch;
    QCOMPARE(counter, 2u);
    }

    {
    //test if a ConsoleStatusReporter is automatically added
    qDebug() << "new dispatcher";
    StatusDispatcher dispatch;
    dispatch.addError("", "");
    QCOMPARE(dispatch.reporters().size(), 1);
    //QCOMPARE(typeid(dispatch.reporters().first()).name(), "ConsoleStatusReporter*");
    dispatch.addReporter(new ScreenStatusReporter());
    QCOMPARE(dispatch.reporters().size(), 1);
    }
}

Q_EXPORT_PLUGIN2(test_libtools, ApexToolsTest);

