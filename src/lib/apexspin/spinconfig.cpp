/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apextools/exceptions.h"

#include "spinconfig.h"
#include "spinusersettings.h"

#include <QMessageBox>

using namespace spin::data;

// List ========================================================================

void List::addToken(QString id, QString file, QString text)
{
    Speechtoken token;
    token.id = id;
    token.file = file;
    token.text = text;

    append(token);
}

// CategoryMap =================================================================

void CategoryMap::addList(List list, QString category)
{
    Q_ASSERT(list.id != QString::Null());

    Category listMap = take(category);
    listMap.insert(list.id, list);
    insert(category, listMap);

    // we've just inserted a category so size cannot be 0
    Q_ASSERT(size() > 0);
}

void CategoryMap::setRms(QString category, double rms)
{
    Category cat = take(category);

    if (cat.isEmpty())
        return;

    cat.rms = rms;
    cat.hasRms = true;

    insert(category, cat);

    Q_ASSERT(hasRms(category));
}

const QList<QString> CategoryMap::categories() const
{
    return keys();
}

const QList<List> CategoryMap::lists(QString category) const
{
    QMap<QString, Category>::const_iterator it = find(category);

    if (it == end()) // no lists for the given category
        return QList<List>();

    return it.value().values();
}

bool CategoryMap::hasCategories() const
{
    if (size() == 0)
        return false;

    return !(size() == 1 && keys().at(0) == "");
}

bool CategoryMap::hasRms(QString category) const
{
    if (!hasCategories())
        return false;

    return value(category).hasRms;
}

double CategoryMap::rms(QString category) const
{
    return value(category).rms;
}

// Speechmaterial ==============================================================

const QList<QString> Speechmaterial::categories() const
{
    return categoryMap.categories();
}

const QList<List> Speechmaterial::lists(QString category) const
{
    return categoryMap.lists(category);
}

bool Speechmaterial::hasCategories() const
{
    return categoryMap.hasCategories();
}

bool Speechmaterial::hasRms(QString category) const
{
    return categoryMap.hasRms(category);
}

double Speechmaterial::rmsOfCategory(QString category) const
{
    if (!categoryMap.hasRms(category))
        return rms;

    return categoryMap.rms(category);
}

// SpinConfig ==================================================================

SpinConfig::SpinConfig() : m_soundcardBuffersize(0), m_soundcardBlocksize(0)
{
}

const apex::data::FilePrefix &SpinConfig::prefix() const
{
    return filePrefix;
}

const QVector<Speaker> &SpinConfig::speaker_setup() const
{
    return speakers;
}

const QMap<QString, Noise> &SpinConfig::noises() const
{
    return noisemats;
}

const Noise &SpinConfig::noise(QString id) const
{
    if (!noisemats.contains(id))
        throw ApexStringException(tr("Invalid noise id: %1").arg(id));

    return *noisemats.find(id);
}

QVector<Noise> SpinConfig::noisesBySpeechmaterial(QString speechmaterial,
                                                  QString category) const
{
    QVector<Noise> noises;
    QList<Noise> noiseList = noisemats.values();
    QList<Noise>::const_iterator it;

    // check if we need to look for the category
    bool checkCat = !category.isEmpty();

    for (it = noiseList.begin(); it != noiseList.end(); it++) {
        if (checkCat) {
            QString categoryId = it->category;

            if (categoryId != category && !categoryId.isEmpty())
                continue;
        }

        QString speechmatId = it->speechmaterial;

        if (speechmatId == speechmaterial || speechmatId.isEmpty())
            noises.append(*it);
    }

    return noises;
}

const QMap<QString, Speechmaterial> &SpinConfig::speechmaterials() const
{
    return speechmats;
}

const Speechmaterial &SpinConfig::speechmaterial(QString id) const
{
    QMap<QString, Speechmaterial>::const_iterator it = speechmats.find(id);
    if (it == speechmats.end())
        throw ApexStringException(tr("Invalid speech material: %1").arg(id));
    return *it;
}

const List SpinConfig::list(QString id, QString speechmaterial,
                            QString category) const
{
    QList<List> lists = this->speechmaterial(speechmaterial).lists(category);

    Q_FOREACH (List list, lists) {
        if (list.id == id)
            return list;
    }

    qFatal("Invalid list in speech material");
    return List();
}

const QString &SpinConfig::subjectScreen() const
{
    return subjScreen;
}

const QString &SpinConfig::experimenterScreenQuiet() const
{
    return expScreenQuiet;
}

const QString &SpinConfig::experimenterScreenNoise() const
{
    return expScreenNoise;
}

QStringList SpinConfig::customScreensDescriptions() const
{
    return custScreens.keys();
}

