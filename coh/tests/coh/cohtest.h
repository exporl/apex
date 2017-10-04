/******************************************************************************
 * Copyright (C) 2007,2012  Michael Hofmann <mh21@piware.de>                  *
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

#ifndef _EXPORL_COH_TESTS_COH_COHTEST_H_
#define _EXPORL_COH_TESTS_COH_COHTEST_H_

#include <QObject>

namespace coh
{
class CohSequence;
}

class CohTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void aseq();
    void aseq_data();
    void codacsStreaming();
    void codacsStreaming_data();
    void coh();
    void incomplete();
    void mapper();
    void nicCiClient();
    void nicCiClient_data();
    void nicXmlCreator();
};

QString nic_test_xml(coh::CohSequence *sequence);
QString nic_test_txt(coh::CohSequence *sequence);

#endif
