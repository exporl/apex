/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
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

/**  Possible conversions for parameters, other conversions on own risk
*    to      <- from
*    ------------------------------------------------------------------
*    int     <- int
*            <- double (loss behind comma)
*            <- string
*
*    double  <- int
*            <- double
*            <- string
*
*    float   <- int
*            <- double
*            <- string
*            <- float
*
*    bool    <- int
*            <- double
*            <- string
*            <- bool
*
*    string  <- int
*            <- double
*            <- float
*            <- stringlist
*            <- vector<double>
*            <- vector<vector<double> >
*            <- bool
*
*    stringlist <- list<string>
*               <- string
*               <- stringlist
*
*    vector<double> <- list<dis>  [*]
*                   <- string (e.g. "[1.1 2.2 3.3]")
*
*    vector<vector<double> > <- list<dis>  [*](values are combined in a vector
                                               containing a single vector)
*                            <- list<list<dis> >   [*]
*                            <- string (e.g. "[[1.1 1.2][2.1 2.2]]"
                                          or "[1.1 1.2;2.1 2.2]" )
*
*    enum    <- int
*            <- string (enumkey or numberstring)
*
*    [*] dis = double or integer or string
**/

#ifndef _BERTHA_SRC_LIB_BERTHA_ABSTRACTPROCESSOR_H_
#define _BERTHA_SRC_LIB_BERTHA_ABSTRACTPROCESSOR_H_

#include "common/exception.h"
#include "common/pluginloader.h"

#include "blockdata.h"
#include "blockinterface.h"
#include "global.h"

#include <QCoreApplication>
#include <QMetaProperty>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QStringList>
#include <QVariant>
#include <QVector>

namespace bertha
{

class BERTHA_EXPORT AbstractProcessor : public QObject
{
    Q_OBJECT
public:
    AbstractProcessor(const BlockData &data, unsigned blockSize,
                      unsigned sampleRate,
                      const QMap<QString, BlockDescription> &plugins);
    virtual ~AbstractProcessor();

    BlockOutputPort *findOutputPort(const QString &name) const;
    BlockInputPort *findInputPort(const QString &name) const;

    QString id() const;
    unsigned getBlockSize() const;
    unsigned getSampleRate() const;

    QStringList parameters() const;
    unsigned parameterIndex(const QString &parameter) const;
    void setParameterValue(unsigned index, const QVariant &value);

    QVariant parameterValue(unsigned index) const;
    QVariant sanitizedParameterValue(unsigned index) const;
    QString parameterStringValue(unsigned index) const;

    QVariant prepareParameterValue(unsigned index, const QVariant &value) const;

    BlockParameter parameterInfo(unsigned index) const;
    QString parameterType(unsigned index) const;
    QStringList enumeratorTypes(unsigned index) const;

protected:
    template <typename Interface>
    Interface *castPlugin();

    template <typename CreatorInterface>
    static QMap<QString, BlockDescription> availableCreatorPlugins();

    // throws if not found
    template <typename BlockPort>
    BlockPort *findPort(const QMap<QString, BlockPort *> &ports,
                        const QString &name) const;
    // NULL if not found
    const char *classInfoValue(const QMetaProperty &property, const char *what);
    void setupOutputBuffers(const QList<BlockOutputPort *> &ports);
    // needs a valid metaObject, returns parameters with fixed casing
    QVariantMap prepareParameters(const QVariantMap &parameters);
    // filters the parameters (with valid casing) and returns only the read-only
    // ones
    QVariantMap readOnlyParameters(const QVariantMap &parameters);
    void setupParameters(const QList<BlockParameter> &parameters,
                         const QVariantMap &initialParameters);

    QVariant checkParameterConstraints(unsigned index,
                                       const QVariant &value) const;

    QList<BlockParameter> blockParameterInfos() const;

protected:
    QString blockId;
    unsigned blockSize;
    unsigned sampleRate;

    QScopedPointer<QObject> plugin;
    BasicBlockInterface *basicBlock;

    const QMetaObject *pluginMetaObject;

    QStringList parameterNames;
    QList<QMetaProperty> parameterProperties;
    QList<BlockParameter> parameterInfos;

    QMap<QString, BlockOutputPort *> outputPorts;
    QMap<QString, BlockInputPort *> inputPorts;
};

template <typename CreatorInterface>
QMap<QString, BlockDescription> AbstractProcessor::availableCreatorPlugins()
{
    QMap<QString, BlockDescription> result;

    Q_FOREACH (const CreatorInterface *const creator,
               cmn::PluginLoader().availablePlugins<CreatorInterface>()) {
        Q_FOREACH (const BlockDescription &block, creator->availableBlocks()) {
            const QString name =
                !block.name.isEmpty()
                    ? block.name
                    : QString::fromLatin1(block.metaObject->className())
                          .replace(QRegExp(QLatin1String("Plugin$")),
                                   QString());
            result.insert(name, block);
        }
    }

    return result;
}

template <typename Interface>
Interface *AbstractProcessor::castPlugin()
{
    Interface *casted = dynamic_cast<Interface *>(basicBlock);
    if (!casted)
        throw cmn::Exception(
            tr("Unable to instantiate: Interface not implemented"));
    return casted;
}

template <typename BlockPort>
BlockPort *AbstractProcessor::findPort(const QMap<QString, BlockPort *> &ports,
                                       const QString &name) const
{
    BlockPort *port = ports.value(name);
    if (!port)
        throw cmn::Exception(
            tr("Unable to find port %1 of %2").arg(name, blockId));
    return port;
}

}; // namespace bertha

#endif
