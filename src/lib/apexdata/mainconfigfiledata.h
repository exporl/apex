#ifndef _APEX_SRC_LIB_APEXDATA_MAINCONFIGFILEDATA_H_
#define _APEX_SRC_LIB_APEXDATA_MAINCONFIGFILEDATA_H_

#include "apextools/global.h"

#include <QMap>
#include <QString>

namespace apex
{
namespace data
{

class APEXDATA_EXPORT MainConfigFileData
{
public:
    typedef QMap<QString, QMap<QString, QString>> SoundCardNames;

    MainConfigFileData();

    void setPluginScriptLibrary(const QString &p);
    void addSoundDriver(const QString &id, const QString &driver,
                        const QString &name);
    void addPrefix(const QString &id, const QString &path);
    void addCohDriverName(const QString &id, const QString &name);
    void addInteractiveConstraint(const QString &expression,
                                  const QString &constraint);

    const QString pluginScriptLibrary() const;
    const SoundCardNames &soundCardDrivers() const;
    const QString soundCardName(const QString &id, const QString &driver) const;
    QString cohDriverName(const QString &id) const;
    QString interactiveConstraint(const QString &expression) const;

    const QString experimentSchema() const;

    const QString prefix(const QString &p_id) const;

    void setBertha(bool value);
    bool useBertha() const;

private:
    QString m_experiment_schema;
    QString m_scripts_path;
    QString m_plugins_path;
    QString m_plugin_script_library;
    SoundCardNames m_SndDrivers;
    QMap<QString, QString> m_prefixes;
    QMap<QString, QString> cohDriverNames;
    QMap<QString, QString> interactiveConstraints;
    bool bertha;
};
}
}

#endif // _APEX_SRC_LIB_APEXDATA_MAINCONFIGFILEDATA_H_
