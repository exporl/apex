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

#include "multicallback.h"

using namespace streamapp;

MultiCallback::MultiCallback(const bool ac_bDeleteContent)
    : mc_bDeleteContent(ac_bDeleteContent)
{
}

MultiCallback::~MultiCallback()
{
    if (mc_bDeleteContent) {
        for (unsigned i = 0; i < m_Callbacks.mf_nGetNumItems(); ++i)
            delete m_Callbacks(i);
    }
    m_Callbacks.mp_RemoveAllItems();
}

void MultiCallback::mp_AddItem(Callback *a_pCallback)
{
    m_Callbacks.mp_AddItem(a_pCallback);
}

void MultiCallback::mp_RemoveItem(Callback *a_pCallback)
{
    m_Callbacks.mp_RemoveItem(a_pCallback);
    if (mc_bDeleteContent)
        delete a_pCallback;
}

unsigned MultiCallback::mf_nGetNumItems() const
{
    return m_Callbacks.mf_nGetNumItems();
}

void MultiCallback::mf_Callback()
{
    const unsigned nItems = m_Callbacks.mf_nGetNumItems();
    for (unsigned i = 0; i < nItems; ++i)
        m_Callbacks.mf_GetItem(i)->mf_Callback();
}
