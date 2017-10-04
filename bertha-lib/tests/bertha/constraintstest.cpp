#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/testutils.h"

#include "tests.h"

#include <sndfile.h>

#include <QFile>
#include <QMap>

using namespace bertha;
using namespace cmn;

void BerthaTest::testConstraints()
{
    TEST_EXCEPTIONS_TRY

    const int checkInt = 10000;
    const float checkFloat = -10000;
    const double checkDouble = 10000;

    QVector<double> checkVector;
    checkVector << 100 << 5 << 22 << -100;

    QVector<QVector<double>> checkDoubleVector;
    QVector<double> first;
    first << 100;
    QVector<double> second;
    second << 5 << 22;
    QVector<double> third;
    third << -100 << 2.345;
    checkDoubleVector << first << second << third;

    const QString xmlFile =
        QLatin1String("testexperiments/constraintsexperiment.xml");
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());

    QFile file(xmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = file.readAll();

    ExperimentProcessor processor(XmlLoader().loadContents(data));

    QCOMPARE(
        processor
            .parameterValue(QLatin1String("block1"), QLatin1String("m_int"))
            .toInt(),
        checkInt);
    QCOMPARE(
        processor
            .parameterValue(QLatin1String("block1"), QLatin1String("m_double"))
            .toDouble(),
        checkDouble);
    QCOMPARE(
        processor
            .parameterValue(QLatin1String("block1"), QLatin1String("m_float"))
            .value<float>(),
        checkFloat);
    QCOMPARE(
        processor
            .parameterValue(QLatin1String("block1"), QLatin1String("m_vector"))
            .value<QVector<double>>(),
        checkVector);
    QCOMPARE(processor
                 .parameterValue(QLatin1String("block1"),
                                 QLatin1String("m_doubleVector"))
                 .value<QVector<QVector<double>>>(),
             checkDoubleVector);

    TEST_EXCEPTIONS_CATCH
}
