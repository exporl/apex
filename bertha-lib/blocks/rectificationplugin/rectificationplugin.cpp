#include "bertha/blockinterface.h"

#include <math.h>

#include <QStringList>
#include <QVariant>

class RectificationCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "rectification")

public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(rectification, RectificationCreator)
Q_PLUGIN_METADATA(rectification)
Q_PLUGIN_METADATA(RectificationCreator)

class RectificationPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
public:
    Q_INVOKABLE RectificationPlugin(unsigned blockSize, unsigned sampleRate,
                                    int inputPorts, int outputPorts);
    virtual ~RectificationPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
};

// RectificationPlugin =========================================================

RectificationPlugin::RectificationPlugin(unsigned blockSize,
                                         unsigned sampleRate, int inputPorts,
                                         int outputPorts)
    : blockSize(blockSize)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 1;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
}

RectificationPlugin::~RectificationPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString RectificationPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    return QString();
}

void RectificationPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else {
        for (unsigned j = 0; j < blockSize; ++j)
            output[j] = qMax(0.0f, input[j]);
    }
}

void RectificationPlugin::process()
{
    for (int i = 0; i < inPorts.size(); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> RectificationPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> RectificationPlugin::outputPorts()
{
    return outPorts;
}

void RectificationPlugin::release()
{
}

// RectificationCreator ========================================================

QList<BlockDescription> RectificationCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &RectificationPlugin::staticMetaObject;
}

#include "rectificationplugin.moc"
