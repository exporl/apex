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
#include "coh/cohclient.h"

#include "common/commonapplication.h"
#include "common/utils.h"

#include <iostream>

using namespace cmn;
using namespace coh;

int main(int argc, char *argv[])
{
    CommonCoreApplication app(argc, argv);

    std::cout << "Creating sequence..." << std::endl;
    QScopedPointer<CohSequence> sequence(new CohSequence(1));

    CohSequence *part = new CohSequence(20);
    sequence->append(part);
    for (unsigned j = 0; j < 10000; ++j)
        part->append(new CohNullStimulus(200, true));

    try {
        QScopedPointer<CohClient> client(CohClient::createCohClient(
            argc < 2 ? QL1S("nic: l34-1") : QL1S(argv[1])));
        std::cout << "Sending sequence..." << std::endl;
        client->send(sequence.data());
        std::cout << "Starting streaming..." << std::endl;
        client->start(Coh::Immediate);
        milliSleep(3000);
        std::cout << "Retrieving status..." << std::endl;
        client->status();
        std::cout << "Stopping streaming..." << std::endl;
        client->stop();
        client.reset();
    } catch (const std::exception &e) {
        qFatal("%s", e.what());
    }
}
