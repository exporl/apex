/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#include "apextools/exceptions.h"

#include "calibrationdatabase.h"

#include <QCoreApplication>
#include <QSet>
#include <QSettings>
#include <QStringList>
#include <QXmlDefaultHandler>

namespace apex
{

class CalibrationDatabasePrivate
{
public:
    static QString prefix()
    {
        return "Calibration";
    }

    static QString hardwareSetups()
    {
        return prefix() + "/config/hardware";
    }

    static QString currentHardwareSetup()
    {
        return prefix() + "/config/current";
    }

    static QString data (const QString &setup = QString(),
            const QString &profile = QString(),
            const QString &name = QString(), const QString &key = QString())
    {
        QString result (prefix() + "/data");
        if (setup.isEmpty())
            return result;
        result += "/" + prepare (setup);
        if (profile.isEmpty())
            return result;
        result += "/" + prepare (profile);
        if (name.isEmpty())
            return result;
        result += "/" + prepare (name);
        if (key.isEmpty())
            return result;
        result += "/" + prepare (key);
        return result;
    }


    static QString parameter (const QString &setup, const QString &profile,
            const QString &name)
    {
        return data (setup, profile, name, "parameter");
    }

    static QString target (const QString &setup, const QString &profile,
            const QString &name)
    {
        return data (setup, profile, name, "target");
    }

    static QString prepare (const QString &value)
    {
        return QString (value).replace ('/', '_');
    }

    static QSettings* getQSettings(bool global = false)
    {
        QSettings* newSettings = new QSettings();
        if(global) {
            newSettings = new QSettings(QSettings::SystemScope, QSettings().organizationName(), QSettings().applicationName());
        }

        settings.reset(newSettings);
        return newSettings;
    }

    static bool isGlobal(const QString& setup)
    {
        QSettings settings(QSettings::SystemScope, QSettings().organizationName(), QSettings().applicationName());
        QStringList globalSetups = settings.value(hardwareSetups()).toStringList();
        return globalSetups.contains(setup);
    }
private:
    static QScopedPointer<QSettings> settings;
};
QScopedPointer<QSettings> CalibrationDatabasePrivate::settings;

class CalibrationSetupHandler : public QXmlDefaultHandler
{
    Q_DECLARE_TR_FUNCTIONS (GenericXmlHandler)
public:
    CalibrationSetupHandler();

    // Returns the message text of the last exception
    QString error() const;

    // Reimplementing QXmlErrorHandler and QXmlContentHandler
    bool startElement (const QString &namespaceURI,
            const QString &localName, const QString &qName,
            const QXmlAttributes &attributes);
    bool endElement (const QString &namespaceURI, const QString &localName,
            const QString &qName);
    bool characters (const QString &str);
    bool fatalError (const QXmlParseException &exception);
    QString errorString() const;

    QString hardwareSetup() const;

private:
    bool root;
    bool calibration;
    // Only modified by #fatalError()
    QString errorException;
    // Must be set by handlers that return false
    QString errorMessage;

    QString currentText;

    QString setup;

    QString currentProfile;
    QString currentName;
    double currentTarget;
    double currentParameter;
};

class CalibrationSetupGenerator
{
public:
    CalibrationSetupGenerator (const QString &setup);

    void write (QIODevice *device);

    const QString setup;

