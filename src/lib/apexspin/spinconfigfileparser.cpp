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

#include "apextools/apexpaths.h"
#include "apextools/version.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "spinconfig.h"
#include "spinconfigfileparser.h"

#include <QDir>
#include <QFileInfo>

using spin::data::SpinConfig;
using namespace apex;
using namespace spin::parser;

SpinConfigFileParser::SpinConfigFileParser() :
    UpgradableXmlParser(apex::ApexPaths::GetConfigFilePath(QSL("spin.xml")),
            ApexPaths::GetSpinSchemaPath(),
            QL1S(SPIN_SCHEMA_URL),
            QL1S(APEX_NAMESPACE))
{
}

SpinConfigFileParser::SpinConfigFileParser(const QString &file) :
    UpgradableXmlParser(file,
            ApexPaths::GetSpinSchemaPath(),
            QL1S(SPIN_SCHEMA_URL),
            QL1S(APEX_NAMESPACE))
{
}

SpinConfig SpinConfigFileParser::parse()
{
    loadAndUpgradeDom();

    SpinConfig config;

    for (QDomElement child = document.documentElement().firstChildElement(); !child.isNull();
            child = child.nextSiblingElement()) {
        QString tag = child.tagName();
        if (tag == "prefix")
            parsePrefix(child, &config);
        else if (tag == "speaker_setup")
            parseSpeakers(child, &config);
        else if (tag == "noises")
            parseNoises(child, &config);
        else if (tag == "subject_screen")
            parseSubjectScreen(child, &config);
        else if (tag == "experimenter_screen_quiet")
            parseExperimenterScreenQuiet(child, &config);
        else if (tag == "experimenter_screen_noise")
            parseExperimenterScreenNoise(child, &config);
        else if (tag == "custom_screens")
            parseCustomScreens(child, &config);
        else if (tag == "id_setup")
            parseIds(child, &config);
        else if (tag == "general_setup")
            parseGeneralData(child, &config);
        else if (tag == "soundcard_setup")
            parseSoundcardSetup(child, &config);
        else if (tag == "speechmaterial_files")
            parseSpeechFiles(child, &config, QFileInfo(fileName).path());
        else
            qFatal("SPIN Parser: unknown tag (%s).", qPrintable(tag));
    }

    return config;
}

/*FIXME link against libparser when ready!
This code is a duplicate from prefixparser */
void SpinConfigFileParser::parsePrefix(const QDomElement &prefixNode, SpinConfig* into)
{
    Q_ASSERT(prefixNode.tagName() == "prefix");

    QString attrib(prefixNode.attribute(QSL("source")));
    QString value(prefixNode.text());

    apex::data::FilePrefix prefix;
    prefix.setValue( value.trimmed() );
    if (attrib.isEmpty() || attrib == "inline")
        prefix.setType( apex::data::FilePrefix::PREFIX_INLINE);
    else if (attrib == "apexconfig")
        prefix.setType(apex::data::FilePrefix::PREFIX_MAINCONFIG );
    else
        qFatal("invalid attribute value");

    into->setPrefix(prefix);
}

void SpinConfigFileParser::parseSpeakers(const QDomElement &speakers, SpinConfig* into)
{
    Q_ASSERT(speakers.tagName() == "speaker_setup");

    for (QDomElement speaker = speakers.firstChildElement(); !speaker.isNull();
            speaker = speaker.nextSiblingElement()) {
        Q_ASSERT(speaker.tagName() == "speaker");

        unsigned angle = speaker.firstChildElement(QSL("angle")).text().toUInt();
        unsigned channel = speaker.firstChildElement(QSL("channel")).text().toUInt();

        into->addSpeaker(channel, angle);
    }
}

void SpinConfigFileParser::parseNoises(const QDomElement &noises, SpinConfig* into)
{
    Q_ASSERT(noises.tagName() == "noises");

    for (QDomElement noise = noises.firstChildElement(); !noise.isNull();
            noise = noise.nextSiblingElement()) {
        Q_ASSERT(noise.tagName() == "noise");

        QString id = noise.attribute(QSL("id"));
        QString file = noise.firstChildElement(QSL("file")).text();
        double rms = noise.firstChildElement(QSL("rms")).text().toDouble();
        QString name = noise.firstChildElement(QSL("name")).text();
        QString description = noise.firstChildElement(QSL("description")).text();

        QDomElement speechNode = noise.firstChildElement(QSL("speechmaterial"));
        QString speechmat;
        QString speechcat;

        if (!speechNode.isNull()) {
            speechmat = speechNode.text();
            speechcat = speechNode.attribute(QSL("category"));
        }

        into->addNoise(id, file, rms, name, description, speechmat, speechcat);
    }
}

