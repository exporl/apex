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

#include "appcore/threads/criticalsection.h"
#include "appcore/threads/locks.h"

#include "containers/ownedarray.h"

#include "utils/stringutils.h"

#include "audiosamplebuffer.h"
#include "connections.h"

#include <QtGlobal>

using namespace appcore;
using namespace streamapp;

namespace
{
typedef std::pair<QString, ConnectItem *> tConnectablePair;
typedef std::pair<QString, StreamCallback *> tUnConnectablePair;
typedef tConnectable::iterator tConIt;
typedef tUnConnectable::iterator tUConIt;
typedef tConnectable::const_iterator tConCIt;
typedef tUnConnectable::const_iterator tUConCIt;
// bool bTrig;
//#define STRICT
}

ConnectItem::ConnectItem(const unsigned ac_nBufferSize,
                         const unsigned ac_nSockets)
    : mc_nChan(ac_nSockets),
      mc_nSize(ac_nBufferSize),
      m_Inputs(ac_nSockets, true),
      m_InputChans(ac_nSockets),
      m_FreeChan(ac_nSockets),
      m_Result(ac_nBufferSize, ac_nSockets)
{
    // std::cout << "ConnectItem" << std::endl;
    for (unsigned i = 0; i < ac_nSockets; ++i)
        m_FreeChan.mp_Set(i, true);
    // bTrig = true;
}

ConnectItem::~ConnectItem()
{
}

void ConnectItem::mp_AddInput(StreamCallback *const ac_pItem)
{
    const unsigned nChanToAdd = ac_pItem->mf_nGetNumChannels();
    for (unsigned i = 0; i < nChanToAdd; ++i) {
        int nIndex = mf_nGetFirstFreeChannel();
        if (nIndex != -1)
            mp_AddInput(ac_pItem, i, nIndex);
        else
            break; // not all inputs were added!
    }
}

void ConnectItem::mp_AddInput(StreamCallback *const ac_pItem,
                              const unsigned ac_nItemOutChannel)
{
    int nIndex = mf_nGetFirstFreeChannel();
    if (nIndex != -1)
        mp_AddInput(ac_pItem, ac_nItemOutChannel, nIndex);
}

void ConnectItem::mp_AddInput(StreamCallback *const ac_pItem,
                              const unsigned ac_nItemOutChannel,
                              const unsigned ac_nThisInChannel)
{
    Q_ASSERT(ac_nItemOutChannel < ac_pItem->mf_nGetNumChannels());
    m_Inputs.mp_Set(ac_nThisInChannel, ac_pItem);
    m_InputChans.mp_Set(ac_nThisInChannel, ac_nItemOutChannel);
    m_FreeChan.mp_Set(ac_nThisInChannel, false);
    // std::cout << toString(this) + " mp_AddInput()" + ac_pItem << std::endl;
    // bTrig = true;
}

void ConnectItem::mp_RemoveInput(const unsigned ac_nThisInChannel)
{
    m_Inputs.mp_Set(ac_nThisInChannel, 0);
    m_Result.ResetChannel(ac_nThisInChannel);
    m_FreeChan.mp_Set(ac_nThisInChannel, true);
    // std::cout << toString(this) + " mp_RemoveInput()" + ac_nThisInChannel <<
    // std::endl;
}

void ConnectItem::mp_RemoveInput(StreamCallback *const ac_pItem)
{
    int nIndex = sf_nFindInArray(ac_pItem, m_Inputs);
    while (nIndex != -1) {
        mp_RemoveInput(nIndex);
        nIndex = sf_nFindInArray(ac_pItem, m_Inputs, nIndex);
    }
}

void ConnectItem::mp_RemoveInput(StreamCallback *const ac_pItem,
                                 const unsigned ac_nItemOutChannel)
{
    int nIndex = sf_nFindInArray(ac_pItem, m_Inputs);
    while (nIndex != -1) {
        if (ac_nItemOutChannel == m_InputChans(nIndex))
            mp_RemoveInput(nIndex);
        nIndex = sf_nFindInArray(ac_pItem, m_Inputs, nIndex);
    }
}

void ConnectItem::mp_RemoveInput(StreamCallback *const ac_pItem,
                                 const unsigned ac_nItemOutChannel,
                                 const unsigned ac_nThisInChannel)
{
    int nIndex = sf_nFindInArray(ac_pItem, m_Inputs);
    while (nIndex != -1) {
        if (ac_nItemOutChannel == (unsigned)m_InputChans(nIndex) &&
            (unsigned)nIndex == ac_nThisInChannel) {
            mp_RemoveInput(nIndex);
            nIndex = -1;
        } else {
            nIndex = sf_nFindInArray(ac_pItem, m_Inputs, nIndex);
        }
    }
}

