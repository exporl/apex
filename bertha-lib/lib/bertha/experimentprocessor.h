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

#ifndef _BERTHA_SRC_LIB_BERTHA_EXPERIMENTPROCESSOR_H_
#define _BERTHA_SRC_LIB_BERTHA_EXPERIMENTPROCESSOR_H_

#include "blockinterface.h"
#include "global.h"

#include <QList>
#include <QMap>
#include <QStringList>

namespace bertha
{

class ExperimentData;

class ExperimentProcessorPrivate;

class BERTHA_EXPORT ExperimentProcessor : public QObject
{
    Q_OBJECT
public:
    enum State { ExperimentLoaded, ExperimentPrepared, ExperimentRunning };

    // may throw
    ExperimentProcessor(const ExperimentData &experiment, bool lazy = false);
    ~ExperimentProcessor();

    // may throw
    void prepare();
    // may throw
    void start();
    void stop();
    void release();

    // may throw
    // if running sets active leafnodes and parameterlist before continuing
    void setActiveLeafNodes(
        const QStringList &leafNodes,
        const QMap<QString, QList<QPair<QString, QVariant>>> &parameters =
            QMap<QString, QList<QPair<QString, QVariant>>>(),
        bool recreate = false);

    QStringList leafNodes() const;
    QStringList blockNames() const;

    // may throw
    QStringList parameterNames(const QString &blockId) const;
    // if running, the parameters are set in the processing thread; may throw
    void setParameterValue(const QString &blockId, const QString &parameterId,
                           const QVariant &value);
    // if running, the parameters are set in the processing thread; may throw
    // parameters: QMap<blockId, QMap<parameterId,value> >
    void setParameterList(
        const QMap<QString, QList<QPair<QString, QVariant>>> &parameters);
    void
    setParameterList(const QMap<QString, QMap<QString, QVariant>> &parameters);
    // may throw
    QVariant parameterValue(const QString &blockId,
                            const QString &parameterId) const;
    // may throw
    QVariant sanitizedParameterValue(const QString &blockId,
                                     const QString &parameterId) const;
    // may throw
    QString parameterStringValue(const QString &blockId,
                                 const QString &parameterId) const;
    // may throw
    BlockParameter parameterInfo(const QString &blockId,
                                 const QString &parameterId) const;
    // may throw
    QString parameterType(const QString &blockId,
                          const QString &parameterId) const;
    // may throw
    QStringList enumeratorTypes(const QString &blockId,
                                const QString &parameterId) const;
    State state() const;
    /** Number of the buffer that is currently being played/recorded
      * Will wrap around if necessary
      * The first buffer that Bertha processes is 0
      */
    int currentBuffer() const;

    // testing purposes
    QStringList dumpActiveConnections() const;

Q_SIGNALS:
    void stateChanged(bertha::ExperimentProcessor::State state);

private:
    ExperimentProcessorPrivate *const d;
};

}; // namespace bertha

Q_DECLARE_METATYPE(bertha::ExperimentProcessor::State)

#endif
