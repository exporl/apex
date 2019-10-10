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

#ifndef _BERTHA_SRC_LIB_BERTHA_BLOCKINTERFACE_H_
#define _BERTHA_SRC_LIB_BERTHA_BLOCKINTERFACE_H_

#include "global.h"

#include <math.h>

#include <QList>
#include <QVariant>
#include <QVector>

#include <QtPlugin>

typedef float PortDataType;

class BlockPort
{
public:
    enum Domain { TimeDomain, FrequencyDomain };

    BlockPort(const QString &name, unsigned sampleRate, unsigned blockSize,
              Domain domain)
        : name(name),
          sampleRate(sampleRate),
          blockSize(blockSize),
          domain(domain)
    {
    }

    const QString name;
    const unsigned sampleRate; // 0 means global rate (of the plugin)
    const unsigned blockSize;  // 0 means global blockSize (of the plugin)
    const Domain domain;
};

class BlockInputPort : public BlockPort
{
public:
    BlockInputPort(const QString &name, unsigned sampleRate = 0,
                   unsigned blockSize = 0, Domain domain = TimeDomain)
        : BlockPort(name, sampleRate, blockSize, domain)
    {
    }

    QWeakPointer<PortDataType> inputData;
};

class BlockOutputPort : public BlockPort
{
public:
    BlockOutputPort(const QString &name, unsigned sampleRate = 0,
                    unsigned blockSize = 0, Domain domain = TimeDomain)
        : BlockPort(name, sampleRate, blockSize, domain)
    {
    }

    QSharedPointer<PortDataType> outputData;
};

class BlockParameter
{
public:
    enum Unit { LinearUnit, LogarithmicUnit };

    BlockParameter(const QString &name, const QString description,
                   Unit unit = LinearUnit, double minimum = -HUGE_VAL,
                   double maximum = HUGE_VAL)
        : name(name),
          description(description),
          unit(unit),
          minimum(minimum),
          maximum(maximum)
    {
    }

    BlockParameter() : unit(LinearUnit), minimum(-HUGE_VAL), maximum(HUGE_VAL)
    {
    }

    QString name;
    QString description;
    Unit unit;

    // only for numeric types (int, double)
    double minimum;
    double maximum;
};

class BlockDescription
{
public:
    BlockDescription() : metaObject(NULL)
    {
    }

    BlockDescription(const QMetaObject *metaObject,
                     const QString &name = QString(),
                     QVariant userData = QVariant())
        : metaObject(metaObject), name(name), userData(userData)
    {
    }

    const QMetaObject *metaObject;
    QString name;
    QVariant userData;
};

class BERTHA_EXPORT BasicBlockInterface
{
public:
    virtual ~BasicBlockInterface();
    virtual QList<BlockInputPort *> inputPorts() = 0;
    virtual QList<BlockOutputPort *> outputPorts() = 0;

    virtual QList<BlockParameter> parameterInfos() const
    {
        return QList<BlockParameter>();
    }
};

class BERTHA_EXPORT BlockInterface : public BasicBlockInterface
{
public:
    virtual ~BlockInterface();
    // Your plugin class needs to derive from QObject, implement
    // BlockInterface and provide a least one Q_INVOKABLE constructor like this:
    //
    // class Plugin: public QObject, public BlockInterface {
    //     Q_OBJECT
    //     Q_INTERFACES(BlockInterface)
    // public:
    //     Q_INVOKABLE Plugin(unsigned blockSize, unsigned sampleRate,
    //         int inputPorts, int outputPort);
    //     Q_INVOKABLE Plugin(unsigned blockSize, unsigned sampleRate,
    //         int inputPorts, int outputPorts,
    //         const QVariant &userData);
    //     Q_INVOKABLE Plugin(unsigned blockSize, unsigned sampleRate,
    //         int inputPorts, int outputPorts, const QVariantMap
    //         &readOnlyParameters);
    //     Q_INVOKABLE Plugin(unsigned blockSize, unsigned sampleRate,
    //         int inputPorts, int outputPorts, const QVariantMap
    //         &readOnlyParameters,
    //         const QVariant &userData);
    //     ...
    // };

    /**************************************************************************
     * Information about this plugin
     * Can be called at any time, returned values must not be changed during the
     * lifetime of the object
     **************************************************************************/

    /**
    * Return list of input ports. The indices in the list will be used later
    * to refer to the parameters. The default nomenclature of input ports is
    * input, input-1, vad, etc., only deviate from this if absolutely necessary.
    */
    // virtual QList<BlockInputPort *> inputPorts() = 0;

    /**
    * Return list of output ports. The indices in the list will be used later
    * to refer to the parameters. The default nomenclature of output ports is
    * output, output-1, etc., only deviate from this if absolutely necessary.
    */
    // virtual QList<BlockOutputPort *> outputPorts() = 0;

    /**
    * Return list of parameters. This is provided for backwards compatibility
    * and is not necessary if you provide parameter information with Q_CLASSINFO
    * or if you don't want fancy features such as descriptions etc.
    */
    // virtual QList<BlockParameter> parameterInfos() const
    // {
    //     return QList<BlockParameter>();
    // }

    /**************************************************************************
     * Actions
     * Are called after setup calls
     * Will be called in the following order:
     * prepare()
     * process() (N times)
     * release()
     **************************************************************************/

    /**
    * @numberOfFrames if non-zero the process() callback will be called
    *                 numberOfFrames times in total
    */
    virtual QString prepare(unsigned numberOfFrames) = 0;

    /**
    * Process the data in the input ports to the output ports
    * Before process is called, the framework will initialize the data pointers
    * in the input and output ports
    **/
    virtual void process() = 0;

    /**
    * Is called after al processing is done, before the destructor
    * after release(), process() will not be called any more
    */
    virtual void release() = 0;
};

class BlockCreatorInterface
{
public:
    virtual ~BlockCreatorInterface()
    {
    }

    virtual QList<BlockDescription> availableBlocks() const = 0;
};

Q_DECLARE_INTERFACE(BlockCreatorInterface, "be.exporl.bertha.blockcreator/1.0")
Q_DECLARE_INTERFACE(BlockInterface, "be.exporl.bertha.block/1.0")

Q_DECLARE_METATYPE(QVector<double>)
Q_DECLARE_METATYPE(QVector<QVector<double>>)

#endif