void ConnectItem::mp_RemoveAll()
{
    for (unsigned i = 0; i < mc_nChan; ++i)
        mp_RemoveInput(i);
}

unsigned ConnectItem::mf_nGetFreeChannels() const
{
    return m_FreeChan.mf_nGetNumFreeChannels();
}

int ConnectItem::mf_nGetFirstFreeChannel() const
{
    return m_FreeChan.mf_nGetFirstFreeChannel();
}

bool ConnectItem::mf_bIsFreeChannel(const unsigned ac_nIndex) const
{
    return m_FreeChan.mf_bIsFreeChannel(ac_nIndex);
}

bool ConnectItem::mf_bIsConnected() const
{
    return !(mf_nGetFreeChannels() == m_Result.mf_nGetNumChannels());
}

DeleteAtDestructionArray<ConnectItem::mt_eConnectionInfo> *
ConnectItem::mf_pGetConnections() const
{
    DeleteAtDestructionArray<ConnectItem::mt_eConnectionInfo> *pRet =
        new DeleteAtDestructionArray<ConnectItem::mt_eConnectionInfo>(mc_nChan);
    for (unsigned i = 0; i < mc_nChan; ++i)
        pRet->mp_Set(i, new ConnectItem::mt_eConnectionInfo(
                            m_Inputs(i), m_InputChans(i), i));
    return pRet;
}

void ConnectItem::mf_CallbackFunc()
{
    for (unsigned i = 0; i < mc_nChan; ++i) {
        StreamCallback *pToCall = m_Inputs(i);
        /*if (bTrig)
          std::cout << pToCall << std::endl;*/
        if (pToCall) {
            if (pToCall->mf_bIsPlaying()) {
                const unsigned nChanToGet = m_InputChans(i);
                const Stream &pResult = pToCall->mf_GetResult();
                m_Result.SetChannel(i, pResult.mf_pGetArray()[nChanToGet]);
            } else {
                m_Result.ResetChannel(i); // must be done everytime if
                                          // connecting from other thread??
            }
        }
    }
    // bTrig = false;
    mc_pResult = &m_Result.Read();
}

ConnectionManager::ConnectionManager() : mc_pLock(new CriticalSection())
{
}

ConnectionManager::~ConnectionManager()
{
    delete mc_pLock;
}

//-1: not found, 0: m_UnConnectable, 1: m_Connectable
int ConnectionManager::mf_nGetItemIndex(const QString &ac_sID) const
{
    tConCIt CoIt = m_Connectable.find(ac_sID);
    tUConCIt UCoIt = m_UnConnectable.find(ac_sID);
    if (CoIt != m_Connectable.end())
        return 1;
    if (UCoIt != m_UnConnectable.end())
        return 0;
    return -1;
}

ConnectItem *ConnectionManager::mf_pGetConnectable(const QString &ac_sID) const
{
    tConCIt CoIt = m_Connectable.find(ac_sID);
    if (CoIt != m_Connectable.end())
        return (*CoIt).second;
    return 0;
}

StreamCallback *
ConnectionManager::mf_pGetUnConnectable(const QString &ac_sID) const
{
    tUConCIt UCoIt = m_UnConnectable.find(ac_sID);
    if (UCoIt != m_UnConnectable.end())
        return (*UCoIt).second;
    return 0;
}

StreamCallback *ConnectionManager::mf_pGetAny(const QString &ac_sID) const
{
    StreamCallback *pRet = mf_pGetConnectable(ac_sID);
    if (pRet)
        return pRet;
    pRet = mf_pGetUnConnectable(ac_sID);
    return pRet;
}

QString ConnectionManager::mf_sGetAny(StreamCallback *const ac_pID) const
{
    tUConCIt itUV = m_UnConnectable.begin();
    tUConCIt itUE = m_UnConnectable.end();
    while (itUV != itUE) {
        if ((*itUV).second == ac_pID)
            return (*itUV).first;
        ++itUV;
    }
    tConCIt itV = m_Connectable.begin();
    tConCIt itE = m_Connectable.end();
    while (itV != itE) {
        if ((*itV).second == ac_pID)
            return (*itV).first;
        ++itV;
    }
    return QString();
}

void ConnectionManager::mp_RegisterItem(const QString &ac_sId,
                                        InputStream *const ac_pInput,
                                        const bool ac_bPlaying)
{
    if (mf_nGetItemIndex(ac_sId) != -1)
        return;

    InputStreamCallback *pNew = new InputStreamCallback(ac_pInput);
    pNew->mp_SetPlaying(ac_bPlaying);
    const Lock L(*mc_pLock);
    m_UnConnectable.insert(tUnConnectablePair(ac_sId, pNew));
    // std::cout << "mp_RegisterItem" + ac_sId << std::endl;
}

