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

#include "apexdata/connection/connectiondata.h"

#include "apexdata/stimulus/stimulusdata.h"

#include "apextools/xml/xmlkeys.h"

#include "connection/connection.h"

#include "datablock.h"
#include "playmatrix.h"

#include <QDebug>

#include <QtGlobal>

using namespace apex;
using namespace apex::stimulus;

namespace
{
const unsigned sc_nMaxNumItems = 128; // initial size of working buf
const QString cDBlTag = "datablock";  // tags we use
const QString cSeqTag = "sequential";
const QString cSimTag = "simultaneous";

QString f_sAppendRow(const QString &ac_sID,
                     const int nRow) // use int, saves us a conversion
{
    return QString(ac_sID + "_" + QString::number(nRow));
}
}

PlayMatrixCreator::PlayMatrixCreator()
    : m_WorkHere(sc_nMaxNumItems, sc_nMaxNumItems),
      m_nSeq(0),
      m_nSim(0),
      m_nSimRec(0)
{
}

PlayMatrixCreator::~PlayMatrixCreator()
{
}

PlayMatrix *
PlayMatrixCreator::createMatrix(const data::StimulusDatablocksContainer &data)
{
    using data::StimulusDatablocksContainer;

    if (!data.id.isEmpty() || data.count() > 0) {
        // it's a stim with 1 datablock only
        if (data.count() == 0) {
            const QString id = data.id;
            Q_ASSERT(id.length() > 0);
            m_WorkHere.mp_Set(0, 0, id);
            m_nSeq = 1;
            m_nSim = 1;
        }
        // it's a more complicated stim
        else {
            const QString type = data.typeName();
            unsigned nSeq = 0;
            unsigned nSim = 0;
            parseDatablocks(type, data, nSeq, nSim);
        }

        // qCDebug(APEX_RS) << "before shrink ";
        // sf_DoDisplay(&m_WorkHere);

        // copy results to matrix with right size
        return sf_pShrinkedMatrix(&m_WorkHere, m_nSim, m_nSeq);
    }
    return new PlayMatrix(0, 0); // no playmatrix defined
}

void PlayMatrixCreator::sf_DoDisplay(const PlayMatrix *a_Mat)
{
    const unsigned cnRows = a_Mat->mf_nGetChannelCount();
    const unsigned cnCols = a_Mat->mf_nGetBufferSize();

    printf("\n------PlayMatrix------\n");

    for (unsigned i = 0; i < cnRows; ++i) {
        for (unsigned j = 0; j < cnCols; ++j) {
            const QString &id = a_Mat->operator()(i, j);
            if (!id.isEmpty())
                printf("%s\t", qPrintable(id));
            else
                printf(" xxxxxx \t");
        }
        printf("\n");
    }

    printf("----------------------\n\n");
}

void PlayMatrixCreator::sf_FixDuplicateIDs(PlayMatrix *a_pMatrix,
                                           tDataBlockMap &a_DataBlocks,
                                           data::ConnectionsData &a_Connections)
{
    const unsigned cnRows = a_pMatrix->mf_nGetChannelCount();
    const unsigned cnCols = a_pMatrix->mf_nGetBufferSize();

#ifdef PRINTPLAYMATRIX
    sf_DoDisplay(a_pMatrix);
#endif

    if (cnRows == 1)
        return;

    tQStringUintMap search;

    for (unsigned i = 0; i < cnRows; ++i) {
        for (unsigned j = 0; j < cnCols; ++j) {
            const QString sCur =
                (*a_pMatrix)(i, j); // No reference here, string gets changed!
            if (sCur.isEmpty())
                continue;
            if (i > 0) {
                tQStringUintMapCIt it = search.find(sCur);
                if (it != search.end() && (*it).second != i) {
                    const QString sCopy(f_sAppendRow(sCur, i));
                    a_pMatrix->mp_Set(i, j, sCopy);
                    DataBlock *pOrig = a_DataBlocks.value(sCur);
                    qCDebug(APEX_RS, "looking for %s", qPrintable(sCur));
                    Q_ASSERT(pOrig);
                    if (a_DataBlocks.value(sCopy) == 0) // check if no copy was
                                                        // made already for a
                                                        // previous stim
                    {
                        // copy datablock
                        qCDebug(APEX_RS, "Copying datablock %s to %s",
                                qPrintable(sCur), qPrintable(sCopy));
                        DataBlock *pCopy = pOrig->GetCopy(sCopy);
                        Q_ASSERT(pCopy);
                        a_DataBlocks[sCopy] = pCopy;

                        // copy all connections from datablock
                        // tConnections& curcs = a_Connections.find(
                        // pOrig->GetDevice() ).value();
                        qCDebug(APEX_RS, "Size of connections map: %d",
                                a_Connections.size());

                        /*Q_ASSERT( a_Connections.contains( pOrig->GetDevice())
                        );
                        tConnections& curcs = a_Connections[ pOrig->GetDevice()
                        ];
                        for( tConnections::size_type i = 0 ; i < curcs.size() ;
                        ++i )
                        {*/
                        for (data::ConnectionsData::const_iterator it =
                                 a_Connections.begin();
                             it != a_Connections.end(); ++it) {
                            // const tConnection& cur = curcs.at( i );
                            const data::ConnectionData *current = *it;
                            qCDebug(APEX_RS) << "sCur=" << sCur;
                            qCDebug(APEX_RS) << "fromId() = "
                                             << current->fromId();
                            if (current->fromId() == sCur) {
                                data::ConnectionData *newconnection =
                                    new data::ConnectionData(*current);
                                newconnection->setFromId(sCopy);
                                a_Connections.push_back(newconnection);
                            }
                            /*if( cur.m_sFromID == sCur )
                            {
                              tConnection newc = cur;
                              newc.m_sFromID = sCopy;
                              curcs.push_back( newc );
                              qCDebug(APEX_RS, "Copying connection to %s",
                            qPrintable( newc.m_sToID));
                            }*/
                        }
                    }
                }
            }
            search[sCur] = i;
        }
    }
}

