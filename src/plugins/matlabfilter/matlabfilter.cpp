/******************************************************************************
 * Copyright (C) 2008  Tom Francart                                           *
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

#include "apexmain/filter/pluginfilterinterface.h"

#include "apextools/global.h"

#include "engine.h"
#include "mex.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QMap>
#include <QStringList>

#include <cmath>

Q_DECLARE_LOGGING_CATEGORY(APEX_MATLABFILTER)
Q_LOGGING_CATEGORY(APEX_MATLABFILTER, "apex.matlabfilter")

class MatlabFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.matlabfilter")
#endif
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blocksize, unsigned fs) const;
};

class MatlabFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    MatlabFilter (unsigned channels, unsigned blockSize);
    ~MatlabFilter();

    virtual void resetParameters();
    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);
    virtual bool prepare (unsigned numberOfFrames);

    virtual void process (double * const *data);

private:
    bool initializeMatlab();
    void sendParameters();

    unsigned channels;
    unsigned blockSize;

    bool matlabInitialized;
    QString matlabPath;
    QString processfunction;
    QString preparefunction;

    QMap<QString,QString> otherparams;

    Engine *engine;
    mxArray *buffer;
};


// MatlabFilter ================================================================

MatlabFilter::MatlabFilter (unsigned channels, unsigned blockSize) :
    channels (channels),
    blockSize (blockSize),
    matlabInitialized(false),
    engine(0),
    buffer(0)
{
    initializeMatlab();
    resetParameters();

    // Connect with matlab server
}

MatlabFilter::~MatlabFilter()
{
    if (buffer)
        mxDestroyArray(buffer);
    if (engine)
        engClose(engine);
}

void MatlabFilter::resetParameters()
{
    /*disabled = false;
    baseGain = 0;*/
    otherparams.clear();
}


void MatlabFilter::sendParameters()
{
    // set parameters as global struct apex
    QStringList command;
    for (QMap<QString,QString>::const_iterator it=otherparams.begin();
    it!=otherparams.end(); ++it) {
        QString value;
        bool ok;
        it.value().toDouble(&ok);
        if (ok)
            value=it.value();
        else
            value="'"+it.value()+"'";
        command.push_back("apex." + it.key() + "="+value);
    }
    engEvalString(engine, command.join("\n").toLatin1());
    engEvalString(engine, "global apex");

    qCDebug(APEX_MATLABFILTER, "Sending comand: %s", qPrintable(command.join("\n")));
}

bool MatlabFilter::prepare (unsigned numberOfFrames)
{
    Q_UNUSED (numberOfFrames);

    // connect with matlab server
    if (!matlabInitialized) {
        bool ok=initializeMatlab();
        if (!ok)
            return false;
    }

    sendParameters();
    // call prepare function
    if (!preparefunction.isEmpty())
        engEvalString(engine, preparefunction.toLatin1() );

    return true;
}

bool MatlabFilter::initializeMatlab ()
{
    if (!matlabInitialized) {
        #ifdef Q_WS_WIN
        QString matlabCommand;
        #else
        QString matlabCommand(matlabPath+"/bin/matlab -nojvm -nosplash");
        #endif
        if (!(engine = engOpen(matlabCommand.toLatin1()))) {
            #ifdef Q_WS_WIN
            setErrorMessage (
            QString("Cannot start matlab engine using command %1.")
            .arg(matlabCommand));
            #else
            setErrorMessage (
            QString("Cannot start matlab engine using command %1. Did you install csh?")
            .arg(matlabCommand));
            #endif
            return false;
        }
        matlabInitialized=true;

        // create data buffer
        buffer = mxCreateDoubleMatrix(1, blockSize, mxREAL);
        Q_ASSERT(buffer);
        qCDebug(APEX_MATLABFILTER, "Created buffer with size %d", blockSize);
        engEvalString(engine, QString("apex.blockSize=%1").arg(blockSize).toLatin1());
    }

    return true;
}


bool MatlabFilter::isValidParameter (const QString &type, int channel) const
{
    if (type == "matlabpath" && channel == -1)
        return true;
    if (type == "preparefunction" && channel == -1)
        return true;
    if (type == "processfunction" && channel == -1)
        return true;
    if (type == "gain" && channel == -1)
        return true;

    return true;            // we store all other parameters

}

bool MatlabFilter::setParameter (const QString &type, int channel,
        const QString &value)
{
    if (type=="matlabpath" && channel==-1) {
        matlabPath=value;
        return true;
    }
    if (type=="preparefunction" && channel==-1) {
        preparefunction=value;
        return true;
    }
    if (type=="processfunction" && channel==-1) {
        processfunction=value;
        return true;
    }

    // just store parameter, will be sent to Matlab in prepare()
    otherparams[type]=value;
    return false;
}

void MatlabFilter::process (double * const *data)
{
    // send buffer to matlab
    memcpy(mxGetPr(buffer), data[0], blockSize*sizeof(double));
    engPutVariable(engine, "buffer", buffer);
    int r = engEvalString(engine, QString("buffer=%1(buffer)").
            arg(processfunction).toLatin1() );
    if (r!=0)
        qCDebug(APEX_MATLABFILTER, "could not evaluate matlab command");

    mxArray* result = engGetVariable(engine,"buffer");
    memcpy(data[0], mxGetPr(result),blockSize*sizeof(double));
    mxDestroyArray(result);
}

// MatlabFilterCreator =========================================================

QStringList MatlabFilterCreator::availablePlugins() const
{
    return QStringList() << "matlabfilter";
}

PluginFilterInterface *MatlabFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    Q_UNUSED (sampleRate);

    if (name == "matlabfilter")
        return new MatlabFilter (channels, size);

    return NULL;
}

#include "matlabfilter.moc"