void ConnectionManager::mp_RegisterItem(const QString &ac_sId,
                                        OutputStream *const ac_pOutput)
{
    if (mf_nGetItemIndex(ac_sId) != -1)
        return;
    const Lock L(*mc_pLock);
    m_Connectable.insert(
        tConnectablePair(ac_sId, new OutputStreamCallback(ac_pOutput)));
    // std::cout << "mp_RegisterItem" + ac_sId << std::endl;
}

void ConnectionManager::mp_RegisterItem(const QString &ac_sId,
                                        IStreamProcessor *const ac_pProcessor)
{
    if (mf_nGetItemIndex(ac_sId) != -1)
        return;
    const Lock L(*mc_pLock);
    m_Connectable.insert(
        tConnectablePair(ac_sId, new ProcessorCallback(ac_pProcessor)));
    // std::cout << "mp_RegisterItem" + ac_sId << std::endl;
}

void ConnectionManager::mp_RegisterItem(const QString &ac_sId,
                                        ConnectItem *const ac_pInput)
{
    if (mf_nGetItemIndex(ac_sId) != -1)
        return;

    const Lock L(*mc_pLock);
    m_Connectable.insert(tConnectablePair(ac_sId, ac_pInput));
    // std::cout << "mp_RegisterItem" + ac_sId << std::endl;
}

void ConnectionManager::mp_Connect(const QString &ac_sFromID,
                                   const QString &ac_sToID)
{
    ConnectItem *pTo = mf_pGetConnectable(ac_sToID);
    StreamCallback *pFrom = mf_pGetAny(ac_sFromID);

#ifdef STRICT
    Q_ASSERT(pTo && pFrom);
#else
    if (!pTo || !pFrom)
        return;
#endif

    const Lock L(*mc_pLock);
    pTo->mp_AddInput(pFrom);
}

void ConnectionManager::mp_Connect(const QString &ac_sFromID,
                                   const QString &ac_sToID,
                                   const unsigned ac_nFromChannel,
                                   const unsigned ac_nToChannel)
{
    ConnectItem *pTo = mf_pGetConnectable(ac_sToID);
    StreamCallback *pFrom = mf_pGetAny(ac_sFromID);

#ifdef STRICT
    Q_ASSERT(pTo && pFrom);
#else
    if (!pTo || !pFrom)
        return;
#endif

    const Lock L(*mc_pLock);
    pTo->mp_AddInput(pFrom, ac_nFromChannel, ac_nToChannel);
    // std::cout << "mp_Connect" + ac_sFromID + ac_sToID << std::endl;
}

void ConnectionManager::mp_RemoveConnection(const QString &ac_sFromID,
                                            const QString &ac_sToID)
{
    ConnectItem *pTo = mf_pGetConnectable(ac_sToID);
    StreamCallback *pFrom = mf_pGetAny(ac_sFromID);
    Q_ASSERT(pTo && pFrom);

    const Lock L(*mc_pLock);
    pTo->mp_RemoveInput(pFrom);
}

void ConnectionManager::mp_RemoveConnection(const QString &ac_sFromID,
                                            const QString &ac_sToID,
                                            const unsigned ac_nFromChannel,
                                            const unsigned ac_nToChannel)
{
    ConnectItem *pTo = mf_pGetConnectable(ac_sToID);
    StreamCallback *pFrom = mf_pGetAny(ac_sFromID);
    Q_ASSERT(pTo && pFrom);

    const Lock L(*mc_pLock);
    pTo->mp_RemoveInput(pFrom, ac_nFromChannel, ac_nToChannel);
}

void ConnectionManager::mp_UnregisterItem(const QString &ac_sID)
{
    ConnectItem *pTo = mf_pGetConnectable(ac_sID);
    if (pTo) {
        const Lock L(*mc_pLock);
        pTo->mp_RemoveAll();
        delete pTo;
        m_Connectable.erase(ac_sID);
        return;
    }
    StreamCallback *pFrom = mf_pGetUnConnectable(ac_sID);
    if (pFrom) {
        tConCIt itV = m_Connectable.begin();
        tConCIt itE = m_Connectable.end();
        while (itV != itE) {
            const Lock L(*mc_pLock);
            (*itV).second->mp_RemoveInput(pFrom);
            ++itV;
        }
        delete pFrom;
        m_UnConnectable.erase(ac_sID);
    }
}