QString SpinConfig::customScreen(const QString &description) const
{
    return custScreens[description].screen;
}

QString SpinConfig::customScreenId(const QString &description) const
{
    return custScreens[description].id;
}

const QString &SpinConfig::getSpeechtokenTextId() const
{
    return speechtokenTextId;
}

const QString &SpinConfig::getSpeechtokenPart1Id() const
{
    return speechtokenPart1Id;
}

const QString &SpinConfig::getNSpeechtokenParts() const
{
    return nSpeechtokenParts;
}

const QString &SpinConfig::getSpeechtokenPart2Id() const
{
    return speechtokenPart2Id;
}

const QString &SpinConfig::getSpeechtokenPart3Id() const
{
    return speechtokenPart3Id;
}

const QString &SpinConfig::getSpeechtokenPart4Id() const
{
    return speechtokenPart4Id;
}

const QString &SpinConfig::getSpeechtokenPart5Id() const
{
    return speechtokenPart5Id;
}

const QString &SpinConfig::getSpeechtokenPart6Id() const
{
    return speechtokenPart6Id;
}

const QString &SpinConfig::getSpeechtokenPart7Id() const
{
    return speechtokenPart7Id;
}

const QString &SpinConfig::getSpeechtokenPart8Id() const
{
    return speechtokenPart8Id;
}

const QString &SpinConfig::getSpeechtokenPart9Id() const
{
    return speechtokenPart9Id;
}

const QString &SpinConfig::getGainId() const
{
    return snrId;
}

double SpinConfig::internalRms() const
{
    return intRms;
}

double SpinConfig::defaultCalibration() const
{
    return defaultCalib;
}

double SpinConfig::noiseRms(QString noiseId) const
{
    return noise(noiseId).rms;
}

double SpinConfig::speechRms(QString speechId, QString speechCategory) const
{
    return speechmaterial(speechId).rmsOfCategory(speechCategory);
}

uint SpinConfig::angleOfChannel(uint channel) const
{
    Q_FOREACH (Speaker speaker, speakers) {
        if (speaker.channel == channel)
            return speaker.angle;
    }

    qFatal("no speaker found on channel %i", channel);
    return 0;
}

uint SpinConfig::channelOfAngle(uint angle) const
{
    Q_FOREACH (Speaker speaker, speakers) {
        if (speaker.angle == angle)
            return speaker.channel;
    }

    qFatal("no speaker found on angle %i", angle);
    return 0;
}

QList<uint> SpinConfig::channelList(SpeakerType which) const
{
    QList<uint> channels;

    if (which == FREE_FIELD) {
        Q_FOREACH (Speaker speaker, speakers)
            channels.append(speaker.channel);
    } else {
        Q_ASSERT(which == HEADPHONE);
        channels.append(Headphone::LEFT);
        channels.append(Headphone::RIGHT);
    }

    return channels;
}

void SpinConfig::setPrefix(const apex::data::FilePrefix &prefix)
{
    this->filePrefix = prefix;
}

void SpinConfig::addSpeaker(unsigned channel, unsigned angle)
{
    Speaker spkr;
    spkr.channel = channel;
    spkr.angle = angle;

    speakers.append(spkr);
}

void SpinConfig::addNoise(QString id, QString file, double rms, QString name,
                          QString description, QString speechmaterial,
                          QString speechcategory)
{
    Q_ASSERT(!noisemats.contains(id));

    Noise noise;
    noise.id = id;
    noise.file = file;
    noise.rms = rms;
    noise.name = name;
    noise.description = description;
    noise.speechmaterial = speechmaterial;
    noise.category = speechcategory;

    noisemats.insert(id, noise);
}

void SpinConfig::addSpeechmaterial(QString id, double rms,
                                   CategoryMap categoryMap, uint valuesForMean)
{
    Speechmaterial speechmat;
    speechmat.id = id;
    speechmat.rms = rms;
    speechmat.categoryMap = categoryMap;
    speechmat.valuesForMean = valuesForMean;

    speechmats.insert(id, speechmat);
}

void SpinConfig::setSubjectScreen(const QString &screen)
{
    subjScreen = screen;
}

void SpinConfig::setExperimenterScreenQuiet(const QString &screen)
{
    expScreenQuiet = screen;
}

void SpinConfig::setExperimenterScreenNoise(const QString &screen)
{
    expScreenNoise = screen;
}

void SpinConfig::addCustomScreen(const QString &description, const QString &id,
                                 const QString &screen)
{
    CustomScreen custScreen;
    custScreen.id = id;
    custScreen.screen = screen;
    custScreens[description] = custScreen;
}