void PlayMatrixCreator::parseDatablocks(
    const QString &ac_sPrevTag, const data::StimulusDatablocksContainer &data,
    unsigned &a_nSeq, unsigned &a_nSim)
{
    const unsigned len = data.count();

    for (unsigned i = 0; i < len; ++i) {
        const QString tag = data[i].typeName();
        if (tag == cDBlTag) {
            const QString id = data[i].id;
            Q_ASSERT(id.length() > 0);
            m_WorkHere.mp_Set(a_nSim, a_nSeq, id);
            if (ac_sPrevTag == cSeqTag)
                a_nSeq++;
            else if (ac_sPrevTag == cSimTag)
                a_nSim++;
        } else {
            parseDatablocks(tag, data[i], a_nSeq, a_nSim);
            m_nSimRec++;
        }
    }

    m_nSeq = s_max(m_nSeq, a_nSeq);
    m_nSim = s_max(m_nSim, a_nSim);

    if (ac_sPrevTag == cSimTag) {
        ++a_nSeq;
        a_nSim = 0;
    } else if (ac_sPrevTag == cSeqTag) {
        ++a_nSim;
        a_nSeq = 0;
    }

    if (m_nSim == 0)
        ++m_nSim;
    if (m_nSeq == 0)
        ++m_nSeq;
}

PlayMatrix *PlayMatrixCreator::sf_pShrinkedMatrix(const PlayMatrix *ac_pSrc,
                                                  const unsigned ac_nSim,
                                                  const unsigned ac_nSeq)
{
    PlayMatrix *pRet = new PlayMatrix(ac_nSim, ac_nSeq);

    for (unsigned i = 0; i < ac_nSim; ++i)
        for (unsigned j = 0; j < ac_nSeq; ++j)
            pRet->mp_Set(i, j, ac_pSrc->operator()(i, j));

#ifdef PRINTPLAYMATRIX
    sf_DoDisplay(pRet);
#endif

    QList<int> fullRows;
    for (unsigned i = 0; i < ac_nSim; ++i) {
        bool found = false;
        for (unsigned j = 0; j < ac_nSeq; ++j) {
            if (!(ac_pSrc->operator()(i, j).isEmpty())) {
                found = true;
                break;
            }
        }
        if (found)
            fullRows.push_back(i);
    }

    if ((unsigned)fullRows.size() != ac_nSim) {
        PlayMatrix *pNew = new PlayMatrix(fullRows.size(), ac_nSeq);
        int i = 0;
        Q_FOREACH (int r, fullRows) {
            qCDebug(APEX_RS, "Adding row %d", r);
            for (unsigned j = 0; j < ac_nSeq; ++j)
                pNew->mp_Set(i, j, pRet->operator()(r, j));
            ++i;
        }

        delete pRet;
        pRet = pNew;

#ifdef PRINTPLAYMATRIX
        qCDebug(APEX_RS, "New playmatrix:");
        sf_DoDisplay(pRet);
#endif
    }

    return pRet;
}

