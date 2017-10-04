#include "mainconfigfiledata.h"

#include <QString>

namespace apex
{
namespace data
{

MainConfigFileData::MainConfigFileData()
    : m_plugin_script_library("pluginscriptlibrary.js"), bertha(true)
{
}

void MainConfigFileData::setPluginScriptLibrary(const QString &p)
{
    m_plugin_script_library = p;
}

void MainConfigFileData::addSoundDriver(const QString &id,
                                        const QString &driver,
                                        const QString &name)
{
    if (!m_SndDrivers.contains(id)) {
        m_SndDrivers.insert(id, QMap<QString, QString>());
    }
    m_SndDrivers[id].insert(driver, name);
}

void MainConfigFileData::addCohDriverName(const QString &id,
                                          const QString &name)
{
    cohDriverNames[id.toLower()] = name;
}

void MainConfigFileData::addPrefix(const QString &id, const QString &path)
{
    m_prefixes.insert(id, path);
}

const MainConfigFileData::SoundCardNames &
MainConfigFileData::soundCardDrivers() const
{
    return m_SndDrivers;
}

const QString MainConfigFileData::soundCardName(const QString &id,
                                                const QString &driver) const
{
    if (!m_SndDrivers.contains(id))
        return QString();
    if (!m_SndDrivers[id].contains(driver))
        return QString();

    return m_SndDrivers[id][driver];
}

QString MainConfigFileData::cohDriverName(const QString &id) const
{
    return cohDriverNames.value(id.toLower(), id);
}

/**
 * Return prefix according to given id, if no prefix is found, an empty string
 * is returned
 * @param p_id
 * @return
 */
const QString MainConfigFileData::prefix(const QString &p_id) const
{
    if (!m_prefixes.contains(p_id)) {
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

void MainConfigFileData::setBertha(bool value)
{
    bertha = value;
}

bool MainConfigFileData::useBertha() const
{
    return bertha;
}

void MainConfigFileData::addInteractiveConstraint(const QString &expression,
                                                  const QString &constraint)
{
    interactiveConstraints.insert(expression, constraint);
}

QString
MainConfigFileData::interactiveConstraint(const QString &expression) const
{
    return interactiveConstraints.value(expression, QString());
}
}
}
