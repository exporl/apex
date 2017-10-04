/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Hans Moorkens.                                 *
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

#include "bertha/blockinterface.h"

#include <QStringList>
#include <QVariant>
#include <QVector>

class TypeTestingCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "typetesting")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(typeTesting, TypeTestingCreator)
Q_PLUGIN_METADATA(typeTesting)
Q_PLUGIN_METADATA(TypeTestingCreator)

class TypeTestingPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
public:
    enum Enum { Zero, One };

    Q_PROPERTY(int m_int WRITE setInt READ getInt)
    Q_PROPERTY(float m_float WRITE setFloat READ getFloat)
    Q_PROPERTY(double m_double WRITE setDouble READ getDouble)
    Q_PROPERTY(QStringList m_stringList WRITE setStringList READ getStringList)
    Q_PROPERTY(QVector<double> m_vector WRITE setVector READ getVector)
    Q_PROPERTY(QVector<QVector<double>> m_doubleVector WRITE setDoubleVector
                   READ getDoubleVector)
    Q_PROPERTY(bool m_bool WRITE setBool READ getBool)
    Q_PROPERTY(Enum m_enum WRITE setEnum READ getEnum)
    Q_CLASSINFO("m_int-description", Q_TR_NOOP("int"))
    Q_CLASSINFO("m_int-unit", "linear")
    Q_CLASSINFO("m_int-minimum", "-10000")
    Q_CLASSINFO("m_int-maximum", "10000")
    Q_CLASSINFO("m_float-description", Q_TR_NOOP("float"))
    Q_CLASSINFO("m_float-unit", "linear")
    Q_CLASSINFO("m_float-minimum", "-10000")
    Q_CLASSINFO("m_float-maximum", "10000")
    Q_CLASSINFO("m_double-description", Q_TR_NOOP("double"))
    Q_CLASSINFO("m_double-unit", "linear")
    Q_CLASSINFO("m_double-minimum", "-10000")
    Q_CLASSINFO("m_double-maximum", "10000")
    Q_CLASSINFO("m_stringList-description", Q_TR_NOOP("QStringList"))
    Q_CLASSINFO("m_vector-description", Q_TR_NOOP("QVector<double>"))
    Q_CLASSINFO("m_vector-unit", "linear")
    Q_CLASSINFO("m_vector-minimum", "-100")
    Q_CLASSINFO("m_vector-maximum", "100")
    Q_CLASSINFO("m_doubleVector-description",
                Q_TR_NOOP("QVector<QVector<double> >"))
    Q_CLASSINFO("m_doubleVector-unit", "linear")
    Q_CLASSINFO("m_doubleVector-minimum", "-100")
    Q_CLASSINFO("m_doubleVector-maximum", "100")
    Q_CLASSINFO("m_bool-description", Q_TR_NOOP("bool"))
    Q_CLASSINFO("m_enum-description", Q_TR_NOOP("Enum"))
    Q_ENUMS(Enum)
public:
    Q_INVOKABLE TypeTestingPlugin(unsigned blockSize, unsigned sampleRate,
                                  int inputPorts, int outputPorts);
    virtual ~TypeTestingPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    int getInt() const;
    void setInt(int value);

    float getFloat() const;
    void setFloat(float value);

    double getDouble() const;
    void setDouble(double value);

    QStringList getStringList() const;
    void setStringList(const QStringList &value);

    QVector<double> getVector() const;
    void setVector(QVector<double> value);

    QVector<QVector<double>> getDoubleVector() const;
    void setDoubleVector(QVector<QVector<double>> value);

    bool getBool() const;
    void setBool(bool value);

    Enum getEnum() const;
    void setEnum(Enum value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    int m_int;
    float m_float;
    double m_double;
    QStringList m_stringList;
    QVector<double> m_vector;
    QVector<QVector<double>> m_doubleVector;
    bool m_bool;
    Enum m_enum;
};

// TypeTestingPlugin ===========================================================

TypeTestingPlugin::TypeTestingPlugin(unsigned blockSize, unsigned sampleRate,
                                     int inputPorts, int outputPorts)
    : blockSize(blockSize),
      m_int(0),
      m_float(0),
      m_double(0),
      m_bool(true),
      m_enum(One)
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

TypeTestingPlugin::~TypeTestingPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString TypeTestingPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    return QString();
}

void TypeTestingPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else {
        for (unsigned i = 0; i < blockSize; ++i)
            output[i] = input[i];
    }
}

void TypeTestingPlugin::process()
{
    for (int i = 0; i < inPorts.size(); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> TypeTestingPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> TypeTestingPlugin::outputPorts()
{
    return outPorts;
}

void TypeTestingPlugin::release()
{
}

int TypeTestingPlugin::getInt() const
{
    return m_int;
}

void TypeTestingPlugin::setInt(int value)
{
    m_int = value;
}

float TypeTestingPlugin::getFloat() const
{
    return m_float;
}

void TypeTestingPlugin::setFloat(float value)
{
    m_float = value;
}

double TypeTestingPlugin::getDouble() const
{
    return m_double;
}

void TypeTestingPlugin::setDouble(double value)
{
    m_double = value;
}

QStringList TypeTestingPlugin::getStringList() const
{
    return m_stringList;
}

void TypeTestingPlugin::setStringList(const QStringList &value)
{
    m_stringList = value;
}

QVector<double> TypeTestingPlugin::getVector() const
{
    return m_vector;
}

void TypeTestingPlugin::setVector(QVector<double> value)
{
    m_vector = value;
}

QVector<QVector<double>> TypeTestingPlugin::getDoubleVector() const
{
    return m_doubleVector;
}

void TypeTestingPlugin::setDoubleVector(QVector<QVector<double>> value)
{
    m_doubleVector = value;
}

bool TypeTestingPlugin::getBool() const
{
    return m_bool;
}

void TypeTestingPlugin::setBool(bool value)
{
    m_bool = value;
}

TypeTestingPlugin::Enum TypeTestingPlugin::getEnum() const
{
    return m_enum;
}

void TypeTestingPlugin::setEnum(Enum value)
{
    m_enum = value;
}

// TypeTestingCreator ==========================================================

QList<BlockDescription> TypeTestingCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &TypeTestingPlugin::staticMetaObject;
}

#include "typetestingplugin.moc"
