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

#ifndef _APEX_SRC_LIB_STREAMAPP_EOFCHECK_H__
#define _APEX_SRC_LIB_STREAMAPP_EOFCHECK_H__

#include "callback/callback.h"

#include "containers/dynarray.h"

#include "stream.h"

#include <QtGlobal>

namespace streamapp
{

/**
  * IEofCheck
  *   interface for classes that can check when end of file is reached.
  *   The installed callback will be called when all the check condition
  *   is met. This is eg when all files are read completely, or when
  *   one file is read completely.
  *   The installed callback is only called once, or again
  *   after adding an input or installing new callback,
  *   or after a call to mp_ResetState().
  *   Derived from Callback because this is the simplest class; if other
  *   means are needed, install one of the wrappers in appcore
  *   ( CallbackEventPoster, WaitObjectCallback ).
  ********************************************************************** */
class IEofCheck : public Callback
{
protected:
    /**
      * Protected Constructor.
      */
    IEofCheck()
    {
    }

public:
    /**
      * Destructor.
      */
    virtual ~IEofCheck()
    {
    }

    /**
      * Add an input to check.
      * @param a_cpInput the InputStream
      */
    virtual void mp_AddInputToCheck(InputStream &a_cpInput) = 0;

    /**
      * Remove all inputs and reset state.
      * @see mp_ResetState()
      */
    virtual void mp_RemoveAllInputs() = 0;

    /**
      * Reset the state.
      * When the check condition is met, the installed callback
      * is called only once.
      * After using this method, it will be called again once.
      * @see mp_InstallCallback()
      */
    virtual void mp_ResetState() = 0;

    /**
      * Install the Callback to call when check condition is met.
      * @param a_EofCallback the callback
      */
    virtual void mp_InstallCallback(Callback &a_EofCallback) = 0;

    /**
      * This method must be implemented to do the actual check.
      */
    virtual void mp_Callback() = 0;

    /**
      * Check if the condition was met.
      * @return true if so
      */
    virtual bool mf_bEof() const = 0;

    /**
      * Implementation of the Callback method.
      */
    void mf_Callback()
    {
        mp_Callback();
    }
};

/**
  * EofCheck
  *   IEofCheck implementation for for PositionableInputStream.
  *   Check condition is true when all inputs have 0 samples left.
  **************************************************************** */
class EofCheck : public IEofCheck
{
public:
    /**
      * Constructor.
      */
    EofCheck();

    /**
      * Destructor.
      */
    ~EofCheck();

    /**
      * Implementation of the IEofCheck method.
      * DO NOT CALL THIS it will assert. This class is for
      * PositionableInputStream only.
      * FIXME can't this be solved more nicely?
      */
    void mp_AddInputToCheck(InputStream & /*ac_Input*/)
    {
        Q_ASSERT(0 && "virtual function not implemented");
    }

    /**
      * Add an input to check.
      * @param a_Input the input stream.
      */
    void mp_AddInputToCheck(PositionableInputStream &a_Input);

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_RemoveAllInputs();

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_ResetState();

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_InstallCallback(Callback &a_EofCallback);

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_Callback();

    /**
      * Implementation of the IEofCheck method.
      */
    INLINE bool mf_bEof() const
    {
        return mv_nCurSamplesLeft == 0;
    }

    /**
      * Get number of samples left of longest input added.
      * Gets updated every mf_Callback() call.
      * @return the number of samples left currently
      */
    INLINE unsigned long mf_lSamplesLeft() const
    {
        return mv_nCurSamplesLeft;
    }

    /**
      * Get the length of the longest input added.
      * @return the number of samples
      */
    INLINE unsigned long mf_lTotalSamples() const
    {
        return mv_nTotalSamples;
    }

    /**
      * Call mp_SeekPosition on all inputs added.
      * @param ac_lPosition the new position, must be in range
      */
    void mp_SeekAll(const unsigned long ac_lPosition);

    /**
      * Dynamic array of PositionableInputStream pointers
      */
    typedef DynamicArray<PositionableInputStream *> mt_SourceList;

private:
    unsigned long mv_nTotalSamples;
    unsigned long mv_nCurSamplesLeft;
    bool mv_bFired;
    mt_SourceList m_List;
    Callback *m_pAllDoneCallback;

    EofCheck(const EofCheck &);
    EofCheck &operator=(const EofCheck &);
};

/**
  * SimpleEofCheck
  *   class to check for any InputStream's eof.
  *   Check condition can be set, see mp_SetSingleEof()
  ***************************************************** */
class SimpleEofCheck : public IEofCheck
{
public:
    /**
      * Constructor.
      */
    SimpleEofCheck();

    /**
      * Destructor.
      */
    ~SimpleEofCheck();

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_AddInputToCheck(InputStream &a_cpInput);

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_RemoveAllInputs();

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_ResetState();

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_InstallCallback(Callback &a_EofCallback);

    /**
      * Implementation of the IEofCheck method.
      */
    void mp_Callback();

    /**
      * Implementation of the IEofCheck method.
      */
    INLINE bool mf_bEof() const
    {
        return mv_bEof;
    }

    /**
      * Set check condition type.
      * If set, mv_bEof is true if only one of the streams is eof,
      * else all of the streams have to be ended.
      * @param ac_bSet true for single
      */
    INLINE void mp_SetSingleEof(const bool ac_bSet = true)
    {
        mv_bSingle = ac_bSet;
    }

    /**
      * Dynamic array of InputStream pointers.
      */
    typedef DynamicArray<InputStream *> mt_SourceList;

private:
    bool mv_bEof;
    bool mv_bFired;
    bool mv_bSingle;
    mt_SourceList m_List;
    Callback *m_pAllDoneCallback;

    SimpleEofCheck(const SimpleEofCheck &);
    SimpleEofCheck &operator=(const SimpleEofCheck &);
};

/**
  * EofCheckFactory
  *   creates one of the IEofChecks.
  *   Doesn't delete it however!
  ********************************** */
class EofCheckFactory
{
private:
    /**
      * Private Constructor.
      */
    EofCheckFactory()
    {
    }
    /**
      * Private Destructor.
      */
    ~EofCheckFactory()
    {
    }

public:
    /**
      * The types known.
      */
    enum mt_eCheckType {
        mv_eSimple,      //!< for a SimpleEofCheck
        mv_ePositionable //!< for an EofCheck
    };

    /**
      * Create one.
      * @param ac_eType the type
      * @return the new'd IEofCheck
      */
    static IEofCheck *sf_CreateCheck(const mt_eCheckType ac_eType)
    {
        if (ac_eType == mv_eSimple)
            return new SimpleEofCheck();
        else
            return new EofCheck();
    }
};
}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_EOFCHECK_H__
