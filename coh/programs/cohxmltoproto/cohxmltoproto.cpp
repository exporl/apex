/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

#include "coh/coh.h"
#include "coh/cohprotodumper.h"
#include "coh/cohxmlloader.h"

#include "common/commonapplication.h"

#include <QFile>

using namespace cmn;
using namespace coh;

int main(int argc, char *argv[])
{
    CommonCoreApplication app(argc, argv);

    QFile input(app.arguments().at(1));
    input.open(QIODevice::ReadOnly);

    QFile output(app.arguments().at(2));
    output.open(QIODevice::WriteOnly);
    output.write(dumpCohSequenceProto(loadCohSequenceXml(input.readAll())));
}
