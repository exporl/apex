/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "coh/cohclient.h"
#include "coh/cohnicxmldumper.h"

#include "common/exception.h"
#include "common/xmlutils.h"

#include <interface/NreSocketInterface.h>

#include <QSet>
#include <QStringList>
#include <QVector>
#include <QXmlDefaultHandler>

#include <QtPlugin>

using namespace cmn;
using namespace coh;

/** Creates CI clients for Cochlear's NIC using the socket interface. Device
 * specifiers are of the form "[<ip address>/]<device>[-<implant>]-<instance>",
 * with:
 * - device: "l34", "sp12", "cps"
 * - implant: "cic3", "cic4"
 * - instance: 0 captures in a log file, higher numbers correspond to USB ports
 *
 * @ingroup ci
 */
class NicSocketCohClientCreator : public QObject, public CohClientCreator
{
    Q_OBJECT
    Q_INTERFACES(coh::CohClientCreator)
    Q_PLUGIN_METADATA(IID "coh.nicsocketcohclient")
public:
    /** Creates a new NIC driver instance for a given device. An
     * std::exception is thrown if the driver is unable to setup the device.
     * The caller is responsible of freeing the returned pointer.
     *
     * If you use a device specifier with an instance of "0", the stimuli will
     * be output to "nic_test.xml" in the current directory.
     *
     * @param device device name as obtained from #devices()
     * @return NIC driver instance
     *
     * @throws std::exception on errors
     */
    virtual CohClient *createCohClient(const QString &device);

    /** Returns the list of devices that are known to the NIC driver. Because
     * the NIC driver provides no way of querying the available devices, this
     * list just contains some common specifiers for devices that need not to be
     * available.
     *
     * @return device names
     */
    virtual QStringList cohDevices() const;

    virtual QString cohDriverName() const;
};

#define NIC_FUNC_EH(func) functionErrorHandler(func, #func)
#define NIC_NEW_EH(ptr) newErrorHandler(ptr, #ptr)

class NicSocketCohClient : public CohClient
{
public:
    NicSocketCohClient(const QString &address);
    ~NicSocketCohClient();

    // NicSocketCohClient implementation
    virtual void send(CohSequence *sequence);
    virtual void start(Coh::TriggerMode trigger);
    virtual void stop();
    virtual Coh::Status status() const;
    virtual bool needsReloadForStop() const;

    static const char *triggerCommand(Coh::TriggerMode trigger);
    static const char *functionErrorHandler(const char *result,
                                            const char *message);
    template <typename T>
    static T newErrorHandler(T ptr, const char *message);

private:
    NreSocketHandle client;
};

class NicStatusHandler : public QXmlDefaultHandler
{
    Q_DECLARE_TR_FUNCTIONS(QXmlDefaultHandler)
public:
    NicStatusHandler();
    Coh::Status status() const;

    // Returns the message text of the last exception
    QString error() const;

    // Reimplementing QXmlErrorHandler and QXmlContentHandler
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    QString errorString() const;

private:
    Coh::Status result;
    bool root;
    // Only modified by #fatalError()
    QString errorException;
    // Must be set by handlers that return false
    QString errorMessage;

    QString currentText;
};

class NicConnectionHandler : public QXmlDefaultHandler
{
    Q_DECLARE_TR_FUNCTIONS(QXmlDefaultHandler)
public:
    NicConnectionHandler();

    enum Type { TYPE_UNKNOWN, TYPE_ACKNOWLEDGE, TYPE_FAULT };

    Type resultType() const;
    QString infoMessage() const;
    QString faultClass() const;

    // Returns the message text of the last exception
    QString error() const;

    // Reimplementing QXmlErrorHandler and QXmlContentHandler
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    QString errorString() const;

private:
    bool root;
    // Only modified by #fatalError()
    QString errorException;
    // Must be set by handlers that return false
    QString errorMessage;

    QString currentText;
    Type type;
    QString infoText;
    QString classText;
};