void SpinConfigFileParser::parseSpeech(const QDomElement &speechmat, SpinConfig* into)
{
    Q_ASSERT(speechmat.tagName() == "speechmaterial");

    //get all childs of speechmat. there must be an <rms> child and either
    //<list> or <category> childs.
    spin::data::CategoryMap categoryMap;

    double rms = speechmat.firstChildElement(QSL("rms")).text().toDouble();
    QString vfm = speechmat.firstChildElement(QSL("values_for_mean")).text();

    bool hasCategories = false;
    QDomNodeList catList = speechmat.elementsByTagName(QSL("category"));

    for (int i = 0; i < catList.length(); i++) {
        hasCategories = true;
        QDomElement catElem = catList.item(i).toElement();
        QString category = catElem.attribute(QSL("id"));
        QDomNodeList listElems = catElem.elementsByTagName(QSL("list"));
        addLists(listElems, &categoryMap, category);
        QString rmsString = catElem.firstChildElement(QSL("rms")).text();
        if (!rmsString.isEmpty())
            categoryMap.setRms(category, rmsString.toDouble());
    }

    if (!hasCategories) {
        QDomNodeList listList = speechmat.elementsByTagName(QSL("list"));
        addLists(listList, &categoryMap);   //no category
        Q_ASSERT(!categoryMap.hasCategories());
    } else {
        Q_ASSERT(categoryMap.hasCategories());
    }

    QString id = speechmat.attribute(QSL("id"));

    if (vfm.isEmpty())
        into->addSpeechmaterial(id, rms, categoryMap);
    else
        into->addSpeechmaterial(id, rms, categoryMap, vfm.toUInt());
}

QStringList SpinConfigFileParser::expandWildcards(const QString &directory, const QString &fileIdentifier)
{
    QStringList pathParts = fileIdentifier.split(QDir::separator());

    QStringList files;
    Q_FOREACH (const QString &pathPart, pathParts) {
        QStringList filters;
        filters << pathPart;

        if (files.isEmpty()) {
            QStringList entries = QDir(directory).entryList(filters);
            if(entries.isEmpty()) {
                files.clear();
                break;
            }

            files << entries;
        } else {
            QStringList newFilePart;
            bool breaked = false;
            Q_FOREACH (const QString &filePart, files) {
                QStringList entries = QDir(directory + "/" + filePart).entryList(filters);
                if(entries.isEmpty()) {
                    files.clear();
                    breaked = true;
                    break;
                }

                Q_FOREACH (const QString &entry, entries) {
                    newFilePart << filePart + "/" + entry;
                }
            }

            if (breaked) {
                break;
            }

            files = newFilePart;
        }
    }

    for (int i=0; i<files.size(); ++i) {
        QFileInfo fi(QDir(directory), files[i]);
        files[i] = fi.absoluteFilePath();
    }

    return files;
}

void SpinConfigFileParser::parseSpeechFiles(const QDomElement &speechfiles, SpinConfig *into,
        const QString &path)
{
    Q_ASSERT(speechfiles.tagName() == "speechmaterial_files");

    for (QDomElement speechfile = speechfiles.firstChildElement(); !speechfile.isNull();
            speechfile = speechfile.nextSiblingElement()) {
        Q_ASSERT(speechfile.tagName() == "speechmaterial_file");
        QString fileIdentifier = speechfile.attribute(QSL("href"));
        QString matchType = speechfile.attribute(QSL("mode"));

        QStringList files;
        if(matchType.isEmpty() || matchType == "name") {
            QFileInfo fileInfo(QDir(path), fileIdentifier);
            files << fileInfo.absoluteFilePath();
        } else if (matchType == "wildcard") {
            files = expandWildcards(path, fileIdentifier);
        } else {
            qFatal("Invalid match type");
        }

        Q_FOREACH (const QString &file, files) {
            SpinConfigFileParser speechParser(file);
            QDomDocument doc = speechParser.loadAndUpgradeDom();
            // TODO all of this should really be done in speechParser.parse()
            for (QDomElement child = doc.documentElement().firstChildElement(); !child.isNull();
                    child = child.nextSiblingElement()) {
                QString tag = child.tagName();
                if (tag == "speechmaterial")
                    parseSpeech(child, into);
                else if (tag == "noises")
                    parseNoises(child, into);
            }
        }
    }
}