void ConnectionManager::mp_PlayAllItems()
{
    const Lock L(*mc_pLock);
    tUConIt itUV = m_UnConnectable.begin();
    tUConCIt itUE = m_UnConnectable.end();
    while (itUV != itUE) {
        (*itUV).second->mp_SetPlaying(true);
        ++itUV;
    }
    tConIt itV = m_Connectable.begin();
    tConCIt itE = m_Connectable.end();
    while (itV != itE) {
        (*itV).second->mp_SetPlaying(true);
        ++itV;
    }
}

void ConnectionManager::mp_PlayItem(const QString &ac_sItem,
                                    const bool ac_bPlay /* = true  */)
{
    StreamCallback *p = mf_pGetAny(ac_sItem);
    if (p) {
        const Lock L(*mc_pLock);
        p->mp_SetPlaying(ac_bPlay);
    }
}

bool ConnectionManager::mf_bIsRegistered(const QString &ac_sItem) const
{
    return mf_pGetAny(ac_sItem) != 0;
}

bool ConnectionManager::mf_bAllInputsConnected() const throw()
{
    // bweirk this can use some refactoring!
    const Lock L(*mc_pLock);

    // build array with all connection infos
    OwnedArray<DeleteAtDestructionArray<ConnectItem::mt_eConnectionInfo>> infos;
    tConCIt itV = m_Connectable.begin();
    tConCIt itE = m_Connectable.end();
    while (itV != itE) {
        const ConnectItem &cur(*(*itV).second);
        DeleteAtDestructionArray<ConnectItem::mt_eConnectionInfo> *p =
            cur.mf_pGetConnections();
        infos.mp_AddItem(p);
        ++itV;
    }
    const unsigned nInfos = infos.mf_nGetNumItems();

    // for all input-only items,
    // see if they're connected somewhere
    tUConCIt itUV = m_UnConnectable.begin();
    tUConCIt itUE = m_UnConnectable.end();
    while (itUV != itUE) {
        // figure out if it's used somewhere
        bool bThis = false;
        for (unsigned i = 0; i < nInfos; ++i) {
            DeleteAtDestructionArray<ConnectItem::mt_eConnectionInfo> *pCur =
                infos.mf_GetItem(i);
            const unsigned nConns = pCur->mf_nGetBufferSize();
            for (unsigned j = 0; j < nConns; ++j) {
                if (pCur->mf_Get(j)->m_pFrom == (*itUV).second) {
                    bThis = true;
                    break;
                }
            }
        }
        if (!bThis)
            return false;
        ++itUV;
    }
    return true;
}

void ConnectionManager::mp_DeleteRegisteredItems() throw()
{
    const Lock L(*mc_pLock);
    tUConIt itUV = m_UnConnectable.begin();
    tUConCIt itUE = m_UnConnectable.end();
    while (itUV != itUE) {
        delete (*itUV).second;
        ++itUV;
    }
    tConIt itV = m_Connectable.begin();
    tConCIt itE = m_Connectable.end();
    while (itV != itE) {
        delete (*itV).second;
        ++itV;
    }
    m_Connectable.clear();
    m_UnConnectable.clear();
}

tStringVector ConnectionManager::mf_saGetRoutes() const
{
    tStringVector ret;
    tConCIt itV = m_Connectable.begin();
    tConCIt itE = m_Connectable.end();
    while (itV != itE) {
        const QString &sCurO = (*itV).first;
        DeleteAtDestructionArray<ConnectItem::mt_eConnectionInfo> *cur =
            (*itV).second->mf_pGetConnections();
        const unsigned nCurItems = cur->mf_nGetBufferSize();

        for (unsigned j = 0; j < nCurItems; ++j) {
            ConnectItem::mt_eConnectionInfo *rCur = cur->mf_Get(j);
            QString sCur = mf_sGetAny(rCur->m_pFrom);
            ret.push_back(QString(sCur + " channel " + rCur->m_nFromChannel +
                                  " -> " + sCurO + " channel " +
                                  rCur->m_nToChannel)
                              .toStdString());
        }

        delete cur;

        ++itV;
    }
    return ret;
}

void ConnectionManager::mf_Callback()
{
    const Lock L(*mc_pLock);
    tUConIt itUV = m_UnConnectable.begin();
    tUConCIt itUE = m_UnConnectable.end();
    while (itUV != itUE) {
        (*itUV).second->mp_UnLockResult();
        ++itUV;
    }
    tConIt itV = m_Connectable.begin();
    tConCIt itE = m_Connectable.end();
    while (itV != itE) {
        (*itV).second->mp_UnLockResult();
        ++itV;
    }
    itV = m_Connectable.begin();
    while (itV != itE) {
        StreamCallback *pCur = (*itV).second;
        if (pCur->mf_bIsPlaying())
            pCur->mf_Callback();
        ++itV;
    }
}
