/******************************************************************************
 * Copyright (C) 2017  Michael Hofmann <mh21@mh21.de>                         *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "apexmain/experimentparser.h"
#include "apexmain/mainconfigfileparser.h"

#include "apexspin/spinconfigfileparser.h"

#include "apextools/apexpaths.h"

#include "apextools/xml/xmltools.h"

#include "common/exception.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>

#include <iostream>

using namespace std;
using namespace cmn;
using namespace apex;
using namespace spin::parser;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QSL("file"), QSL("XML file to upgrade"));
    parser.addOption(QCommandLineOption(
        QStringList() << QSL("inplace") << QSL("i"),
        QSL("Replace input files if upgrade was successful")));
    parser.addOption(QCommandLineOption(
        QSL("parse"), QSL("For experiment files, actually try to parse them")));
    parser.process(app);

    if (parser.positionalArguments().isEmpty())
        parser.showHelp(1);

    bool inplace = parser.isSet(QSL("inplace"));
    bool parse = parser.isSet(QSL("parse"));
    int exitCode = 0;

    Q_FOREACH (const QString &xmlFile, parser.positionalArguments()) {
        try {
            QString target = inplace ? xmlFile : xmlFile + QSL(".upgraded");
            QString schema = XmlUtils::parseDocument(xmlFile)
                                 .documentElement()
                                 .attribute(QSL("xsi:schemaLocation"))
                                 .section(QL1C('/'), -1);
            QScopedPointer<UpgradableXmlParser> parser;
            if (schema == QUrl(QL1S(EXPERIMENT_SCHEMA_URL)).fileName())
                parser.reset(new ExperimentParser(xmlFile));
            else if (schema == QUrl(QL1S(CONFIG_SCHEMA_URL)).fileName())
                parser.reset(new MainConfigFileParser(xmlFile));
            else if (schema == QUrl(QL1S(SPIN_SCHEMA_URL)).fileName())
                parser.reset(new SpinConfigFileParser(xmlFile));
            else
                throw Exception(
                    app.tr("Unknown format %1: %2").arg(schema, xmlFile));

            XmlUtils::writeDocument(parser->loadAndUpgradeDom(false), target);

            if (parse && schema == QUrl(QL1S(EXPERIMENT_SCHEMA_URL)).fileName())
                ExperimentParser(target).parse(false);
        } catch (const std::exception &e) {
            cerr << qPrintable(xmlFile) << ": " << e.what() << endl;
            exitCode = 1;
        }
    }

    return exitCode;
}
