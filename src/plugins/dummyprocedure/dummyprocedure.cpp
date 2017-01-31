#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/procedureinterface.h"

#include <QStringList>
#include <QVariant>

using namespace apex;

class DummyProcedure : public ProcedureInterface
{
    public:

        DummyProcedure ( ProcedureApi* api, const data::ProcedureData* data);

        data::Trial setupNextTrial();
        double progress() const;
        ResultHighlight processResult(const ScreenResult *screenResult);
        QString firstScreen();

        QString resultXml() const;
        QString finalResultXml() const;

};


class DummyProcedureParser:
    public QObject,
    public ProcedureParserInterface
{
    Q_OBJECT
public:
    virtual data::ProcedureData *parse(xercesc::DOMElement *base);
};

class DummyProcedureData : public data::ProcedureData
{
    DummyProcedureData::Type type() const
    {
        return ConstantType; //A dummy shouldn't use this
    }


    QString name() const
    {
        return QLatin1String("apex:dummyProcedure"); //A dummy shouldn't use this
    }
};


class DummyProcedureCreator:
    public QObject,
    public ProcedureCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(apex::ProcedureCreatorInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.dummyprocedure")
#endif
    QStringList availableProcedurePlugins() const;

    ProcedureInterface* createProcedure(const QString& name,
                                        ProcedureApi* api,
                                        const data::ProcedureData* config);

    ProcedureParserInterface* createProcedureParser(const QString& name);

};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( dummyprocedure, DummyProcedureCreator)
#endif

/////////////// DummyProcedure

DummyProcedure::DummyProcedure(ProcedureApi* api, const data::ProcedureData* data)  :
    ProcedureInterface(api, data)
{
    Q_UNUSED(data);
}

data::Trial DummyProcedure::setupNextTrial()
{
    data::Trial trial;

    trial.setId("dummytrial");
    trial.addScreen("screen1", 1, 0);
    trial.addStimulus("stimulus", QMap<QString,QVariant>(), QString(), 0);
    trial.addScreen("screen2", 1, 0);
    trial.addStimulus("stimulus1", QMap<QString,QVariant>(), QString(), 0);
    trial.addStimulus("stimulus2", QMap<QString,QVariant>(), QString(), 0);

    return trial;
}

QString DummyProcedure::firstScreen()
{
    return "screen1";
}

double DummyProcedure::progress() const
{
    return 50;
}

ResultHighlight DummyProcedure::processResult(const ScreenResult *screenResult)
{
    Q_UNUSED( screenResult);
    return ResultHighlight();
}


QString DummyProcedure::resultXml() const
{
    return QString("<procedure/>");
}

QString DummyProcedure::finalResultXml() const
{
    return QString();
}




/////////////// DummyProcedureParser

data::ProcedureData *DummyProcedureParser::parse(xercesc::DOMElement *base)
{
    Q_UNUSED ( base );
    return new DummyProcedureData();
}


////////////// DummyProcedureCreator
QStringList DummyProcedureCreator::availableProcedurePlugins() const
{
     return QStringList() << QLatin1String("dummyprocedure");
}

ProcedureInterface *DummyProcedureCreator::createProcedure
        (const QString &name,
         ProcedureApi *api,
         const data::ProcedureData *config)
{
    if (name == QLatin1String("dummyprocedure")) {
        return new DummyProcedure(api, config);
    } else {
         qFatal("Invalid plugin name");
    }

    return 0;           // keep compiler happy
}

ProcedureParserInterface *DummyProcedureCreator::createProcedureParser(const QString &name)
{
    if (name == QLatin1String("dummyprocedure")) {
        return new DummyProcedureParser();
    }
    qFatal("Invalid procedure parser plugin name");
    return 0;           // keep compiler happy

}




#include "dummyprocedure.moc"