    QTextStream out;
};

// CalibrationDatabase =========================================================

CalibrationDatabase::CalibrationDatabase()
    // No private data yet
{
}

CalibrationDatabase::~CalibrationDatabase()
{
}

QStringList CalibrationDatabase::hardwareSetups() const
{
    QStringList local = QSettings().value(CalibrationDatabasePrivate::hardwareSetups()).toStringList();
    QStringList global = QSettings(QSettings::SystemScope, QSettings().organizationName(), QSettings().applicationName()).value(CalibrationDatabasePrivate::hardwareSetups()).toStringList();
    QStringList both(local);
    both.append(global);

    return both;
}

QStringList CalibrationDatabase::calibrationProfiles
    (const QString &setup) const
{
    QSettings* settings = CalibrationDatabasePrivate::getQSettings(CalibrationDatabasePrivate::isGlobal(setup));
    settings->beginGroup (CalibrationDatabasePrivate::data (setup));
    return settings->childGroups();
}

QStringList CalibrationDatabase::parameterNames (const QString &setup,
        const QString &profile) const
{
    QSettings* settings = CalibrationDatabasePrivate::getQSettings(CalibrationDatabasePrivate::isGlobal(setup));
    settings->beginGroup (CalibrationDatabasePrivate::data (setup, profile));
    return settings->childGroups();
}

unsigned CalibrationDatabase::calibrationCount (const QString &setup) const
{
    unsigned result = 0;
    QSettings* settings = CalibrationDatabasePrivate::getQSettings(CalibrationDatabasePrivate::isGlobal(setup));

    settings->beginGroup (CalibrationDatabasePrivate::data (setup));
    Q_FOREACH (const QString &profile, settings->childGroups()) {
        settings->beginGroup (profile);
        result += settings->childGroups().count();
        settings->endGroup();
    }
    return result;
}

void CalibrationDatabase::addHardwareSetup (const QString &setup)
{
    QSettings settings;

    QStringList hardwareSetups = settings.value
        (CalibrationDatabasePrivate::hardwareSetups()).toStringList();
    if (!hardwareSetups.contains (setup))
        settings.setValue (CalibrationDatabasePrivate::hardwareSetups(),
                hardwareSetups << setup);
}

void CalibrationDatabase::renameHardwareSetup (const QString &setup,
        const QString &newName)
{
    if (newName == setup)
        return;
    addHardwareSetup (newName);
    Q_FOREACH (const QString &profile, calibrationProfiles (setup))
        Q_FOREACH (const QString &name, parameterNames (setup, profile))
            calibrate (newName, profile, name,
                    targetAmplitude (setup, profile, name),
                    outputParameter (setup, profile, name));
    removeHardwareSetup (setup);
}

void CalibrationDatabase::removeHardwareSetup (const QString &setup)
{
    QSettings settings;

    QStringList hardwareSetups = settings.value
        (CalibrationDatabasePrivate::hardwareSetups()).toStringList();
    hardwareSetups.removeAll (setup);
    settings.setValue (CalibrationDatabasePrivate::hardwareSetups(),
            hardwareSetups);

    settings.remove (CalibrationDatabasePrivate::data (setup));
}

bool CalibrationDatabase::isCalibrated (const QString &setup,
        const QString &profile, const QString &name) const
{
    QSettings* settings = CalibrationDatabasePrivate::getQSettings(CalibrationDatabasePrivate::isGlobal(setup));

    return settings->contains (CalibrationDatabasePrivate::parameter
            (setup, profile, name)) &&
        settings->contains (CalibrationDatabasePrivate::target
            (setup, profile, name));
}

bool CalibrationDatabase::isGlobal(const QString &setup) const {
    return CalibrationDatabasePrivate::isGlobal(setup);
}

double CalibrationDatabase::targetAmplitude (const QString &setup,
        const QString &profile, const QString &name) const
{
    return CalibrationDatabasePrivate::getQSettings(CalibrationDatabasePrivate::isGlobal(setup))->value(
                CalibrationDatabasePrivate::target(setup, profile, name)).toDouble();
}

double CalibrationDatabase::outputParameter (const QString &setup,
        const QString &profile, const QString &name) const
{
    return CalibrationDatabasePrivate::getQSettings(CalibrationDatabasePrivate::isGlobal(setup))->value(
                CalibrationDatabasePrivate::parameter(setup, profile, name)).toDouble();
}

void CalibrationDatabase::calibrate (const QString &setup,
        const QString &profile, const QString &name,
        double target, double parameter)
{
    addHardwareSetup (setup);

    QSettings settings;
    settings.setValue (CalibrationDatabasePrivate::parameter
            (setup, profile, name), parameter);
    settings.setValue (CalibrationDatabasePrivate::target
            (setup, profile, name), target);
}

void CalibrationDatabase::remove (const QString &setup,
        const QString &profile, const QString &name)
{
    QSettings().remove (CalibrationDatabasePrivate::data (setup, profile,
                name));
}

QString CalibrationDatabase::currentHardwareSetup() const
{
    return QSettings().value
        (CalibrationDatabasePrivate::currentHardwareSetup()).toString();
}

void CalibrationDatabase::setCurrentHardwareSetup (const QString &setup)
{
    QSettings().setValue
        (CalibrationDatabasePrivate::currentHardwareSetup(), setup);
}

// XML stuff ===================================================================

void CalibrationDatabase::exportHardwareSetup (const QString &setup,
        const QString &fileName) const
{
    QFile file (fileName);
    // No QFile::Text so we always create linux line endings
    if (!file.open (QFile::WriteOnly))
        throw ApexStringException
            (QString ("Unable to open calibration file %1: %2")
                .arg (fileName, file.errorString()));

    CalibrationSetupGenerator (setup).write (&file);
}

QString CalibrationDatabase::importHardwareSetup (const QString &fileName)
{
    CalibrationSetupHandler handler;
    QXmlSimpleReader reader;
    reader.setContentHandler (&handler);
    reader.setErrorHandler (&handler);

    QFile file (fileName);
    if (!file.open (QFile::ReadOnly | QFile::Text))
        throw ApexStringException
            (QString ("Unable to open calibration file %1: %2")
                .arg (fileName, file.errorString()));

    QXmlInputSource xmlInputSource (&file);
    if (!reader.parse (xmlInputSource)) {
        const QString setup = handler.hardwareSetup();
        if (!setup.isEmpty())
            CalibrationDatabase().removeHardwareSetup (setup);
        throw ApexStringException
            (QString ("Unable to read calibration file %1: %2")
                .arg (fileName, handler.error()));
    }
    return handler.hardwareSetup();
}

// CalibrationSetupHandler =====================================================

CalibrationSetupHandler::CalibrationSetupHandler() :
    root (false),
    calibration (false)
{
}

bool CalibrationSetupHandler::startElement (const QString
        &namespaceURI, const QString &localName, const QString &qName,
        const QXmlAttributes &attributes)
{
    Q_UNUSED (namespaceURI);
    Q_UNUSED (localName);

    if (!root && qName != "calibrationsetup") {
        errorMessage = tr ("Invalid root element.");
        return false;
    }

    if (qName == "calibrationsetup") {
        const QString version = attributes.value ("version");
        if (!version.isEmpty() && version != "1.0") {
            errorMessage = tr ("Unsupported version.");
            return false;
        }
        root = true;
    } else if (qName == "profile") {
        currentProfile = attributes.value ("name");
    } else if (qName == "calibration") {
        calibration = true;
    } else if (qName == "name" || qName == "target" || qName == "parameter") {
        // Do nothing, everything is done in #endElement
    } else {
        errorMessage = tr ("Unknown tag: %1").arg (qName);
        return false;
    }

    currentText.clear();
    return true;
}

bool CalibrationSetupHandler::endElement (const QString &namespaceURI,
        const QString &localName, const QString &qName)
{
    Q_UNUSED (namespaceURI);
    Q_UNUSED (localName);

    if (qName == "name" && !calibration) {
        setup = currentText;
        const QSet<QString> setups = QSet<QString>::fromList
            (CalibrationDatabase().hardwareSetups());
        if (setups.contains (setup)) {
            for (unsigned i = 1;; ++i) {
                const QString newSetup = QString ("%2 %1").arg (i).arg (setup);
                if (!setups.contains (newSetup)) {
                    setup = newSetup;
                    break;
                }
            }
        }
    } else if (qName == "name" && calibration) {
        currentName = currentText;
    } else if (qName == "target") {
        currentTarget = currentText.toDouble();
    } else if (qName == "parameter") {
        currentParameter = currentText.toDouble();
    } else if (qName == "calibration") {
        CalibrationDatabase().calibrate (setup, currentProfile,
                currentName, currentTarget, currentParameter);
        calibration = false;
    }

    return true;
}

bool CalibrationSetupHandler::characters (const QString &str)
{
    currentText += str;
    return true;
}

// #warning() or #error() are never called by QXmlSimpleReader, so we do not
// need to implement them
// the message passed in exception is either the error string returned by
// #errorString() or some internal error message if, e.g., the xml code was
// malformed
bool CalibrationSetupHandler::fatalError (const QXmlParseException &exception)
{
    errorException = tr ("Fatal error on line %1, column %2: %3")
        .arg (exception.lineNumber())
        .arg (exception.columnNumber())
        .arg (exception.message());
    return false;
}

QString CalibrationSetupHandler::errorString() const
{
    return errorMessage;
}

QString CalibrationSetupHandler::error() const
{
    return errorException;
}

QString CalibrationSetupHandler::hardwareSetup() const
{
    return setup;
}

// CalibrationSetupGenerator ===================================================

static QString xmlIndent (unsigned depth, unsigned indentSize = 4)
{
    return QString (indentSize * depth, ' ');
}

static QString xmlEscapedText (const QString &text)
{
    QString result = text;
    result.replace ("&", "&amp;");
    result.replace ("<", "&lt;");
    result.replace (">", "&gt;");
    return result;
}

static QString xmlEscapedAttribute (const QString &text)
{
    QString result = xmlEscapedText (text);
    result.replace ("\"", "&quot;");
    result.prepend ("\"");
    result.append ("\"");
    return result;
}

CalibrationSetupGenerator::CalibrationSetupGenerator (const QString &setup) :
    setup (setup)
{
}

void CalibrationSetupGenerator::write (QIODevice *device)
{
    out.setDevice (device);
    out.setCodec ("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE calibrationsetup>\n"
        << "<calibrationsetup version=\"1.0\">\n";

    out << xmlIndent (1) << "<name>" << xmlEscapedText (setup) << "</name>\n";

    const CalibrationDatabase db;

    Q_FOREACH (const QString &profile, db.calibrationProfiles (setup)) {
        out << xmlIndent (1) << "<profile name="
            << xmlEscapedAttribute (profile) << ">\n";
        Q_FOREACH (const QString &name, db.parameterNames (setup, profile)) {
            const QString target = QString::number (db.targetAmplitude
                (setup, profile, name), 'g', 10);
            const QString parameter = QString::number (db.outputParameter
                (setup, profile, name), 'g', 10);
            out << xmlIndent (2) << "<calibration>\n";
            out << xmlIndent (3) << "<name>"
                << xmlEscapedText (name) << "</name>\n";
            out << xmlIndent (3) << "<target>"
                << xmlEscapedText (target) << "</target>\n";
            out << xmlIndent (3) << "<parameter>"
                << xmlEscapedText (parameter) << "</parameter>\n";
            out << xmlIndent (2) << "</calibration>\n";
        }
        out << xmlIndent (1) << "</profile>\n";
    }
    out << "</calibrationsetup>\n";
}

} // namespace rba