void SpinConfigFileParser::parseSubjectScreen(const QDomElement &screen, SpinConfig* into)
{
    Q_ASSERT(screen.tagName() == "subject_screen");
    into->setSubjectScreen(XmlUtils::nodeToString(screen.firstChildElement()));
}

void SpinConfigFileParser::parseExperimenterScreenQuiet(const QDomElement &screen, SpinConfig* into)
{
    Q_ASSERT(screen.tagName() == "experimenter_screen_quiet");
    into->setExperimenterScreenQuiet(XmlUtils::nodeToString(screen.firstChildElement()));
}

void SpinConfigFileParser::parseExperimenterScreenNoise(const QDomElement &screen, SpinConfig* into)
{
    Q_ASSERT(screen.tagName() == "experimenter_screen_noise");
    into->setExperimenterScreenNoise(XmlUtils::nodeToString(screen.firstChildElement()));
}


void SpinConfigFileParser::parseCustomScreens(const QDomElement &screens, SpinConfig* into)
{
    Q_ASSERT(screens.tagName() == "custom_screens");

    for (QDomElement screen = screens.firstChildElement(); !screen.isNull();
            screen = screen.nextSiblingElement()) {
        Q_ASSERT(screen.tagName() == "screen");
        QString description = screen.attribute(QSL("description"));
        QString id = screen.attribute(QSL("id"));
        if (description.isEmpty())
            description = id;
        Q_ASSERT(!description.isEmpty());
        into->addCustomScreen(description, id, XmlUtils::nodeToString(screen));
    }
}

void SpinConfigFileParser::parseIds(const QDomElement &ids, SpinConfig* into)
{
    Q_ASSERT(ids.tagName() == "id_setup");
    into->setGainId(ids.firstChildElement(QSL("gain")).text());
    into->setTextId(ids.firstChildElement(QSL("text")).text());
}

void SpinConfigFileParser::parseGeneralData(const QDomElement &data, SpinConfig* into)
{
    into->setInternalRms(data.firstChildElement(QSL("internal_rms")).text().toDouble());
    into->setDefaultCalibration(data.firstChildElement(QSL("default_calibration")).text().toDouble());
}

void SpinConfigFileParser::parseSoundcardSetup(const QDomElement &data, SpinConfig* into)
{
    for (QDomElement node = data.firstChildElement(); !node.isNull();
            node = node.nextSiblingElement()) {
        QString tagname(node.tagName());
        QString value(node.text());

        if (tagname == "blocksize")
            into->setSoundcardBlocksize(value.toUInt());
        else if (tagname == "buffersize")
            into->setSoundcardBuffersize(value.toUInt());
        else if (tagname == "driver")
            into->setSoundcardDriver(value);
    }
}

void SpinConfigFileParser::addLists(const QDomNodeList &lists, spin::data::CategoryMap* into,
                                    const QString &category)
{
    for (int i = 0; i < lists.length(); i++) {
        QDomElement listNode = lists.item(i).toElement();
        Q_ASSERT(listNode.tagName() == "list");

        spin::data::List list;
        list.id = listNode.attribute(QSL("id"));

        //get the speechtokens
        for (QDomElement token = listNode.firstChildElement(); !token.isNull();
                token = token.nextSiblingElement()) {
            Q_ASSERT(token.tagName() == "speechtoken");

            QString id = token.attribute(QSL("id"));
            QString file = token.firstChildElement(QSL("file")).text();
            QString text = XmlUtils::richText(token.firstChildElement(QSL("text")));

            list.addToken(id, file, text);
        }

        into->addList(list, category);
    }
}

static void fixUri(const QDomDocument &document, const QString &from,
        const QString &to)
{
    QDomNodeList list = document.elementsByTagName(from);
    // walk backwards as the list is updated dynamically
    for (int i = list.count() - 1; i >= 0; --i) {
        QDomElement element = list.at(i).toElement();
        QString uri = element.text();
        if (uri.startsWith(QSL("file:")) || uri.contains(QSL("%20")))
            uri = QUrl(uri).path();
        element.setTagName(to);
        XmlUtils::setText(&element, uri);
    }
}

void SpinConfigFileParser::upgrade3_1_3()
{
    // uri -> file
    fixUri(document, QSL("uri_prefix"), QSL("prefix"));
    fixUri(document, QSL("uri"), QSL("file"));
}
