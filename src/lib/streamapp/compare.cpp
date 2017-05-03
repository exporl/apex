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

#include "containers/matrix.h"

#include "file/wavefile.h"

#include "compare.h"

using namespace streamapp;

Compare::mt_eResult Compare::sf_eCompare(AudioFormatReader& a_Input0, AudioFormatReader& a_Input1) throw()
{
  const unsigned nChan = a_Input0.mf_nChannels();
  if(nChan != a_Input1.mf_nChannels())
    return mc_eNotIdentical;
  const AudioFormat::mt_eBitMode eMode = a_Input0.mf_eBitMode();
  if(eMode != a_Input1.mf_eBitMode())
    return mc_eNotIdentical;

  const unsigned nBytesPerSample = eMode == AudioFormat::MSBfloat64 ? 8 : 4;
  const unsigned nBytes = nBytesPerSample * sc_nReadSize;

  MatrixStorage< char > buf0(nChan, nBytes);
  MatrixStorage< char > buf1(nChan, nBytes);

  unsigned nRead = sc_nReadSize;
  while (nRead == sc_nReadSize) {
    nRead = a_Input0.Read((void**) buf0.mf_pGetArray(), nRead);
    if (a_Input1.Read((void**) buf1.mf_pGetArray(), nRead) != nRead)
      return mc_eNotIdentical;
    for (unsigned i = 0 ; i < nChan ; ++i) {
      if (memcmp(buf0.mf_pGetArray()[ i ], buf1.mf_pGetArray()[ i ], nRead * nBytesPerSample))
        return mc_eNotIdentical;
    }
  }
  return mc_eIdentical;
}

Compare::mt_eResult Compare::sf_eCompare(Compare::mt_Readers& a_Input0, AudioFormatReader& a_Input1) throw()
{
  const unsigned nItems = a_Input0.mf_nGetBufferSize();
  if (!nItems)
    return mc_eNotIdentical;
  for (unsigned i = 0; i < nItems ; ++i) {
    if (sf_eCompare(*a_Input0(i), a_Input1) != mc_eIdentical)
      return mc_eNotIdentical;
  }
  return mc_eIdentical;
}

Compare::mt_eResult Compare::sf_eCompareWave(const QString &ac_sFile0, const QString &ac_sFile1) throw()
{
  try {
    InputWaveFile fin0;
    if (!fin0.mp_bOpen(ac_sFile0))
      return mc_eFailedOpen;
    InputWaveFile fin1;
    if (! fin1.mp_bOpen(ac_sFile1))
      return mc_eFailedOpen;
    return sf_eCompare(fin0, fin1);
  } catch (...) {
    return mc_eFailedOpen;
  }
}

Compare::mt_eResult Compare::sf_eCompareWave(const Compare::mt_Files& ac_sFile0, const QString &ac_sFile1) throw()
{
  try {
    mt_eResult bRet = mc_eFailedOpen;
    const unsigned nItems = ac_sFile0.mf_nGetBufferSize();
    if (!nItems)
      return mc_eNotIdentical;

    InputWaveFile fin1;
    ArrayStorage<AudioFormatReader*> ar(nItems, true);

    for (unsigned i = 0 ; i < nItems ; ++i) {
      InputWaveFile* fin = new InputWaveFile();
      if (!fin->mp_bOpen(ac_sFile0(i)))
        goto error;
      ar.mp_Set(i, fin);
    }

    if (!fin1.mp_bOpen(ac_sFile1))
      goto error;

     bRet = sf_eCompare(ar, fin1);

  error:
    for (unsigned i = 0 ; i < nItems ; ++i)
      delete ar(i);
    return bRet;
  } catch (...) {
    return mc_eFailedOpen;
  }
}
