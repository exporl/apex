#include "bertha/blockinterface.h"

#include <math.h>

#include <QStringList>
#include <QVariant>

class MultiplierCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "multiplier")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(multiplier, MultiplierCreator)
Q_PLUGIN_METADATA(multiplier)
Q_PLUGIN_METADATA(MultiplierCreator)

class MultiplierPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
public:
    Q_INVOKABLE MultiplierPlugin(unsigned blockSize, unsigned sampleRate,
                                 int inputPorts, int outputPorts);
    virtual ~MultiplierPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    QVector<PortDataType> scratch;
};

// MultiplierPlugin ============================================================

MultiplierPlugin::MultiplierPlugin(unsigned blockSize, unsigned sampleRate,
                                   int inputPorts, int outputPorts)
    : blockSize(blockSize)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 2;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(inputPorts); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
    }
    for (unsigned i = 0; i < unsigned(outputPorts); ++i) {
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }

    scratch.resize(blockSize);
}

MultiplierPlugin::~MultiplierPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString MultiplierPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    return QString();
}

void MultiplierPlugin::processOne(PortDataType *input)
{
    if (input) {
        for (unsigned i = 0; i < blockSize; ++i)
            scratch[i] *= input[i];
    }
}

void MultiplierPlugin::process()
{
    scratch.fill(1);
    for (int i = 0; i < inPorts.size(); ++i)
        processOne(inPorts[i]->inputData.toStrongRef().data());
    for (int i = 0; i < outPorts.size(); ++i)
        memcpy(outPorts[i]->outputData.data(), scratch.data(),
               scratch.size() * sizeof(PortDataType));
}

QList<BlockInputPort *> MultiplierPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> MultiplierPlugin::outputPorts()
{
    return outPorts;
}

void MultiplierPlugin::release()
{
}

// MultiplierCreator ===========================================================

QList<BlockDescription> MultiplierCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &MultiplierPlugin::staticMetaObject;
}

#include "multiplierplugin.moc"
