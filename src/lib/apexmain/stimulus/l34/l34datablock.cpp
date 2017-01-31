/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "./l34datablock.h"
#include "datablock/datablockdata.h"
#include "l34device.h"
#include <assert.h>
#include <QFile>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qdatastream.h>
#include <QString>
#include <QUrl>
#include <QFileInfo>

#include <iostream>
#include <fstream>
#include <cmath>            // round

#include <string.h>     // char functions to speed up xml generation

#include "qicparser.h"
#include "aseqparser.h"

//from libdata
#include "map/apexmap.h"

namespace apex
{

namespace stimulus
{

L34DataBlock::L34DataBlock(const data::DatablockData& data, const QUrl& filename,
                           const ExperimentRunDelegate* experiment) :
    DataBlock(data, filename, experiment),
    bIsXMLData(false),
    mappedForVolume(-1),
    m_length(0)
{
    this->data.setNbChannels(1);
    const QString sFileName = filename.path();

    if (data.uri().isEmpty()) {
        m_XMLData += data.directData().split(QLatin1Char('\n'));
        bIsXMLData = true;
    } else {
        if( !ParseFile( sFileName) )
            throw(ApexStringException("error opening/parsing L34datablock" +
                        sFileName));
    }
}

L34DataBlock::~L34DataBlock()
{
}

const L34XMLData& L34DataBlock::GetMappedData(data::ApexMap* pMap, float volume) const
{
    if (bIsXMLData) {
//        qDebug("We don't map");
        return m_XMLData;
    }
    if (volume!=mappedForVolume) {
//        qDebug("Volume differs, doing mapping");
        ConvertToMappedXMLData(pMap, volume);
    } else {
//        qDebug("Volume the same, NOT mapping again");
    }

    m_XMLData.SetLength(m_length);

    return m_XMLData;
}

void L34DataBlock::ConvertToMappedXMLData(data::ApexMap* pMap, float volume) const
{
    // TODO: check of mapping nog niet gebeurd is

//     qDebug("Entering ConvertToMappedXMLData");
//     qDebug("File extension is %s", qPrintable(file_extension));

    Q_ASSERT(volume>=0);
    Q_ASSERT(volume<=100);

    QTime time;
    time.restart();

    Q_ASSERT(pMap);
//    qDebug("Map size: %s", qPrintable(QString::number((unsigned)pMap->size())));

    m_XMLData.clear();

    if(!file_extension.compare("qic")) {

        int ref=(*pMap).defaultMap().referenceElectrode();

        double period=(*pMap).defaultMap().period();		// debug

        // Matthias: [Header for .qic files]
        QString header =
                QString(NIC_STREAM_OPEN) +
                QString(NIC_SEQUENCE_OPEN) +
                QString("<stimulus repeat=\"0\"><parameters><cl>0</cl>") +
            "<pw>" + QString::number((*pMap).defaultMap().phaseWidth()) +"</pw>" +
            "<pg>" + QString::number((*pMap).defaultMap().phaseGap()) +"</pg>" +
            "<t>" + QString::number((*pMap).defaultMap().period(),'g',15) +"</t>" +
            QString("<ae>1</ae><re>") + QString::number(ref) + "</re></parameters></stimulus>";
        //QString header = "<stream>";

        //<rate>0</rate> <cpc>4</cpc> <ae>20</ae> <re>0</re> <cl>20</cl> <t>31.6</t>

        try {
            m_XMLData.push_back(header);
        } catch ( std::bad_alloc &ex ) {
            std::cerr<< ex.what() <<std::endl;
            Q_ASSERT("problem pushing back header");
        }


        // template to be filled in later on
        char templ_norep[] = "<stimulus><parameters><re>%d</re><ae>%d</ae><cl>%d</cl></parameters></stimulus>";
        char templ_rep[] = "<stimulus repeat=\"%d\"><parameters><re>%d</re><ae>%d</ae><cl>%d</cl></parameters></stimulus>";

        char powerup_rep[] = "<stimulus repeat=\"%d\"><parameters><re>0</re><ae>0</ae><cl>0</cl></parameters></stimulus>";

        int stringielen=strlen(templ_rep)+100;        // rep is the longest + some space for numbers
        char* stringie = (char*) malloc(stringielen);

        int newmag=0;
        int tAE=0;

        int prevchan=-2;
        float prevmag=-2;

        int curcount=0;         // number of times we see the same stimulus

        // calculate datablock length
        m_length = long (m_Data.size() * period);

        for (L34Data::const_iterator it = m_Data.begin(); it != m_Data.end(); ++it) {
            // FIXME hier kan wel wat gecomprimeerd worden (=tradeoff cpu/memory)

            int chan = (*it).channel;
            float mag = (*it).magnitude;

            if (chan==prevchan && mag==prevmag) {
                ++curcount;
                continue;
            } else {
                if (curcount!=0) {
                    if (newmag==0) {
                        sprintf(stringie,powerup_rep , curcount );
                    } else {
                        if (curcount==1) {
                            sprintf(stringie,templ_norep , ref, tAE, newmag );
                        } else {
                            sprintf(stringie,templ_rep, curcount, ref, tAE, newmag );
                        }
                    }
                    m_XMLData.push_back(stringie);
                }

                curcount=1;
                const data::ChannelMap& curMap= (*pMap)[chan];        // cache zodat map lookup slechts 1 keer gebeurt

                int thr=curMap.thresholdLevel();
                int comf=curMap.comfortLevel();

                if (chan==0) {
                    newmag=0;
                    tAE=1;
                } else {
                    newmag = int (std::floor( (thr + (comf-thr)*volume/100*mag)   +0.5));
                    tAE=curMap.stimulationElectrode();
                }

            }
            prevchan=chan;
            prevmag=mag;


            // Q_ASSERT(strlen(stringie)<stringielen);
        }

        // one more time
        if (newmag==0)
            sprintf(stringie,powerup_rep , curcount );
        else
            sprintf(stringie,templ_rep, curcount , ref, tAE, newmag );
        m_XMLData.push_back(stringie);

        free(stringie);

        //m_XMLData.push_back("</nic:sequence>");
        m_XMLData.push_back(NIC_SEQUENCE_END);
        m_XMLData.push_back(NIC_STREAM_END);

        //    bIsXMLData=true;
        mappedForVolume=volume;

    }
    else if(!file_extension.compare("qicext")) {

        int refEl = (*pMap).defaultMap().referenceElectrode();
        QString header =
                QString(NIC_STREAM_OPEN) +
                QString(NIC_SEQUENCE_OPEN) +
                QString("<stimulus repeat=\"0\"><parameters><cl>0</cl>") +
            "<pw>" + QString::number((*pMap).defaultMap().phaseWidth()) +"</pw>" +
            "<pg>" + QString::number((*pMap).defaultMap().phaseGap()) +"</pg>" +
            "<t>" + QString::number((*pMap).defaultMap().period(),'g',15) +"</t>" +
            QString("<ae>1</ae><re>") + QString::number(refEl) + "</re></parameters></stimulus>";


        try {
            m_XMLData.push_back(header);
        }
        catch ( std::bad_alloc &ex ) {
            std::cerr<< ex.what() <<std::endl;
            Q_ASSERT("problem pushing back header");
        }

        // Matthias: [Templates for writing stimulus commands]
        char templ_norep[] = "<stimulus><parameters><ae>%d</ae><re>%d</re><cl>%d</cl><t>%f</t><pw>%f</pw><pg>%f</pg></parameters></stimulus>";
        char templ_rep[] = "<stimulus repeat=\"%d\"><parameters><ae>%d</ae><re>%d</re><cl>%d</cl><t>%f</t><pw>%f</pw><pg>%f</pg></parameters></stimulus>";
        char powerUp_rep_nodur[] = "<stimulus repeat=\"%d\"><parameters><ae>0</ae><re>0</re><cl>0</cl></parameters></stimulus>";
        char powerUp_rep_dur[] = "<stimulus repeat=\"%d\"><parameters><ae>0</ae><re>0</re><cl>0</cl><t>%f</t></parameters></stimulus>";

        int stimuliSeqLen  = strlen(templ_rep)+1000;        // rep is the longest + some space for numbers
        char* stimuliSeq = (char*) malloc(stimuliSeqLen);

        int newMag;
        int tAE;

        // Matthias: [to store values of the previous stimulus]
        int prevChan	= -2;
        float prevMag	= -2;
        float prevPer	= -2;
        int prevMode	= -2;
        float prevPw    = -2;
        float prevPg	= -2;

        int repCount = 0;
        for (L34Data::const_iterator it = m_Data.begin(); it != m_Data.end(); ++it) {

            int chan	= (*it).channel;
            float mag	= (*it).magnitude;
            float per	= (*it).period;
            int mode	= (*it).mode;
            float pw    = (*it).phaseWidth;
            float pg	= (*it).phaseGap;

            if (chan == prevChan &&
                    mag  == prevMag  &&
                    per  == prevPer  &&
                    mode == prevMode &&
                    pw   == prevPw   &&
                    pg   == prevPg) {
                ++repCount;
                continue;
            }
            else {
                if (repCount > 0) {
                    // Matthias: [Will this powerUp-Frame work with variable periods ?]
                    if(newMag == 0) {
                        if(prevPer == 0){
                            sprintf(stimuliSeq, powerUp_rep_nodur, repCount);
                        }
                        else {
                            sprintf(stimuliSeq, powerUp_rep_dur, repCount, prevPer);
                        }
                    }
                    else {
                        switch (repCount) {
                        case 1:
                            sprintf(stimuliSeq, templ_norep, tAE, refEl, newMag, prevPer, prevPw, prevPg);
                            break;
                        default:
                            sprintf(stimuliSeq, templ_rep, repCount, tAE, refEl, newMag, prevPer, prevPw, prevPg);
                            break;
                        }
                    }
                    try {
                        m_XMLData.push_back(stimuliSeq);
                    }
                    catch ( std::bad_alloc &ex ) {
                        std::cerr<< ex.what() <<std::endl;
                        Q_ASSERT("problem pushing back stimuli sequence");
                    }
                }

                repCount=1;
                const data::ChannelMap& curMap = (*pMap)[chan];        // cache zodat map lookup slechts 1 keer gebeurt

                int thr  = curMap.thresholdLevel();
                int comf = curMap.comfortLevel();

                if (chan == 0) {
                    newMag = 0;
                    tAE = 1;
                }
                else {
                    newMag	= std::floor( (thr + (comf-thr)*volume/100*mag) + 0.5);
                    tAE		= curMap.stimulationElectrode();
                }
            }

            prevChan = chan;
            prevMag  = mag;
            prevPer	 = per;
            prevMode = mode;
            prevPw   = pw;
            prevPg   = pg;

            //Q_ASSERT(strlen(stimuliSeq)<stimuliSeqLen);
        }

        free(stimuliSeq);

        m_XMLData.push_back(NIC_SEQUENCE_END);
        m_XMLData.push_back(NIC_STREAM_END);

        // bIsXMLData=true;
        mappedForVolume=volume;

    }  else if(!file_extension.compare("aseq")) {
        Q_ASSERT(m_Data.size());
        //L34Stimulus firstStimulus = m_Data[0];
        
//        int activeElectrode     = m_Data[0].activeElectrode;
        int returnElectrode     = m_Data[0].returnElectrode;
//        int currentLevel        = m_Data[0].currentLevel;
        float phaseWidth        = m_Data[0].phaseWidth;
        float phaseGap          = m_Data[0].phaseGap;
        float period            = m_Data[0].period;

        const data::ChannelMap& defaultMap( pMap->defaultMap());
        
        if (phaseWidth==-1)
            phaseWidth=defaultMap.phaseWidth();
        
        if (phaseGap==-1)
            phaseGap=defaultMap.phaseGap();

        if (period==-1)
            period=defaultMap.period();

        // fixme: return electrode
        
        char templ_norep[] =
                "<stimulus><parameters>"
                "<ae>%d</ae>"
                "<re>%d</re>"
                "<cl>%d</cl>"
                "<t>%1.1f</t>"
                "<pw>%1.1f</pw>"
                "<pg>%1.1f</pg>"
                "</parameters></stimulus>";

        char templ_rep[] =
                "<stimulus repeat=\"%d\"><parameters>"
                "<ae>%d</ae>"
                "<re>%d</re>"
                "<cl>%d</cl>"
                "<t>%1.1f</t>"
                "<pw>%1.1f</pw>"
                "<pg>%1.1f</pg>"
                "</parameters></stimulus>";

        char powerUp_rep[] =
                "<stimulus repeat=\"%d\"><parameters>"
                "<ae>0</ae><re>0</re><cl>0</cl>"
                "<t>%1.1f</t>"
                "</parameters></stimulus>";

        // rep is the longest + some space for numbers
        int stimuliSeqLen  = strlen(templ_rep)+1000;
        // buffer for printf      
        char* printbuffer = (char*) malloc(stimuliSeqLen);

        m_XMLData.push_back(NIC_STREAM_OPEN);
        m_XMLData.push_back(NIC_SEQUENCE_OPEN);

        
        L34Stimulus prevStim=m_Data[0];
        int repetitions=1;
        for (int c=1; c<=m_Data.size(); ++c) {
            L34Stimulus curStim;
            if (c!=m_Data.size()) {
                curStim=m_Data[c];
                    
                if ( curStim == prevStim) {
                    ++repetitions;
                     continue;
                }
            }
            int ae;
            int cl;

            // powerup?
            bool isPowerup=false;
            if ( prevStim.activeElectrode == 0 ||
                 prevStim.channel ==0 )
                isPowerup=true;

            if (prevStim.activeElectrode!=-1)
                ae=prevStim.activeElectrode;
            else
                ae=(*pMap)[prevStim.channel].stimulationElectrode();

            // [FIXME] should get T and C based on ae, not based on channel.
            // This won't work if channel is not defined
            int T = -1;
            int C = -1;
            if (!isPowerup && prevStim.activeElectrode==-1) {
                T=(*pMap)[prevStim.channel].thresholdLevel();
                C=(*pMap)[prevStim.channel].comfortLevel();
                Q_ASSERT ( T>=0 && T<=255);
                Q_ASSERT ( C>=0 && C<=255);
                Q_ASSERT ( T<=C );
            }


            if (prevStim.currentLevel!=-1) {
                cl=prevStim.currentLevel;
                if (cl==0)
                    isPowerup=true;
            } else {
                Q_ASSERT(prevStim.magnitude!=-1);
                if (prevStim.magnitude<0 || isPowerup) {
                    isPowerup=true;
                } else {
                    Q_ASSERT(prevStim.activeElectrode == -1) ;    // Mapping not implemented for direct electrode specification
                    Q_ASSERT ( prevStim.magnitude>=0 && prevStim.magnitude<=1 );
                    Q_ASSERT ( T>=0 && T<=255);
                    Q_ASSERT ( C>=0 && C<=255);
                    Q_ASSERT ( T<=C );
                    cl = std::floor(
                            (T + (C-T)*volume/100*prevStim.magnitude) + 0.5);
                }
//                qDebug("t=%d, c=%d, m=%f, cl=%d", t, c, prevStim.magnitude, cl);
            }

            if (prevStim.returnElectrode!=L34_INVALID)
                returnElectrode=prevStim.returnElectrode;

            if (prevStim.phaseWidth!=-1)
                phaseWidth=prevStim.phaseWidth;

            if (prevStim.period!=-1)
                period=prevStim.period;

            if (isPowerup ) {
                sprintf(printbuffer, powerUp_rep,
                        repetitions, period);
            } else {
                Q_ASSERT(ae>=-3 && ae<23);
                Q_ASSERT(returnElectrode>=-3 && returnElectrode<23);
                Q_ASSERT(cl>0 && cl<=255);
                Q_ASSERT(period>0 && period<13107);
                Q_ASSERT(phaseWidth>0 && phaseWidth<1000);
                Q_ASSERT(phaseGap>0 && phaseGap<1000);

                if (C>=0 && cl>C)
                    throw( ApexStringException(
                            tr("Current level (%1) larger than C (%2), aborting")
                            .arg(cl).arg(C) ));
                
            
                if (repetitions==1)
                    sprintf(printbuffer, templ_norep,
                        ae, returnElectrode, cl, period, phaseWidth, phaseGap);
                else
                    sprintf(printbuffer, templ_rep, repetitions,
                        ae, returnElectrode, cl, period, phaseWidth, phaseGap);
            }
            
                
            m_XMLData.push_back(printbuffer);
            repetitions=1;
            prevStim=curStim;
        }

        m_XMLData.push_back(NIC_SEQUENCE_END);
        m_XMLData.push_back(NIC_STREAM_END);
        
    }  else {
        qDebug("Unknown mapping extension");
    }

    qDebug("** Time in ConvertToMappedXMLData: %s",
            qPrintable(QString::number(time.elapsed())));
}

bool L34DataBlock::canMap(data::ApexMap* pMap) const
{
    Q_ASSERT(m_Data.size());
    //L34Stimulus firstStimulus = m_Data[0];
        
    int activeElectrode     = m_Data[0].activeElectrode;
    int returnElectrode     = m_Data[0].returnElectrode;
    int currentLevel        = m_Data[0].currentLevel;
    float phaseWidth        = m_Data[0].phaseWidth;
    float phaseGap          = m_Data[0].phaseGap;
    float period            = m_Data[0].period;
       
    int channel             = m_Data[0].channel;
    int magnitude           = m_Data[0].magnitude;

    if (activeElectrode==-1 || returnElectrode==L34_INVALID) {
        if (channel==-1)
            return false;

        if (! pMap->isComplete())
            return false;
    }


    // FIXME: in the current implementation, the following will always succeed
    // because maps with empty (-1, invalid) values are not possible
    
    const data::ChannelMap& defaultMap( pMap->defaultMap());

    if ( returnElectrode == L34_INVALID &&
         defaultMap.referenceElectrode()== L34_INVALID)
        return false;

    if ( currentLevel==-1 && magnitude==-1)
        return false;

    if ( phaseWidth==-1 && defaultMap.phaseWidth()==-1)
        return false;

    if ( phaseGap==-1 && defaultMap.phaseGap()==-1)
        return false;

    if ( period==-1 && defaultMap.period()==-1)
        return false;

    return true;
    
}

bool L34DataBlock::ParseFile( QString pFilename )
{
//    qDebug("Parsing file %s", qPrintable(pFilename));

    if (pFilename.toLower().endsWith(".xml")) {
//        qDebug("Parsing XML stimulus file\n");

        QFile file( pFilename );
        if (!file.open( QIODevice::ReadOnly)) {
            qDebug("Can't open file for reading");
            return false;
        }

        QTextStream stream( &file );
        QString line;
        //              int i = 1;
        while( !stream.atEnd() ) {
            line = stream.readLine(); //line of text excluding '\n'
            //                  printf( "%3d: %s\n", i++, line.latin1() );
            if (line.startsWith("<?xml"))
                continue;
            m_XMLData.push_back(line);
        }
        file.close();

        bIsXMLData=true;
    } else if (pFilename.toLower().endsWith(".qic")) {
        QFileInfo finfo(pFilename);
        file_extension = finfo.suffix();

        QFile file(pFilename);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug("Can't open file for reading");
            return false;
        }
        QicParser parser(file.readAll());
        m_Data = parser.GetData();

        bIsXMLData=false;
    } else if( pFilename.toLower().endsWith(".qicext")){
        QFileInfo finfo(pFilename);
        file_extension = finfo.suffix();

        qDebug("Parsing %s stimulus file: %s", qPrintable(file_extension),
                qPrintable(pFilename));
        QFile file(pFilename);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug("Can't open file for reading");
            return false;
        }
        QicParser parser(file.readAll());
        m_Data = parser.GetExtendedData();
        qDebug("Loaded %i stimuli", m_Data.size());

        bIsXMLData=false;
    } else if (pFilename.toLower().endsWith(".aseq")) {
        QFileInfo finfo(pFilename);
        file_extension = finfo.suffix();

        QFile file(pFilename);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug("Can't open file for reading");
            return false;
        }
        AseqParser parser(file.readAll());
        m_Data = parser.GetData();

        bIsXMLData=false;

    } else {
        throw( ApexStringException("Unknown L34 file type.\n"
                    "Supported types: .qic and .qicext"));
    }

    return true;
}

}

}