// NicConnectionHandler ========================================================

NicConnectionHandler::NicConnectionHandler() : root(false), type(TYPE_UNKNOWN)
{
}

bool NicConnectionHandler::startElement(const QString &namespaceURI,
                                        const QString &localName,
                                        const QString &qName,
                                        const QXmlAttributes &attributes)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);
    Q_UNUSED(attributes);

    if (!root && qName != QL1S("NRE-DEVICE:connection") &&
        qName != QL1S("device:connection")) {
        errorMessage = tr("Invalid root element.");
        return false;
    };

    root = true;

    // We are accepting all tags

    currentText.clear();
    return true;
}

bool NicConnectionHandler::endElement(const QString &namespaceURI,
                                      const QString &localName,
                                      const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (qName == QL1S("device:info")) {
        infoText = currentText;
    } else if (qName == QL1S("device:fault")) {
        classText = currentText;
        type = TYPE_FAULT;
    } else if (qName == QL1S("device:acknowledge")) {
        type = TYPE_ACKNOWLEDGE;
    } else if (qName == QL1S("NRE-DEVICE:info")) {
        infoText = currentText;
    } else if (qName == QL1S("NRE-DEVICE:class")) {
        classText = currentText;
    } else if (qName == QL1S("NRE-DEVICE:fault")) {
        type = TYPE_FAULT;
    } else if (qName == QL1S("NRE-DEVICE:acknowledge")) {
        type = TYPE_ACKNOWLEDGE;
    }

    return true;
}

bool NicConnectionHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}

// #warning() or #error() are never called by QXmlSimpleReader, so we do not
// need to implement them
// the message passed in exception is either the error string returned by
// #errorString() or some internal error message if, e.g., the xml code was
// malformed
bool NicConnectionHandler::fatalError(const QXmlParseException &exception)
{
    errorException = tr("Fatal error on line %1, column %2: %3")
                         .arg(exception.lineNumber())
                         .arg(exception.columnNumber())
                         .arg(exception.message());
    return false;
}

QString NicConnectionHandler::errorString() const
{
    return errorMessage;
}

QString NicConnectionHandler::error() const
{
    return errorException;
}

NicConnectionHandler::Type NicConnectionHandler::resultType() const
{
    return type;
}

QString NicConnectionHandler::infoMessage() const
{
    return infoText;
}

QString NicConnectionHandler::faultClass() const
{
    return classText;
}

// NicStatusHandler ============================================================

NicStatusHandler::NicStatusHandler() : result(Coh::Unknown), root(false)
{
}

bool NicStatusHandler::startElement(const QString &namespaceURI,
                                    const QString &localName,
                                    const QString &qName,
                                    const QXmlAttributes &attributes)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);
    Q_UNUSED(attributes);

    if (!root && qName != QL1S("nic:status")) {
        errorMessage = tr("Invalid root element.");
        return false;
    };

    root = true;

    // We are accepting all tags

    currentText.clear();
    return true;
}

bool NicStatusHandler::endElement(const QString &namespaceURI,
                                  const QString &localName,
                                  const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (qName == QL1S("code")) {
        unsigned value = currentText.toUInt();
        switch (value) {
        case 0:
            result = Coh::Unknown;
            break;
        case 1:
            result = Coh::Streaming;
            break;
        case 2:
            result = Coh::Idle;
            break;
        case 3:
            result = Coh::SafetyError;
            break;
        case 4:
            result = Coh::Error;
            break;
        case 5:
            result = Coh::Stopped;
            break;
        case 6:
            result = Coh::Waiting;
            break;
        case 7:
            result = Coh::Underflow;
            break;
        default:
            result = Coh::Unknown;
            break;
        }
    }

    return true;
}

bool NicStatusHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}

