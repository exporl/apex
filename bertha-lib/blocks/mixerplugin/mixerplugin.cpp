#include "bertha/blockinterface.h"

#include <math.h>

#include <QStringList>
#include <QVariant>
#include <QVector>

class MixerCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "mixer")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(mixer, MixerCreator)
Q_PLUGIN_METADATA(mixer)
Q_PLUGIN_METADATA(MixerCreator)

class MixerPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(QVector<QVector<double>> matrix WRITE setSwitchMatrix READ
                   getSwitchMatrix)
    Q_PROPERTY(
        bool logarithmicGain WRITE setLogarithmicGain READ getLogarithmicGain)
    Q_CLASSINFO("matrix-description",
                Q_TR_NOOP("gains[m][n]: n = inputports, m = outputports"))
    Q_CLASSINFO("matrix-unit", "linear")
    Q_CLASSINFO(
        "logarithmicGain-description",
        Q_TR_NOOP("if true, matrix elements are in dB, otherwise linear"))
    Q_CLASSINFO("logarithmicGain-unit", "linear")
public:
    Q_INVOKABLE MixerPlugin(unsigned blockSize, unsigned sampleRate,
                            int inputPorts, int outputPorts);
    virtual ~MixerPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output, double gain);

    QVector<QVector<double>> getSwitchMatrix();
    void setSwitchMatrix(const QVector<QVector<double>> &values);

    void setLogarithmicGain(bool value);
    bool getLogarithmicGain() const;

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    QVector<QVector<double>> switchMatrix; // may be converted to linear from dB
    QVector<QVector<double>> origSwitchMatrix;
    bool logarithmicGain;
};

// MixerPlugin ================================================================

MixerPlugin::MixerPlugin(unsigned blockSize, unsigned sampleRate,
                         int inputPorts, int outputPorts)
    : blockSize(blockSize), logarithmicGain(true)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 1;

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
    for (unsigned i = 0; i < unsigned(outputPorts); ++i)
        switchMatrix.append(QVector<double>(inputPorts, 0));
}

MixerPlugin::~MixerPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString MixerPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    return QString();
}

void MixerPlugin::processOne(PortDataType *input, PortDataType *output,
                             double gain)
{
    if (gain != 0 && input) {
        for (unsigned q = 0; q < unsigned(blockSize); ++q)
            output[q] += input[q] * gain;
    }
}

void MixerPlugin::process()
{
    for (unsigned i = 0; i < unsigned(outPorts.size()); ++i) {
        memset(outPorts[i]->outputData.data(), 0,
               blockSize * sizeof(PortDataType));
        for (unsigned j = 0; j < unsigned(inPorts.size()); ++j) {
            processOne(inPorts[j]->inputData.toStrongRef().data(),
                       outPorts[i]->outputData.data(), switchMatrix[i][j]);
        }
    }
}

QList<BlockInputPort *> MixerPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> MixerPlugin::outputPorts()
{
    return outPorts;
}

void MixerPlugin::release()
{
}

QVector<QVector<double>> MixerPlugin::getSwitchMatrix()
{
    return origSwitchMatrix;
}

void MixerPlugin::setSwitchMatrix(const QVector<QVector<double>> &values)
{
    origSwitchMatrix = values;
    switchMatrix = values;
    if (logarithmicGain) {
        for (unsigned i = 0; i < unsigned(switchMatrix.size()); ++i)
            for (unsigned j = 0; j < unsigned(switchMatrix[i].size()); ++j)
                switchMatrix[i][j] = pow(10.0, switchMatrix[i][j] / 20.0);
    }

    for (unsigned i = 0; i < unsigned(switchMatrix.size()); ++i)
        if (switchMatrix[i].size() < inPorts.size())
            for (unsigned j = unsigned(switchMatrix[i].size());
                 j < unsigned(inPorts.size()); ++j)
                switchMatrix[i].append(0);
    for (unsigned i = unsigned(switchMatrix.size());
         i < unsigned(outPorts.size()); ++i)
        switchMatrix.append(QVector<double>(inPorts.size(), 0));
}

void MixerPlugin::setLogarithmicGain(bool value)
{
    logarithmicGain = value;
    setSwitchMatrix(origSwitchMatrix);
}

bool MixerPlugin::getLogarithmicGain() const
{
    return logarithmicGain;
}

// MixerCreator ===============================================================

QList<BlockDescription> MixerCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &MixerPlugin::staticMetaObject;
}

#include "mixerplugin.moc"
