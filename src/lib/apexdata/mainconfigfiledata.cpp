#include "mainconfigfiledata.h"

#include <QString>

namespace apex {
namespace data {


MainConfigFileData::MainConfigFileData():
    m_plugin_script_library("pluginscriptlibrary.js")
{
}

void MainConfigFileData::setPerlPath(const QString &p)
{
    m_perl_path = p;
}

void MainConfigFileData::setPluginScriptLibrary(const QString &p)
{
    m_plugin_script_library = p;
}

/*void MainConfigFileData::setSchemasPath(const QString &p)
{
    m_schemas_path = p;
}*/

void MainConfigFileData::addSoundDriver(const QString &id, const QString &driver, const QString &name)
{
    if (! m_SndDrivers.contains(id)) {
        m_SndDrivers.insert(id, QMap<QString,QString>());
    }
    m_SndDrivers[id].insert(driver,name);
}

void MainConfigFileData::addPrefix(const QString &id, const QString &path)
{
    m_prefixes.insert(id, path);
}

const QString& MainConfigFileData::perlPath() const
{
    return m_perl_path;
}

const MainConfigFileData::SoundCardNames& MainConfigFileData::soundCardDrivers() const
{
    return m_SndDrivers;
}

const QString MainConfigFileData::soundCardName(const QString &id, const QString &driver) const
{
    if (! m_SndDrivers.contains(id))
        return QString();
    if (! m_SndDrivers[id].contains(driver))
        return QString();

    return m_SndDrivers[id][driver];
}


/**
 * Return prefix according to given id, if no prefix is found, an empty string is returned
 * @param p_id
 * @return
 */
const QString  MainConfigFileData::prefix( const QString & p_id ) const
{
    if (! m_prefixes.contains(p_id)) {
        return QString();
    }

    return m_prefixes[p_id];

}

const QString MainConfigFileData::pluginScriptLibrary() const
{
    return m_plugin_script_library;
}

const QString MainConfigFileData::experimentSchema() const
{
    return m_experiment_schema;
}



}
}
