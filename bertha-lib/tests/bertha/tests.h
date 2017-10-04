/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
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

#ifndef _BERTHA_SRC_TESTS_TESTS_H_
#define _BERTHA_SRC_TESTS_TESTS_H_

#include <QtTest>

class BerthaTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testBlockTree();
    void testBlockTreeLeafNodeSelection();
    void testCompressor();
    void testCompressor_data();
    void testConstraints();
    void testData();
    void testFft();
    void testFileInput();
    void testFiles();
    void testFileSink();
    void testFileSink_data();
    void testGain();
    void testGain_data();
    void testGainRamp();
    void testOlaFft();
    void testOlaFft_data();
    void testPlugin();
    void testProcessor();
    void testProcessorBlockConnections();
    void testProcessorLeafNodeSelection();
    void testProcessorLeafNodeSelectionWithParameters();
    void testParameterList();
    void testSin();
    void testSin_data();
    void testMixer();
    void testMixer_data();
    void testTypes();
    void testXmlLoader();
    void testHrtfMwf();
    void testHrtfMwf_data();
};

#endif
