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

#include "apexdata/map/apexmap.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xmlkeys.h"

#include "mapfactory.h"

#ifdef R126
#include "../gui/apexmapwizard.h"
#endif

#include <iostream>

using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using namespace r126;

namespace apex {

    MapFactory::MapFactory()
    {
    }


    MapFactory::~MapFactory()
    {

    }

    //static const



    data::ApexMap* MapFactory::GetMap( DOMElement * p_base ) {
        Q_ASSERT(p_base);
        Q_ASSERT(p_base->getNodeType() == DOMNode::ELEMENT_NODE);

        //const QString id = XMLutils::GetAttribute( p_base, gc_sID );
        data::ChannelMap basemap;      // this contains all default values
        data::ApexMap* result = new data::ApexMap;

        bool hasElec[22];      // array of bools, determining whether an electrode was defined
        memset(hasElec, 0, 22);

        p_base=(DOMElement*)(p_base->getFirstChild());
        Q_ASSERT(p_base);
        Q_ASSERT(p_base->getNodeType() == DOMNode::ELEMENT_NODE);
        const QString tag   = XMLutils::GetTagName( p_base );
        if (tag=="from_r126")
        {
#ifdef R126
            ApexMapWizard* pWiz = new ApexMapWizard();
            //cancelled??
            if( pWiz->exec() != QDialog::Accepted )
            {
                delete pWiz;
                delete result;
                return 0;
            }
            R126NucleusMAPRecord* pRec = pWiz->GetSelectedMap();
            delete result;
            result = R126ToApexMap(pRec);
            delete pRec;
            Q_ASSERT(result);
            delete pWiz;
#endif


        } else if (tag=="inline") {


            for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling()) {
                Q_ASSERT(currentNode);
                //qCDebug(APEX_RS, "%s", qPrintable(XMLutils::GetTagName( currentNode )));
                if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {

                    const QString tag   = XMLutils::GetTagName( currentNode );
                    const QString value = XMLutils::GetFirstChildText( currentNode );

                    if (tag == "mode") {
                        Q_ASSERT(value.isEmpty()==0);
                        basemap.setMode(data::ChannelMap::modeToStimulationModeType(value));
                        Q_ASSERT(basemap.mode() != -1 );
                    } else if (tag == "pulsewidth") {
                        basemap.setPhaseWidth(value.toDouble());
                    } else if (tag == "number_electrodes") {
                        result->setNumberOfElectrodes(value.toInt());
                    } else if (tag == "pulsegap") {
                        basemap.setPhaseGap(value.toDouble());
                    } else if (tag == "period") {
                        basemap.setPeriod(value.toDouble());
                    } else if  (tag == gc_sChannel) {    // this should be the last tag encountered, so basemap is valid
                        Q_ASSERT(basemap.isBaseValid());
                        if (result->defaultMap().totalRate() == -1)
                            result->setDefaultMap(basemap);
                        basemap.setChannelNumber(XMLutils::GetAttribute(currentNode, "number").toInt());
                        basemap.setStimulationElectrode(XMLutils::GetAttribute(currentNode, "electrode").toInt());
                        basemap.setThresholdLevel(XMLutils::GetAttribute(currentNode, "threshold").toInt());
                        basemap.setComfortLevel(XMLutils::GetAttribute(currentNode, "comfort").toInt());

                        //qCDebug(APEX_RS, "%s", qPrintable("channelnr = " + QString::number(basemap.channelNumber())));
                        //qCDebug(APEX_RS, "%s", qPrintable("electrode = " + QString::number(basemap.stimulationElectrode())));

                        Q_ASSERT(basemap.isValid());
                        //result->insert(result->begin()+basemap.m_nChannelNr, basemap);
                        (*result)[basemap.channelNumber()]=basemap;
                        Q_ASSERT( (*result)[basemap.channelNumber()].isValid() );


                        hasElec[basemap.channelNumber()-1]=true;

                    } else {
                        std::cerr << "Unknown element in mapfactory::getmap" << std::endl;
                    }

                } else {
                    Q_ASSERT(0);    // TODO
                }
            }

            if (basemap.phaseGap()+basemap.phaseWidth()*2>=basemap.period())
                log().addWarning("Mapfactory", QString("PhaseGap+PhaseWidth*2>=Period, this is not physically possible"));

            bool ok=true;
            for (int i=0; i<22; ++i) {
                if (hasElec[i]==false) {
                    log().addWarning("Mapfactory", QString("No map defined for channel %1").arg(i));
                    qCDebug(APEX_RS, "%s", qPrintable( QString("No map defined for channel %1").arg(i) ));
                    ok=false;
                }
            }
            if (!ok) {
                log().addWarning("Mapfactory", QString("Map not OK"));
                Q_ASSERT(0);    // TODO
            }
        } else {
            qCDebug(APEX_RS, "%s", qPrintable("Invalid tag: " + tag ));
            Q_ASSERT(0);
        }

#ifdef DEBUGMAP
        qCDebug(APEX_RS, "%s", result->ToXML());
#endif

        return result;

    }


    /**
    * Converts a R126NucleusMAPRecord to an ApexMap
    * @param p_map
    * @return new ApexMap
    */
    data::ApexMap* apex::MapFactory::R126ToApexMap( const R126NucleusMAPRecord * p_map )
    {

        data::ApexMap* result = new data::ApexMap;

        result->setNumberOfElectrodes(p_map->m_nChannels);

        data::ChannelMap defaultMap = result->defaultMap();
        defaultMap.setPhaseWidth(p_map->m_nMinPHaseWidth);
        defaultMap.setPhaseGap(p_map->m_nInterPhaseGap);
        //  result->m_defaultMap.m_nPeriod = p_map->m_nPeriod;
        defaultMap.setTotalRate(p_map->m_nTotalRate);
        //result->m_defaultMap.m_nMode = p_map->GetStimulationMode();
        result->setDefaultMap(defaultMap);



        // FIXME sommige parameters worden genegeerd


        for (int i=0; i<p_map->m_nChannels; ++i) {
            data::ChannelMap temp;
            temp.setComfortLevel(p_map->m_naCLevels[i]);
            temp.setThresholdLevel(p_map->m_naTLevels[i]);
            temp.setChannelNumber(i + 1);
            temp.setStimulationElectrode(p_map->m_naElectrodes[i]);

            Q_ASSERT(temp.comfortLevel() <= 255 && temp.comfortLevel() >= 0);
            Q_ASSERT(temp.thresholdLevel() >= 0 && temp.thresholdLevel() <= 255);
            Q_ASSERT(temp.thresholdLevel() <= temp.comfortLevel());
            Q_ASSERT(temp.channelNumber() >= 1 && temp.channelNumber() <= 22);
            Q_ASSERT(temp.stimulationElectrode() >= 1 && temp.stimulationElectrode() <= 22);

            //FIXME remove test if not needed. it is here to keep
            //the same behaviour is with std::map
            if (!result->contains(i + 1))
                result->insert(i + 1, temp);
            else
                qCWarning(APEX_RS, "FIXME: this should not happen!!!");
        }

        return result;
    }




}
