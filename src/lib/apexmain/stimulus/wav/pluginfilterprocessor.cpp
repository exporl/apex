/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "filter/pluginfilterinterface.h"
#include "pluginfilterprocessor.h"

#include "services/mainconfigfileparser.h"
#include "services/pluginloader.h"
#include "exceptions.h"

#include <QPluginLoader>

using namespace streamapp;

namespace apex {

namespace stimulus {

    // PluginFilterProcessor =======================================================

    /**
     *
     * @param channels  number of channels
     * @param size      blocksize (frames)
     * @param fs        sample rate (samples/s)
     */
    PluginFilterProcessor::PluginFilterProcessor (unsigned channels,
            unsigned size,
            unsigned fs,
            const QString &name) :
        StreamProcessor (channels, size)
    {
        qDebug ("Looking for %s plugin", qPrintable (name));


        PluginFilterCreator* creator = PluginLoader::Get().
            createPluginCreator<PluginFilterCreator>(name);
        m_plugin.reset (creator->createFilter (name, channels, size, fs));
        return;

/*
        // end test

    QStringList fileNames;

        QString fileName = Paths::findReadableFile (name,
                QStringList() << Paths::Get().GetBinaryPluginPath()
                <<  Paths::Get().GetExecutablePath(),
#ifdef LINUX
                QStringList() << ".so",
                QStringList() << "lib"
#else
                QStringList() << ".dll"
#endif
                );

        if (!fileName.isEmpty())
            fileNames.append (fileName);

        fileNames += Paths::findReadableFiles (
                QStringList() <<Paths::Get().GetBinaryPluginPath(),
#ifdef LINUX
                QStringList() << ".so"
#else
                QStringList() << ".dll"
#endif
                );

        Q_FOREACH (const QString &fileName, fileNames) {
            qDebug("Looking for %s", qPrintable (fileName));

            // load plugin into pluginfilterinterface
            QPluginLoader loader (fileName);
            loader.load();
            if (!loader.isLoaded()) {
#if QT_VERSION < 0x040300
                qDebug ("Cannot load filter plugin");
#else
                qDebug ("Cannot load filter plugin: %s",
                        qPrintable (loader.errorString()));
#endif
                continue;
            }

            PluginFilterCreator *creator = qobject_cast<PluginFilterCreator*>
                (loader.instance());
            if (!creator) {
#if QT_VERSION < 0x040300
                qDebug ("Filter plugin has wrong type");
#else
                qDebug ("Filter plugin has wrong type: %s",
                        qPrintable (loader.errorString()));
#endif
                continue;
            }
            qDebug ("Available filters: %s", qPrintable
                    (creator->availablePlugins().join (", ")));
            m_plugin.reset (creator->createFilter (name, channels, size, fs));
            if (!m_plugin.get()) {
                qDebug ("Filter could not be created");
                continue;
            }
            return;
        }
        throw ApexStringException (QString ("No suitable library found that "
                    "provides the '%1' plugin").arg (name));
        */
    }

    PluginFilterProcessor::~PluginFilterProcessor()
    {
    }

    void PluginFilterProcessor::resetParameters()
    {
        const appcore::Lock locker (section);

        m_plugin->resetParameters();
    }

    bool PluginFilterProcessor::setParameter (const QString &type, int channel,
            const QString &value)
    {
        const appcore::Lock locker (section);

        if (!m_plugin->setParameter (type, channel, value))
            throw ApexStringException (m_plugin->errorMessage());

        return true;
    }

    bool PluginFilterProcessor::isValidParameter (const QString &type,
            int channel) const
    {
        const appcore::Lock locker (section);

        return m_plugin->isValidParameter (type, channel);
    }


    //void PluginFilterProcessor::Reset() {
    //    const appcore::Lock locker (section);
    //    m_plugin->resetParameters ();
    //}

    void PluginFilterProcessor::Prepare (unsigned numberOfFrames) {
        const appcore::Lock locker (section);
        if (!m_plugin->prepare (numberOfFrames))
            throw ApexStringException (m_plugin->errorMessage());
    }

    const Stream &PluginFilterProcessor::mf_DoProcessing (const Stream& ac_Output)
    {
        const appcore::Lock locker (section);

        /* const unsigned channels = ac_Output.mf_nGetChannelCount();
           const unsigned frames = ac_Output.mf_nGetBufferSize();*/
        double** const data = ac_Output.mf_pGetArray();

        m_plugin->process (data);

        return ac_Output;
    }

}

}