// #warning() or #error() are never called by QXmlSimpleReader, so we do not
// need to implement them
// the message passed in exception is either the error string returned by
// #errorString() or some internal error message if, e.g., the xml code was
// malformed
bool NicStatusHandler::fatalError(const QXmlParseException &exception)
{
    errorException = tr("Fatal error on line %1, column %2: %3")
                         .arg(exception.lineNumber())
                         .arg(exception.columnNumber())
                         .arg(exception.message());
    return false;
}

QString NicStatusHandler::errorString() const
{
    return errorMessage;
}

QString NicStatusHandler::error() const
{
    return errorException;
}

Coh::Status NicStatusHandler::status() const
{
    return result;
}

// NicSocketCohClient ==========================================================

NicSocketCohClient::NicSocketCohClient(const QString &address)
    : client(NIC_NEW_EH(nreSocketCreate("")))
{
    try {
        NIC_FUNC_EH(nreSocketConnect(client, address.toLocal8Bit().data()));
    } catch (...) {
        nreSocketClose(client);
        throw;
    }
}

NicSocketCohClient::~NicSocketCohClient()
{
    try {
        stop();
    } catch (...) {
        // ignore all errrors in the destructor
    }
    nreSocketClose(client);
}

void NicSocketCohClient::send(CohSequence *sequence)
{
    NIC_FUNC_EH(nreSocketSend(client, dumpCohSequenceNicXml(sequence).data()));
}

void NicSocketCohClient::start(Coh::TriggerMode trigger)
{
    NIC_FUNC_EH(nreSocketSend(client, triggerCommand(trigger)));
}

void NicSocketCohClient::stop()
{
    NIC_FUNC_EH(nreSocketSend(client, "<stop/>"));
}

Coh::Status NicSocketCohClient::status() const
{
    NIC_FUNC_EH(nreSocketSend(client, "<status/>"));
    const char *result = nreSocketReceive(client);

    NicStatusHandler handler;
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QXmlInputSource xmlInputSource;
    xmlInputSource.setData(QByteArray(result));

    if (!reader.parse(xmlInputSource))
        throw Exception(tr("Unable to parse status: %1").arg(handler.error()));

    return handler.status();
}

bool NicSocketCohClient::needsReloadForStop() const
{
    return true;
}

// Private methods =============================================================

const char *NicSocketCohClient::triggerCommand(Coh::TriggerMode trigger)
{
    switch (trigger) {
    case Coh::Immediate:
        return "<start>immediate</start>";
    case Coh::External:
        return "<start>external</start>";
    case Coh::Bilateral:
        return "<start>bilateral</start>";
    default:
        qFatal("Unknown trigger type %u", trigger);
    }
    // Silence the compiler
    return "<start>immediate</start>";
}

const char *NicSocketCohClient::functionErrorHandler(const char *result,
                                                     const char *message)
{
    // TODO: use message
    Q_UNUSED(message);

    NicConnectionHandler handler;
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QXmlInputSource xmlInputSource;
    xmlInputSource.setData(QByteArray(result));

    if (!reader.parse(xmlInputSource))
        throw Exception(tr("Unable to parse status: %1").arg(handler.error()));

    if (handler.resultType() == NicConnectionHandler::TYPE_FAULT)
        throw Exception(tr("NIC function failed: %1: %2")
                            .arg(handler.faultClass(), handler.infoMessage()));

    return result;
}

template <typename T>
T NicSocketCohClient::newErrorHandler(T ptr, const char *message)
{
    if (!ptr)
        throw Exception(tr("NIC new failed: %1").arg(QL1S(message)));
    return ptr;
}

// NicSocketCohClientCreator ===================================================

CohClient *NicSocketCohClientCreator::createCohClient(const QString &device)
{
    return new NicSocketCohClient(device);
}

QStringList NicSocketCohClientCreator::cohDevices() const
{
    return QStringList() << QL1S("l34-0");
}

QString NicSocketCohClientCreator::cohDriverName() const
{
    return QL1S("nic-socket");
}

#include "nicsocketcohclient.moc"
