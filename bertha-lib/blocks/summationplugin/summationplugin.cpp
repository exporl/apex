#include "bertha/blockinterface.h"

#include <math.h>

#include <QStringList>
#include <QVariant>

class SummationCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "Summation")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(summation, SummationCreator)
// Q_PLUGIN_METADATA(summation)
// Q_PLUGIN_METADATA(SummationCreator)

class SummationPlugin : public QObject, public BlockInterface
{

    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    // Q_PLUGIN_METADATA(IID "Summation")
public:
    // default constructor needed for plugin macro.
    // SummationPlugin() : SummationPlugin(256, 44100,2,2) {}
    Q_INVOKABLE SummationPlugin(unsigned blockSize, unsigned sampleRate,
                                int inputPorts, int outputPorts);
    virtual ~SummationPlugin();

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

// SummationPlugin =============================================================

/*SummationPlugin::SummationPlugin()
{
    //Default constructor needed for plugin  macro
    this->SummationPlugin(0,0,-1,-1); //Will this cause trouble?
}*/

SummationPlugin::SummationPlugin(unsigned blockSize, unsigned sampleRate,
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

SummationPlugin::~SummationPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString SummationPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    return QString();
}

void SummationPlugin::processOne(PortDataType *input)
{
    if (input) {
        for (unsigned i = 0; i < blockSize; ++i)
            scratch[i] += input[i];
    }
}

void SummationPlugin::process()
{
    scratch.fill(0);
    for (int i = 0; i < inPorts.size(); ++i)
        processOne(inPorts[i]->inputData.toStrongRef().data());
    for (int i = 0; i < outPorts.size(); ++i)
        memcpy(outPorts[i]->outputData.data(), scratch.data(),
               scratch.size() * sizeof(PortDataType));
}

QList<BlockInputPort *> SummationPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> SummationPlugin::outputPorts()
{
    return outPorts;
}

void SummationPlugin::release()
{
}

// SummationCreator ============================================================

QList<BlockDescription> SummationCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &SummationPlugin::staticMetaObject;
}

#include "summationplugin.moc"