bool PlayMatrixCreator::sf_bCheckMatrixValid(const PlayMatrix *a_pMatrix,
                                             const tDataBlockMap &a_DataBlocks,
                                             QString &a_sError)
{
    const unsigned cnRows = a_pMatrix->mf_nGetChannelCount();
    const unsigned cnCols = a_pMatrix->mf_nGetBufferSize();

    /*Q_ASSERT( cnRows );
    Q_ASSERT( cnCols );*/

    if (cnCols == 1)
        return true;

    for (unsigned i = 0; i < cnRows; ++i) {
        if ((a_pMatrix->operator()(i, 0)).isEmpty())
            continue;
        const QString sDevice =
            a_DataBlocks.value(a_pMatrix->operator()(i, 0))->GetDevice();
        Q_ASSERT(!sDevice.isEmpty());
        for (unsigned j = 1; j < cnCols; ++j) {
            const QString &sCur = a_pMatrix->operator()(i, j);
            if (!sCur.isEmpty()) {
                //          qCDebug(APEX_RS, "Checking device of %s",
                //          qPrintable(sCur));
                Q_ASSERT(a_DataBlocks.value(sCur));
                if (a_DataBlocks.value(sCur)->GetDevice() != sDevice) {
                    a_sError = QString("not every item from row %1 is from the "
                                       "same device")
                                   .arg(i);
                    return false;
                }
            }
        }
    }
    return true;
}

PlayMatrix *
PlayMatrixCreator::sf_pCreateSubMatrix(const PlayMatrix *a_pMatrix,
                                       const tDataBlockMap &a_DataBlocks,
                                       const QString &ac_sDevice)
{
    const unsigned cnRows = a_pMatrix->mf_nGetChannelCount();
    const unsigned cnCols = a_pMatrix->mf_nGetBufferSize();

    Q_ASSERT(cnRows);
    Q_ASSERT(cnCols);

    PlayMatrix *pTemp = new PlayMatrix(cnRows, cnCols);
    unsigned nFinalRows = 0;
    unsigned nFinalCols = 0;

    /*for( unsigned i = 0 ; i < cnRows ; ++i )
    {
        if ( a_pMatrix->operator() ( i, 0 ).isEmpty())
            continue;
      if( ac_sDevice == a_DataBlocks.value( a_pMatrix->operator() ( i, 0 )
    )->GetDevice() )
      {
        unsigned nCurCols = 0;
        for( unsigned j = 0 ; j < cnCols ; ++j )
        {
          const QString& sCur = a_pMatrix->operator() ( i, j );
          pTemp->mp_Set( nFinalRows, j, sCur );
          if( !sCur.isEmpty() )
            ++nCurCols;
        }
        nFinalCols = s_max( nFinalCols, nCurCols );
        ++nFinalRows;
      }
    }*/

    for (unsigned i = 0; i < cnRows; ++i) {
        for (unsigned j = 0; j < cnCols; ++j) {

            if ((*a_pMatrix)(i, j).isEmpty())
                continue;

            if (ac_sDevice ==
                a_DataBlocks.value((*a_pMatrix)(i, j))->GetDevice()) {

                const QString &sCur = (*a_pMatrix)(i, j);
                pTemp->mp_Set(i, j, sCur);

                nFinalCols = s_max(nFinalCols, j);
                nFinalRows = s_max(nFinalRows, i);
            }
        }
    }

    PlayMatrix *pRet = sf_pShrinkedMatrix(pTemp, ++nFinalRows, ++nFinalCols);
    delete pTemp;
    return pRet;
};

DataBlockMatrix *
PlayMatrixCreator::sf_pConvert(const PlayMatrix *ac_pSrc,
                               const tDataBlockMap &a_DataBlocks)
{
    const unsigned cnRows = ac_pSrc->mf_nGetChannelCount();
    const unsigned cnCols = ac_pSrc->mf_nGetBufferSize();

    //  Q_ASSERT( cnRows );                   [Tom] can be empty...
    //  Q_ASSERT( cnCols );

    DataBlockMatrix *pRet = new DataBlockMatrix(cnRows, cnCols, true);

    for (unsigned i = 0; i < cnRows; ++i) {
        for (unsigned j = 0; j < cnCols; ++j) {
            DataBlock *p = a_DataBlocks.value(ac_pSrc->operator()(i, j));
            pRet->mp_Set(i, j, p);
        }
    }

    return pRet;
}
