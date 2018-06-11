#include "apextools/apextools.h"
#include "apextools/apexpaths.h"

#include "apextools/signalprocessing/iirfilter.h"
#include "apextools/signalprocessing/peakfilter.h"

#include "apextools/xml/xmltools.h"

#include "common/debug.h"
#include "common/global.h"
#include "common/testutils.h"

#include "apextoolstest.h"

#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#include <QTemporaryDir>

using namespace apex;

void ApexToolsTest::initTestCase()
{
    cmn::enableCoreDumps(QCoreApplication::applicationFilePath());
}

void ApexToolsTest::testXmlRichText()
{
    TEST_EXCEPTIONS_TRY

    QString xml =
        QSL("<a param='c'><e>This is a <b param=\"d\">Test</b></e></a>");
    QDomDocument doc = XmlUtils::parseString(xml);
    QCOMPARE(
        XmlUtils::richText(doc.documentElement().firstChildElement(QSL("e"))),
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

    const unsigned count = 20;

    { // test FIR
        IirFilter filter;

        QVector<double> B, A;
        B.resize(3);
        A.resize(3);

        B[0] = 1;
        B[1] = 2;
        B[2] = 3;
        A[0] = 1;
        A[1] = 0;
        A[2] = 0;

        filter.setCoefficients(B, A);

        double data[count];
        data[0] = 1;
        for (unsigned i = 1; i < count; ++i) {
            data[i] = 0;
        }

        filter.process(data, count);

        // FIXME nothing is tested here

        /*        for (unsigned i=0; i<count; ++i) {
                    qCDebug(APEX_RS, "data[%u]=%f", i, data[i]);
                }*/
    }

    { // test IIR
        IirFilter filter;

        QVector<double> B, A;
        B.resize(3);
        A.resize(3);

        B[0] = 2.1273;
        B[1] = -3.9315;
        B[2] = 1.8444;
        A[0] = 2.0000;
        A[1] = -3.9315;
        A[2] = 1.9717;

        filter.setCoefficients(B, A);

        double data[count];
        data[0] = 0.9;
        for (unsigned i = 1; i < count; ++i) {
            data[i] = 0;
        }

        filter.process(data, count);

        double data_matlab[] = {0.9573,  0.1126,  0.1076,  0.1005,  0.0915,
                                0.0807,  0.0685,  0.0551,  0.0408,  0.0259,
                                0.0106,  -0.0046, -0.0196, -0.0339, -0.0474,
                                -0.0597, -0.0706, -0.0800, -0.0876, -0.0933};

        /*        for (unsigned i=0; i<count; ++i) {
                    qCDebug(APEX_RS, "data[%u]=%f", i, data[i]);
                }*/

        for (unsigned i = 0; i < count; ++i) {
            //            qCDebug(APEX_RS, "i=%u", i);
            QCOMPARE(std::abs(data[i] - data_matlab[i]) < 0.0001, true);
        }
    }

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::testPeakFilter()
{
    TEST_EXCEPTIONS_TRY

    QVector<double> B, A, B_matlab, A_matlab;
    B.resize(3);
    A.resize(3);

    apex::PeakFilter::peakFilter(B, A, 20, 1000, 200, 44100);

    B_matlab.resize(3);
    A_matlab.resize(3);

    B_matlab[0] = 2.2529;
    B_matlab[1] = -3.9038;
    B_matlab[2] = 1.6909;
    A_matlab[0] = 2.0000;
    A_matlab[1] = -3.9038;
    A_matlab[2] = 1.9438;

    // FIXME is this supposed to be a fuzzy compare? QCOMPARE can do that
    for (int i = 0; i < 3; ++i) {
        QVERIFY(B[i] - B_matlab[i] < 0.0001);
        QVERIFY(A[i] - A_matlab[i] < 0.0001);
    }

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::testRecursiveCopy()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryDir source;
    QDir sourceDir(source.path());
    QVERIFY(sourceDir.mkpath(sourceDir.filePath(QSL("dir1"))));
    QDir dir1(sourceDir.filePath(QSL("dir1")));
    QFile file1(sourceDir.filePath(QSL("file1")));
    QFile file2(dir1.filePath(QSL("file2")));
    file1.open(QIODevice::WriteOnly);
    file2.open(QIODevice::WriteOnly);
    file1.close();
    file2.close();

    /* copy a directory to an existing directory, thereby making
     * a subdirectory in the destination with the same name
     */
    QTemporaryDir destination;
    QDir destinationDir(destination.path());
    QVERIFY(ApexTools::recursiveCopy(source.path(), destination.path()));
    QVERIFY(QFile::exists(
        destinationDir.filePath(sourceDir.dirName() + QL1S("/file1"))));
    QVERIFY(QFile::exists(
        destinationDir.filePath(sourceDir.dirName() + QL1S("/dir1"))));
    QVERIFY(QFile::exists(
        destinationDir.filePath(sourceDir.dirName() + QL1S("/dir1/file2"))));

    /* copy a single file */
    QVERIFY(ApexTools::recursiveCopy(file2.fileName(), destination.path()));
    QVERIFY(QFile::exists(destinationDir.filePath(QSL("file2"))));
    /* file already exists */
    QVERIFY(!ApexTools::recursiveCopy(file2.fileName(), destination.path()));
    QVERIFY(QFile::exists(destinationDir.filePath(QSL("file2"))));
    /* force copy */
    QVERIFY(
        ApexTools::recursiveCopy(file2.fileName(), destination.path(), true));
    QVERIFY(QFile::exists(destinationDir.filePath(QSL("file2"))));

    /* copy a directory to a non-existent directory
     * thereby making the directory
     */
    QVERIFY(ApexTools::recursiveCopy(
        destinationDir.filePath(sourceDir.dirName() + QL1S("/dir1")),
        destination.path()));
    QVERIFY(QFile::exists(destinationDir.filePath(QSL("dir1/file2"))));

    /* nested directories to a non-existent directory */
    sourceDir.mkpath("dir2/dir3");
    QVERIFY(ApexTools::recursiveCopy(file1.fileName(),
                                     sourceDir.filePath(QSL("dir2/dir3"))));
    QVERIFY(QFile::exists(sourceDir.filePath(QSL("dir2/dir3/file1"))));
    QVERIFY(ApexTools::recursiveCopy(sourceDir.filePath(QSL("dir2")),
                                     destinationDir.path()));

    QVERIFY(QFile::exists(destinationDir.filePath(QSL("dir2/dir3/file1"))));

    /* make dir4/dir5 in destination, contents of dir2 is copied */
    QVERIFY(
        ApexTools::recursiveCopy(sourceDir.filePath(QSL("dir2")),
                                 destinationDir.filePath(QSL("dir4/dir5"))));
    QVERIFY(
        QFile::exists(destinationDir.filePath(QSL("dir4/dir5/dir3/file1"))));

    /* copy dir2 to existing directory, so it's nested inside that directory */
    destinationDir.mkpath(QSL("dir6"));
    QVERIFY(ApexTools::recursiveCopy(sourceDir.filePath(QSL("dir2")),
                                     destinationDir.filePath(QSL("dir6"))));
    QVERIFY(
        QFile::exists(destinationDir.filePath(QSL("dir6/dir2/dir3/file1"))));

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::testGenerateKeyPair()
{
    TEST_EXCEPTIONS_TRY

    /* TODO
     * On windows QTemporaryDir creation often fails in this particular test.
     * No error messages are provided (they will be starting Qt5.6).
     */
    QTemporaryDir tempDir;
    QDir dir(tempDir.path());
    if (!tempDir.isValid())
        qCWarning(APEX_RS, "QTemporary Directory creation failed.");
    QString privateKeyPath = dir.filePath(QSL("mykey"));
    QString publicKeyPath = dir.filePath(QSL("mykey.pub"));

    ApexTools::generateKeyPair(QFile::encodeName(privateKeyPath));

    QFile publicKeyFile(publicKeyPath);
    QVERIFY(publicKeyFile.open(QIODevice::ReadOnly));

    QList<QByteArray> publicKeyDatas =
        publicKeyFile.readAll().split(QChar::Space);

    QByteArray keyType = QSL("ssh-rsa").toUtf8();
    QCOMPARE(publicKeyDatas.at(0), keyType);

    QByteArray publicKeyData = QByteArray::fromBase64(publicKeyDatas.at(1));
    QDataStream dataStream(&publicKeyData, QIODevice::ReadOnly);
    quint32 typeSize;
    dataStream >> typeSize;
    QCOMPARE(typeSize, (quint32)keyType.size());
    QByteArray keyType2(typeSize, QChar::Null);
    dataStream.readRawData(keyType2.data(), typeSize);
    quint32 exponentSize;
    dataStream >> exponentSize;
    QCOMPARE(exponentSize, (quint32)3);
    QByteArray exponentData(exponentSize, QChar::Null);
    dataStream.readRawData(exponentData.data(), exponentSize);
    quint32 modulusSize;
    dataStream >> modulusSize;
    /* ignore the leading 0 */
    QCOMPARE((modulusSize - 1) * 8, (quint32)2048);
    QByteArray modulusData(modulusSize, QChar::Null);
    dataStream.readRawData(modulusData.data(), modulusSize);

    QSharedPointer<RSA> publicKey(RSA_new(), RSA_free);
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    int charsUsed = BN_hex2bn(&publicKey->e, exponentData.toHex());
#else
    BIGNUM *e = NULL; /* RSA takes ownership */
    int charsUsed = BN_hex2bn(&e, exponentData.toHex());
#endif
    QCOMPARE(charsUsed, exponentData.toHex().size());
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    charsUsed = BN_hex2bn(&publicKey->n, modulusData.toHex().constData());
#else
    BIGNUM *n = NULL; /* RSA takes ownership */
    charsUsed = BN_hex2bn(&n, modulusData.toHex());
    RSA_set0_key(publicKey.data(), n, e, NULL);
#endif
    QCOMPARE(charsUsed, modulusData.toHex().size());

    QSharedPointer<BIO> privateKeyFile(
        BIO_new_file(QFile::encodeName(privateKeyPath), "r"), BIO_free_all);
    QSharedPointer<RSA> privateKey(
        PEM_read_bio_RSAPrivateKey(privateKeyFile.data(), NULL, NULL, NULL),
        RSA_free);
    QVERIFY(privateKey);

    QString message = QSL("verify me!");
    QByteArray signature(RSA_size(privateKey.data()), QChar::Null);
    unsigned int signatureLength = 0;
    int result =
        RSA_sign(NID_sha1, (unsigned char *)message.toUtf8().constData(),
                 message.length(), (unsigned char *)signature.data(),
                 &signatureLength, privateKey.data());
    QCOMPARE(result, 1);

    result = RSA_verify(NID_sha1, (unsigned char *)message.toUtf8().constData(),
                        message.length(), (unsigned char *)signature.data(),
                        signatureLength, publicKey.data());
    QCOMPARE(result, 1);

    if (!tempDir.isValid()) {
        publicKeyFile.close();
        privateKeyFile.clear();
        QVERIFY(QFile::setPermissions(privateKeyPath, QFileDevice::WriteOwner));
        QVERIFY(QFile::setPermissions(publicKeyPath, QFileDevice::WriteOwner));
        QVERIFY(QFile::remove(privateKeyPath));
        QVERIFY(QFile::remove(publicKeyPath));
    }

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::testRecursiveFind()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryDir source;
    QDir sourceDir(source.path());
    sourceDir.mkpath(sourceDir.filePath(QSL("dir1/dir2")));
    QDir dir1(sourceDir.filePath(QSL("dir1")));
    QDir dir2(dir1.filePath(QSL("dir2")));
    QFile file1(sourceDir.filePath(QSL("file1.apx")));
    QFile file2(dir1.filePath(QSL("file2.apx")));
    QFile file3(dir2.filePath(QSL("file3.apx")));
    QFile file4(dir2.filePath(QSL("file4.apz")));
    QFile file5(dir2.filePath(QSL(".hidden.apx")));
    file1.open(QIODevice::WriteOnly);
    file2.open(QIODevice::WriteOnly);
    file3.open(QIODevice::WriteOnly);
    file4.open(QIODevice::WriteOnly);
    file5.open(QIODevice::WriteOnly);
    file1.close();
    file2.close();
    file3.close();
    file4.close();
    file5.close();

#ifdef Q_OS_WIN32
    QString path = file5.fileName();
    wchar_t *pathUtf16 = (wchar_t *)path.utf16();
    SetFileAttributes(pathUtf16,
                      GetFileAttributes(pathUtf16) | FILE_ATTRIBUTE_HIDDEN);
#endif

    QCOMPARE(ApexTools::recursiveFind(source.path(),
                                      QRegularExpression(QSL(".*[.]apx"))),
             QStringList() << file1.fileName() << file2.fileName()
                           << file3.fileName());
    QCOMPARE(ApexTools::recursiveFind(
                 source.path(), QRegularExpression(QSL(".*[.]apx")), true),
             QStringList() << file1.fileName() << file2.fileName()
                           << file3.fileName() << file5.fileName());
    QVERIFY(
        ApexTools::recursiveFind(QString(), QRegularExpression(QSL(".*[.]apx")))
            .isEmpty());
    QCOMPARE(ApexTools::recursiveFind(source.path(),
                                      QRegularExpression(QSL(".*[.]apz"))),
             QStringList() << file4.fileName());
    QVERIFY(ApexTools::recursiveFind(source.path(),
                                     QRegularExpression(QSL("[a-z[")))
                .isEmpty());

    TEST_EXCEPTIONS_CATCH
}

// generate standalone binary for the test
QTEST_MAIN(ApexToolsTest)