void SpinConfig::setSpeechtokenTextId(const QString &id)
{
    speechtokenTextId = id;
}

void SpinConfig::setNSpeechtokenParts(const QString &id)
{
    nSpeechtokenParts = id;
}

void SpinConfig::setSpeechtokenPart1Id(const QString &id)
{
    speechtokenPart1Id = id;
}

void SpinConfig::setSpeechtokenPart2Id(const QString &id)
{
    speechtokenPart2Id = id;
}

void SpinConfig::setSpeechtokenPart3Id(const QString &id)
{
    speechtokenPart3Id = id;
}

void SpinConfig::setSpeechtokenPart4Id(const QString &id)
{
    speechtokenPart4Id = id;
}

void SpinConfig::setSpeechtokenPart5Id(const QString &id)
{
    speechtokenPart5Id = id;
}

void SpinConfig::setSpeechtokenPart6Id(const QString &id)
{
    speechtokenPart6Id = id;
}

void SpinConfig::setSpeechtokenPart7Id(const QString &id)
{
    speechtokenPart7Id = id;
}

void SpinConfig::setSpeechtokenPart8Id(const QString &id)
{
    speechtokenPart8Id = id;
}

void SpinConfig::setSpeechtokenPart9Id(const QString &id)
{
    speechtokenPart9Id = id;
}

void SpinConfig::setGainId(const QString &id)
{
    snrId = id;
}

void SpinConfig::setDefaultCalibration(double d)
{
    defaultCalib = d;
}

void SpinConfig::setInternalRms(double rms)
{
    intRms = rms;
}

void SpinConfig::setSoundcardBuffersize(unsigned int v)
{
    m_soundcardBuffersize = v;
}

void SpinConfig::setSoundcardBlocksize(unsigned int v)
{
    m_soundcardBlocksize = v;
}

void SpinConfig::setSoundcardDriver(QString v)
{
    m_soundcardDriver = v;
}

unsigned int SpinConfig::soundcardBuffersize() const
{
    return m_soundcardBuffersize;
}

unsigned int SpinConfig::soundcardBlocksize() const
{
    return m_soundcardBlocksize;
}

QString SpinConfig::soundcardDriver() const
{
    return m_soundcardDriver;
}

void SpinConfig::showContent()
{
    // QMessageBox box;

    QString content = "Content of SpinConfig:\n";
    content += QString("\nPrefix: %1\n").arg(filePrefix.value());
    content += "\nSpeakers:\n";

    QVector<Speaker>::const_iterator itSpkr;
    for (itSpkr = speakers.begin(); itSpkr != speakers.end(); itSpkr++) {
        content += QString("speaker on channel %1, with angle %2\n")
                       .arg(itSpkr->channel)
                       .arg(itSpkr->angle);
    }

    content += "\nNoises:\n";

    QMap<QString, Noise>::const_iterator itNoise;
    for (itNoise = noisemats.begin(); itNoise != noisemats.end(); itNoise++) {
        content += QString("noise %1:\n").arg(itNoise->id);
        content += QString("description: %1\n").arg(itNoise->description);
        content += QString("name: %1\n").arg(itNoise->name);
        content += QString("file: %1\n").arg(itNoise->file);
        content += QString("rms: %1\n").arg(itNoise->rms);
        content += QString("speechmaterial: %1\n").arg(itNoise->speechmaterial);
    }

    content += "\nSpeechmaterials:\n";

    QMap<QString, Speechmaterial>::const_iterator itSpeech;
    for (itSpeech = speechmats.begin(); itSpeech != speechmats.end();
         itSpeech++) {
        content += QString("speechmaterial %1:\n").arg(itSpeech->id);
        content += QString("rms: %1\n").arg(itSpeech->rms);

        QList<QString> categories = itSpeech->categoryMap.categories();

        QList<QString>::const_iterator itCats;
        for (itCats = categories.begin(); itCats != categories.end();
             itCats++) {
            QList<List> lists = itSpeech->categoryMap.lists(*itCats);

            QList<List>::const_iterator itLists;
            for (itLists = lists.begin(); itLists != lists.end(); itLists++) {
                content += QString("list %1:\n").arg(itLists->id);
                content += QString("category: %1\n").arg(*itCats);

                List::const_iterator itTokens;
                for (itTokens = itLists->begin(); itTokens != itLists->end();
                     itTokens++) {
                    content += QString("speechtoken %1:\n").arg(itTokens->id);
                    content += QString("file: %1\n").arg(itTokens->file);
                    content += QString("text: %1\n").arg(itTokens->text);
                }
            }
        }
    }

    // box.setText( content );
    // box.exec();
    qCDebug(APEX_RS, "%s", qPrintable(content));
}
