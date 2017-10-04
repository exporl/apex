#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/testutils.h"

#include "tests.h"

#include <QFile>
#include <QMap>

using namespace bertha;
using namespace cmn;

void BerthaTest::testTypes()
{
    TEST_EXCEPTIONS_TRY

    const int checkInt = 1234;
    const float checkFloat = 11.1111f;
    const double checkDouble = 2222.222;
    const bool checkBool = false;
    const QString checkEnum = QLatin1String("One");

    QStringList checkList;
    checkList << QString::fromLatin1("ene") << QString::fromLatin1("nog ene")
              << QString::fromLatin1("IET \"moeilijker\",comma")
              << QString::fromLatin1("\\en wa\tnu");

    QStringList checkList2;
    checkList2 << QString::fromLatin1("gewoon")
               << QString::fromLatin1("comma's") << QString::fromLatin1("mag")
               << QString::fromLatin1("ook");

    QVector<double> checkVector;
    checkVector << 0.1 << 1.23 << 2 << 3.115;

    QVector<QVector<double>> checkDoubleVector;
    QVector<double> first;
    first << 0.1;
    QVector<double> second;
    second << 2.3 << 3;
    QVector<double> third;
    third << 4.3 << -2.345;
    checkDoubleVector << first << second << third;

    const QString xmlFile = QLatin1String("testexperiments/typeexperiment.xml");
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());

    QFile file(xmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = file.readAll();

    ExperimentProcessor processor(XmlLoader().loadContents(data));

    QString b1 = QSL("block1");
    QString b2 = QSL("block2");
    QCOMPARE(processor.parameterValue(b1, QSL("m_int")).toInt(), checkInt);
    QCOMPARE(processor.parameterInfo(b1, QSL("m_int")).description, QSL("int"));
    QCOMPARE(processor.parameterInfo(b1, QSL("m_int")).unit,
             BlockParameter::LinearUnit);
    QCOMPARE(processor.parameterInfo(b1, QSL("m_int")).minimum, -10000.0);
    QCOMPARE(processor.parameterInfo(b1, QSL("m_int")).maximum, 10000.0);
    QCOMPARE(processor.parameterValue(b1, QSL("m_double")).toDouble(),
             checkDouble);
    QCOMPARE(processor.parameterValue(b1, QSL("m_float")).value<float>(),
             checkFloat);
    QCOMPARE(processor.parameterValue(b1, QSL("m_bool")).toBool(), checkBool);
    QCOMPARE(processor.sanitizedParameterValue(b1, QSL("m_enum")).toString(),
             checkEnum);
    QCOMPARE(processor.parameterValue(b1, QSL("m_stringList")).toStringList(),
             checkList);
    QCOMPARE(
        processor.parameterValue(b1, QSL("m_vector")).value<QVector<double>>(),
        checkVector);
    QCOMPARE(processor.parameterValue(b1, QSL("m_doubleVector"))
                 .value<QVector<QVector<double>>>(),
             checkDoubleVector);

    QCOMPARE(processor.parameterValue(b2, QSL("m_bool")).toBool(), checkBool);
    QCOMPARE(processor.sanitizedParameterValue(b2, QSL("m_enum")).toString(),
             checkEnum);
    QCOMPARE(processor.parameterValue(b2, QSL("m_stringList")).toStringList(),
             checkList2);
    QCOMPARE(
        processor.parameterValue(b2, QSL("m_vector")).value<QVector<double>>(),
        checkVector);
    QCOMPARE(processor.parameterValue(b2, QSL("m_doubleVector"))
                 .value<QVector<QVector<double>>>(),
             checkDoubleVector);

    TEST_EXCEPTIONS_CATCH
}
