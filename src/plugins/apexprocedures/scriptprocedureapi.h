#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDUREAPI_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDUREAPI_H_

#include <QObject>
#include <QStringList>
#include <QVariant>

namespace apex
{

class ScriptProcedure;
class ProcedureApi;

/** API for script procedure
 * Is a proxy to ScriptProcedure, but only exposes relevant methods
 */
class ScriptProcedureApi : public QObject
{
    Q_OBJECT

public:
    ScriptProcedureApi(ProcedureApi *, ScriptProcedure *p);

public slots:
    // Functions to change something to Apex

    QStringList parameterNames(const QString stimulusID) const;

    QVariantMap parameterMap(const QString stimulusID) const;

    QVariant parameterValue(const QString &parameter_id);

    /**
     * Register a new parameter with the manager.
     */
    void registerParameter(const QString &name); // TODO more parameters?

    // Functions to get something from Apex
    /** Get list of trials for this procedure
     * will return a QList of ApexTrial objects
     */
    QObjectList trials() const;

    QStringList stimuli() const;

    QVariant fixedParameterValue(const QString stimulusID,
                                 const QString parameterID);

    QString answerElement(const QString trialid);

    // Misc functions
    /** Shows the user a messagebox with the specified messagebox
     */
    void messageBox(const QString message) const;

    void showMessage(const QString &message) const;

    void abort(const QString &message) const;

    /**
      * Returns full filepath for relative path
      */
    QString absoluteFilePath(const QString &path);

signals:
    void showMessageBox(const QString title, const QString text) const;

private:
    apex::ScriptProcedure *scriptProcedure;
    ProcedureApi *api;
};
}

#endif
