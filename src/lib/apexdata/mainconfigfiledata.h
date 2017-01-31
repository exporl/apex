#ifndef _EXPORL_SRC_LIB_APEXDATA_MAINCONFIGFILEDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_MAINCONFIGFILEDATA_H_

#include "apextools/global.h"

#include <QMap>
#include <QString>

namespace apex {
namespace data {


class APEXDATA_EXPORT MainConfigFileData
{
public:
    typedef QMap<QString, QMap<QString,QString> > SoundCardNames;

    MainConfigFileData();

    void setPerlPath(const QString& p);
    void setPluginScriptLibrary(const QString& p);
    //void setSchemasPath(const QString& p);
    void addSoundDriver( const QString& id, const QString& driver, const QString& name);
    void addPrefix( const QString& id, const QString& path);

    const QString& perlPath() const;
    const QString pluginScriptLibrary() const;
    const SoundCardNames& soundCardDrivers() const;
    const QString soundCardName(const QString& id, const QString& driver) const;

    const QString experimentSchema() const;

    const QString prefix(const QString& p_id) const;

private:

    //QString m_schemas_path;
    QString m_experiment_schema;
    QString m_perl_path;
    QString m_scripts_path;
    QString m_plugins_path;
    QString m_plugin_script_library;
    SoundCardNames m_SndDrivers;
    QMap<QString,QString> m_prefixes;
};

}
}


#endif // _EXPORL_SRC_LIB_APEXDATA_MAINCONFIGFILEDATA_H_

