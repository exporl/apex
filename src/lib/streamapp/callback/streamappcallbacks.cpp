/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "../multistream.h"

#include "multiproc.h"
#include "streamappcallbacks.h"

#include <QtGlobal>

using namespace streamapp;

Channel::Channel(const bool ac_bDeleteContent)
    : mc_bDeleteContent(ac_bDeleteContent),
      m_pPlaySource(0),
      m_pPlaySink(0),
      m_pProcessor(0)
{
}

Channel::~Channel()
{
    if (mc_bDeleteContent) {
        delete m_pPlaySource;
        delete m_pPlaySink;
        delete m_pProcessor;
    }
}

void Channel::mp_SetProcessor(IStreamProcessor *const a_Proc)
{
    if (a_Proc != m_pProcessor) {
        if (mc_bDeleteContent)
            delete m_pProcessor;
        m_pProcessor = a_Proc;
    }
}

void Channel::mp_SetSource(InputStream *const a_SourceToPlayFrom)
{
    if (a_SourceToPlayFrom != m_pPlaySource) {
        if (mc_bDeleteContent)
            delete m_pPlaySource;
        m_pPlaySource = a_SourceToPlayFrom;
    }
}

void Channel::mp_SetSink(OutputStream *const a_DeviceToPlayOn)
{
    if (a_DeviceToPlayOn != m_pPlaySink) {
        if (mc_bDeleteContent)
            delete m_pPlaySink;
        m_pPlaySink = a_DeviceToPlayOn;
    }
}

void Channel::mf_Callback()
{
    if (m_pPlaySource && m_pPlaySink) {
        if (m_pProcessor)
            m_pPlaySink->Write(
                m_pProcessor->mf_DoProcessing(m_pPlaySource->Read()));
        else
            m_pPlaySink->Write(m_pPlaySource->Read());
    } else if (m_pPlaySource && m_pProcessor) {
        m_pProcessor->mf_DoProcessing(m_pPlaySource->Read());
    } else if (m_pPlaySource) {
        m_pPlaySource->Read();
    }
}

/*************************************************************************************************************/

/**
  * All this detail stuff is just here so that we don't have
  * to add the _fromv3 directory to apex' include path
  */

struct StreamAppChannel::mt_Detail {
    virtual ~mt_Detail()
    {
    }

    InputStream *m_pInput;
    OutputStream *m_pOutput;
    IStreamProcessor *m_pProc;
    DynamicArray<InputStream *> *m_pInputs;
    DynamicArray<OutputStream *> *m_pOutputs;
    DynamicArray<IStreamProcessor *> *m_pProcs;
};

struct StreamAppChannel::mt_OwnedDetail : public StreamAppChannel::mt_Detail {
    mt_OwnedDetail(const bool ac_bSerialOutput) : m_Outputs(ac_bSerialOutput)
    {
        m_pInput = &m_Inputs;
        m_pOutput = &m_Outputs;
        m_pProc = &m_Procs;
        m_pInputs = &m_Inputs;
        m_pOutputs = &m_Outputs;
        m_pProcs = &m_Procs;
    }

    ~mt_OwnedDetail()
    {
    }

    MultiInputStream<true> m_Inputs;
    MultiOutputStream<true> m_Outputs;
    SequentialProcessor m_Procs;
};

struct StreamAppChannel::mt_NotOwnedDetail
    : public StreamAppChannel::mt_Detail {
    mt_NotOwnedDetail(const bool ac_bSerialOutput) : m_Outputs(ac_bSerialOutput)
    {
        m_pInput = &m_Inputs;
        m_pOutput = &m_Outputs;
        m_pProc = &m_Procs;
        m_pInputs = &m_Inputs;
        m_pOutputs = &m_Outputs;
        m_pProcs = &m_Procs;
    }

    ~mt_NotOwnedDetail()
    {
    }

    MultiInputStream<false> m_Inputs;
    MultiOutputStream<false> m_Outputs;
    SequentialProcessor m_Procs;
};

StreamAppChannel::StreamAppChannel(const bool ac_bDeleteContent,
                                   const bool ac_bSerialOutput)
{
    if (ac_bDeleteContent)
        m_pDetail = new mt_OwnedDetail(ac_bSerialOutput);
    else
        m_pDetail = new mt_NotOwnedDetail(ac_bSerialOutput);

    m_Channel.mp_SetProcessor(m_pDetail->m_pProc);
    m_Channel.mp_SetSource(m_pDetail->m_pInput);
    m_Channel.mp_SetSink(m_pDetail->m_pOutput);
}

StreamAppChannel::~StreamAppChannel()
{
    delete m_pDetail;
}

void StreamAppChannel::mp_AddInputStream(InputStream *const a_Source)
{
    m_pDetail->m_pInputs->mp_AddItem(a_Source);
}

void StreamAppChannel::mp_AddOutputStream(OutputStream *const a_Sink)
{
    m_pDetail->m_pOutputs->mp_AddItem(a_Sink);
}

void StreamAppChannel::mp_AddProcessor(IStreamProcessor *const a_Proc)
{
    m_pDetail->m_pProcs->mp_AddItem(a_Proc);
}

void StreamAppChannel::mp_RemoveInput(const unsigned ac_nIndex)
{
    m_pDetail->m_pInputs->mp_RemoveItemAt(ac_nIndex);
}

void StreamAppChannel::mp_RemoveOutput(const unsigned ac_nIndex)
{
    m_pDetail->m_pOutputs->mp_RemoveItemAt(ac_nIndex);
}

void StreamAppChannel::mp_RemoveProcessor(const unsigned /*ac_nIndex*/)
{
    Q_ASSERT(0);
    // m_Processors.mp_RemoveItem( ac_nIndex ); //FIXME ??
}

INLINE const unsigned &StreamAppChannel::mf_nGetNumInputs() const
{
    return m_pDetail->m_pInputs->mf_nGetNumItems();
}

INLINE const unsigned &StreamAppChannel::mf_nGetNumOutputs() const
{
    return m_pDetail->m_pOutputs->mf_nGetNumItems();
}

INLINE const unsigned &StreamAppChannel::mf_nGetNumProcessors() const
{
    return m_pDetail->m_pProcs->mf_nGetNumItems();
}

bool StreamAppChannel::mf_bCheckValidConfig() const
{
    const unsigned c_nStrIchan = m_pDetail->m_pInput->mf_nGetNumChannels();
    const unsigned c_nStrOchan = m_pDetail->m_pOutput->mf_nGetNumChannels();

    if (!c_nStrIchan || !c_nStrOchan)
        return false;

    if (m_pDetail->m_pProc->mf_nGetNumInputChannels()) {
        if ((c_nStrIchan >= m_pDetail->m_pProc->mf_nGetNumInputChannels()) &&
            (m_pDetail->m_pProc->mf_nGetNumOutputChannels() >= c_nStrOchan))
            return true;
    } else {
        if (c_nStrIchan >= c_nStrOchan)
            return true;
    }
    return false;
}
