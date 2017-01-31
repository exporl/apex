#include "apexdata/mainconfigfiledata.h"

#include "apexdata/procedure/scriptproceduredata.h"

#include "apextools/services/paths.h"

#include "scriptprocedure.h"
#include "scriptresulthighlight.h"

#include <QtScript>

using namespace apex;

Q_SCRIPT_DECLARE_QMETAOBJECT(data::Trial, QObject*)
Q_SCRIPT_DECLARE_QMETAOBJECT(data::ScriptResultHighlight, QObject*)

namespace apex
{
ScriptProcedure::ScriptProcedure(ProcedureApi* api, const data::ProcedureData *data)
    : ProcedureInterface(api, data),
      m_plugin(new ScriptProcedureWrapper),
      m_api(new ScriptProcedureApi(api, this)),
          data(dynamic_cast<const data::ScriptProcedureData*>(data))
{
    //Qt < 4.6 does not seed javascript Math.random(), do it manually
    //TODO remove if it works in 4.6
#if QT_VERSION < 0x040600
    qsrand(time(0));
#endif

    //construct script filename
    m_doDebug = this->data->debugger();

    QString newName = Paths::findReadableFile (this->data->script(),
                                               QStringList() <<  QString() << Paths::Get().GetNonBinaryPluginPath(),
                                               QStringList() << ".js");

    if(m_doDebug)
    {
        m_scriptEngineDebugger.attachTo(&m_scriptEngine);
    }

    if(newName.isEmpty())
    {
        throw ApexStringException("Cannot find procedure plugin file " +
                                  (this->data->script()));
    }

    qCDebug(APEX_RS, "Looking for %s", qPrintable (newName));

    qCDebug(APEX_RS, "Adding ScriptProcedureInterface object to QtScript");
    QScriptValue baseClass = m_scriptEngine.scriptValueFromQMetaObject<ScriptProcedureInterface>();
    m_scriptEngine.globalObject().setProperty("ScriptProcedureInterface", baseClass );

    // for consistency, add a console.log function that proxies to the internal print method
    m_scriptEngine.evaluate("console = { log: function() { print.apply(0, Array.prototype.slice.call(arguments, 0)); } }");

    // Expose Trial class
    QScriptValue trial = m_scriptEngine.scriptValueFromQMetaObject<data::Trial>();
    m_scriptEngine.globalObject().setProperty("Trial", trial);

        /*QScriptValue trialPValue = m_scriptEngine.newQObject(&trialProto);
        m_scriptEngine.setDefaultPrototype(qMetaTypeId<data::Trial*>(), trialPValue);

        data::Trial *trial= new data::Trial();

        QScriptValue trialObj = m_scriptEngine.newVariant(qVariantFromValue<data::Trial*>(trial));
        m_scriptEngine.globalObject().setProperty("Trial", trialObj);*/
    /*QScriptValue trial = m_scriptEngine.newQMetaObject(trialProto);
        m_scriptEngine.globalObject().setProperty("Trial", trial);*/


    // Expose ScriptResultHighlight class
    QScriptValue resultHighlight = m_scriptEngine.scriptValueFromQMetaObject<data::ScriptResultHighlight>();
    m_scriptEngine.globalObject().setProperty("ResultHighlight", resultHighlight);

    // create API
    QScriptValue APIObject = m_scriptEngine.newQObject(m_api.data());
    m_scriptEngine.globalObject().setProperty("api", APIObject);

    // load script, script is supposed to define scriptProcedure, which inherits from scriptProcedureInterface
    QFile file(newName);
    if(!file.open(QIODevice::ReadOnly))
    {
        throw ApexStringException("Cannot open plugin procedure script: " + newName);
    }
    QString scriptdata(file.readAll());

    // attach the main script library to the current script
    int nApiLines = 0;          // number of lines in the API, used to calculate correct error message line
    QString mainPluginLibrary( api->nonBinaryPluginPath() + api->pluginScriptLibrary() );

    if(!mainPluginLibrary.isEmpty())
    {
        QFile file(mainPluginLibrary);
        if(!file.open(QIODevice::ReadOnly))
        {
            throw ApexStringException("Cannot open main script library file: " + mainPluginLibrary);
        }

        QString append = file.readAll();
        nApiLines=append.count("\n")+1;

        scriptdata=append + "\n" + scriptdata;
        file.close();
    }

    //set proceduredata as properties of data object
    qCDebug(APEX_RS, "Setting properties");
    QScriptValue dataObject = m_scriptEngine.newObject();
    m_scriptEngine.globalObject().setProperty("params", dataObject);
    const data::ScriptProcedureData* scriptProcedureData = dynamic_cast<const data::ScriptProcedureData*>(data);
    const data::tScriptProcedureParameterList& pp = scriptProcedureData->customParameters();

    for(data::tScriptProcedureParameterList::const_iterator it=pp.begin(); it!=pp.end(); ++it)
    {
        dataObject.setProperty(it->name, QScriptValue(&m_scriptEngine, it->value));
    }

    dataObject.setProperty("choices", QScriptValue(&m_scriptEngine, (scriptProcedureData->choices()).nChoices()));
    dataObject.setProperty("presentations", QScriptValue(&m_scriptEngine, scriptProcedureData->presentations()));
    QString orderString = (scriptProcedureData->order() == data::ProcedureData::RandomOrder)?(QString("random")):(QString("sequential"));
    dataObject.setProperty("order", QScriptValue(&m_scriptEngine, orderString));

    qCDebug(APEX_RS, "Evaluating script");
    m_classname = m_scriptEngine.evaluate(scriptdata);
    file.close();

    // If the script did not return an object, call getProcedure()
    if (!m_classname.isQObject()) {
        m_classname = m_scriptEngine.evaluate("getProcedure()");
    }

    // check for errors from script execution
    if(m_scriptEngine.hasUncaughtException())
    {
        int lineNo = m_scriptEngine.uncaughtExceptionLineNumber();
        if(lineNo<nApiLines)
        {
            throw ApexStringException(QString("Error in scriptProcedure API line %1: %2").arg(lineNo).arg(m_classname.toString()));
        }
        else
        {
            throw ApexStringException(QString("Error in scriptProcedure line %1: %2").arg(lineNo-nApiLines).arg(m_classname.toString()));
        }
    }

    // give the wrapper some information
    m_plugin->setEngine(&m_scriptEngine);
    m_plugin->setClassname(&m_classname);
    if (! m_plugin->isValid()) {
        throw ApexStringException("plugin is not valid");
    }

    // Change instance name to something we know:
    m_scriptEngine.globalObject().setProperty(ScriptProcedureWrapper::INSTANCENAME, m_classname );

    //checkParameter() is not declared in ProcedureInterface
    QString parameterCheckResult = m_plugin->checkParameters();
    if(!parameterCheckResult.isEmpty())
    {
        throw ApexStringException(parameterCheckResult);
    }

}

ScriptProcedure::~ScriptProcedure()
{
}

QString ScriptProcedure::firstScreen()
{
    return m_plugin->firstScreen();
}

const data::ScriptProcedureData* ScriptProcedure::procedureData()
{
    return data;
}

double ScriptProcedure::progress() const
{
    return m_plugin->progress();
}

QString ScriptProcedure::resultXml() const
{
    return m_plugin->resultXml();
}

QString ScriptProcedure::finalResultXml() const
{
    return m_plugin->finalResultXml();
}

ResultHighlight ScriptProcedure::processResult(const ScreenResult *screenResult)
{
    return m_plugin->processResult(screenResult);
}

data::Trial ScriptProcedure::setupNextTrial()
{
    return m_plugin->setupNextTrial();
}



}   //namespace apex
