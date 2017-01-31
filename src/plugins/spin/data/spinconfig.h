/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef SPINCONFIG_H
#define SPINCONFIG_H

#include "spinglobal.h"

//#include <QtGlobal>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QMap>
#include <QCoreApplication>

#include "spindataenums.h"
#include "spindefaultsettings.h"

//from libdata
#include "fileprefix.h"

namespace spin
{
namespace data
{

struct SPIN_EXPORT Speechtoken
{
    QString id;
    QUrl uri;
    QString text;
};

/**
 * List of speech tokens, part of a speechmaterial
 */
struct List : QVector<Speechtoken>
{
    QString id; //FIXME change to unsigned

    void addToken(QString id, QString uri, QString text);
};

struct SPIN_EXPORT Speaker
{
    unsigned channel;
    unsigned angle;
};

struct Category : QMap<QString, List>
{
    Category() : hasRms(false){}

    double rms;
    bool hasRms;
};

struct CategoryMap : QMap<QString, Category>
{
    void addList(List list, QString category = "");
    void setRms(QString category, double rms);
    const QList<QString> categories() const;
    const QList<List> lists(QString category = "") const;
    bool hasCategories() const;
    bool hasRms(QString category) const;

    /**
     * @return The rms value of the given category. If the category does not
     *   have an rms, the return value is unspecified (probably 0).
     */
    double rms(QString category) const;
};

struct SPIN_EXPORT Speechmaterial
{
    QString id;
    double rms;
    uint valuesForMean;
    CategoryMap categoryMap;

    const QList<QString> categories() const;
    const QList<List> lists(QString category = "") const;
    bool hasCategories() const;
    bool hasRms(QString category) const;
    double rmsOfCategory(QString category) const;
};

struct Noise
{
    QString id;
    QString name;
    QString description;
    QUrl uri;
    double rms;
    QString speechmaterial; //empty if none
    QString category; //the category of the speechmaterial (empty if none)
};

struct CustomScreen
{
    QString id;
    QString screen;
};

/**
 * This class contains data parsed from the spin.xml file
 * The SpinConfigFileParser will return an instance of this class.
 *
 * @author Job Noorman
 */
class SPIN_EXPORT SpinConfig
{
    Q_DECLARE_TR_FUNCTIONS(SpinConfig);
public:

    SpinConfig();

    ~SpinConfig(){}

    //getters
    const apex::data::FilePrefix& uri_prefix() const;
    const QVector<Speaker>& speaker_setup() const;
    const QMap<QString, Noise>& noises() const;
    const QMap<QString, Speechmaterial>& speechmaterials() const;

    const Speechmaterial& speechmaterial(QString id) const;
    const Noise& noise(QString id) const;
    QVector<Noise> noisesBySpeechmaterial(
        QString speechmaterial,
        QString category = "") const;
    const List list(QString id, QString speechmaterial,
                        QString category = "") const;

    /**
        * Returns a string holding the entire xml node that represents the
        * screen that is used when the subject sits before the monitor.
        */
    const QString& subjectScreen() const;

    /**
        * Returns a string holding the entire xml node that represents the
        * screen that is used when experimenter sits before the monitor.
        */
    const QString& experimenterScreen() const;
    QStringList customScreensDescriptions() const;
    QString customScreen(const QString& description) const;
    QString customScreenId(const QString& description) const;
    const QString& textId() const;
    const QString& gainId() const;

    /**
      * All signals will be scaled by APEX to internalRMS (dBFS)
      */
    double internalRms() const;

    /**
      * Returns the sound pressure level corresponding to internalRMS.
      * If a sound had an RMS of internalRMS, the resulting SPL after
      * calibration will be defaultCalibration
      */
    double defaultCalibration() const;

    double noiseRms(QString noiseId) const;
    double speechRms(QString speechId, QString speechCategory = QString()) const;
    uint angleOfChannel(uint channel) const;
    uint channelOfAngle(uint angle) const;
    QList<uint> channelList(SpeakerType which) const;

    unsigned int soundcardBuffersize() const;
    unsigned int soundcardBlocksize() const;
    QString soundcardDriver() const;


    //setters
    void setPrefix(const apex::data::FilePrefix& prefix);
    void addSpeaker(unsigned channel, unsigned angle);
    void addNoise(QString id, QString uri, double rms, QString name = "",
                    QString description = "", QString speechmaterial = "",
                    QString speechcategory = "");
    void addSpeechmaterial(QString id, double rms, CategoryMap categoryMap,
                    uint valuesForMean = defaults::NB_RESPONSES_THAT_COUNT);
    void setSubjectScreen(const QString& screen);
    void setExperimenterScreen(const QString& screen);
    void addCustomScreen(const QString& description, const QString& id,
                            const QString& screen);
    void setTextId(const QString& id);
    void setGainId(const QString& id);
    void setInternalRms(double rms);
    void setDefaultCalibration(double d);

    void setSoundcardBuffersize(unsigned int v);
    void setSoundcardBlocksize(unsigned int v);
    void setSoundcardDriver(QString v);

    //tools
    void showContent();

private:

    //TODO make private data structure
    //<uri_prefix>
    apex::data::FilePrefix prefix;
    //<speaker_setup>
    QVector<Speaker> speakers;
    //<noises>
    QMap<QString, Noise> noisemats;
    //<speechmaterials>
    QMap<QString, Speechmaterial> speechmats;
    //<subject_screen>
    QString subjScreen;
    //<exprimenter_screen>
    QString expScreen;
    //<custom_screens>
    QMap<QString, CustomScreen> custScreens;
    //<id_setup>
    QString txtId;
    QString snrId;
    //<general_setup>
    double intRms;
    double defaultCalib;
    unsigned int m_soundcardBuffersize;
    unsigned int m_soundcardBlocksize;
    QString m_soundcardDriver;
};

} // ns data
} //ns spin

#endif
