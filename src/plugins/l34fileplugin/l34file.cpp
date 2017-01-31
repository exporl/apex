/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

#include "apexmain/l34stimulus/apexnresocketinterface.h"

#include <QStringList>
#include <QTime>
#include <QFile>
#include <QTextStream>

#include <QtPlugin>

using namespace apex::stimulus;

class ApexL34FilePlugin: public ApexNreSocketInterface
{
public:
    ApexL34FilePlugin() :
        running(false)
    {
        m_file = new QFile( QString("L34output.xml"));
        m_file->open( QIODevice::WriteOnly);
        Q_ASSERT(m_file->isOpen());
        m_stream=new QTextStream(m_file);
        *m_stream << "<nic>" << endl;
    }

    ~ApexL34FilePlugin()
    {
        *m_stream << endl << "</nic>" << endl;
        m_file->close();
        delete m_file;
        delete m_stream;
    }

    virtual QString send(const QString &message)
    {
        printf("L34File::send: %s\n", qPrintable(message));
        if (message.startsWith(QLatin1String("<start>"))) {
            running = true;
            currentTime = QTime::currentTime();
            receiveText = QString();
        } else if (message.startsWith(QLatin1String("<status/>"))) {
            if (!running) {
                receiveText = QLatin1String("finished<code>2");
            } else if (currentTime.elapsed() > 1 * 1000) {
                running = false;
                receiveText = QLatin1String("finished<code>2");
            } else {
                // trick the L34device into continuing
                receiveText = QLatin1String("streaming<code>1<code>6");
            }
        }
        *m_stream << message << endl;
        return QString();
    }

    virtual QString connect(const QString &device)
    {
        Q_UNUSED(device);
        return QString();
    }

    virtual QString receive()
    {
        const QString result = receiveText;
        receiveText = QString();
        return result;
    }

    virtual void disconnect()
    {
    }

private:
    bool running;
    QTime currentTime;
    QString receiveText;

    QFile* m_file;
    QTextStream* m_stream;
};

class ApexL34FilePluginCreator :
    public QObject,
    public ApexNreSocketCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(ApexNreSocketCreatorInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.l34plugin")
#endif
public:
    virtual QStringList availablePlugins() const
    {
        return QStringList() << QLatin1String("l34plugin");
    }

    virtual ApexNreSocketInterface *create(const QString &host, int deviceNr) const
    {
        Q_UNUSED(host);
        Q_UNUSED(deviceNr);
        return new ApexL34FilePlugin();
    }
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(l34plugin, ApexL34FilePluginCreator)
#endif

#include "l34file.moc"
