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
 
#ifndef __INIFILE_H__
#define __INIFILE_H__

#if QT_VERSION < 0x040000
#include <qstring.h>
#else
#include <QString>
#endif
#include <vector>

namespace appcore
{

    /**
      * CIniFile
      *   simple *.ini file functionality.
      *   Supports strings and numbers.
      *   Class is quite self-explanatory,
      *   use it once and you know how it works ;-]
      ********************************************* */
  class CIniFile
  {
    public:

        /**
          * CValueSection
          *   encapsulates a value.
          *   Consists of name and value.
          ******************************* */
      struct CValueSection
      {
        QString   mv_sName;
        QString   mv_sValue;
      };

        /**
          * Vector of CValueSection.
          */
      typedef std::vector<CValueSection> CValueVector;

        /**
          * CKeySection
          *   encapsulates a key.
          *   Consists of name and value list.
          ************************************ */
      struct CKeySection
      {
        QString         mv_sKeyName;
        CValueVector    mv_Values;
      };

        /**
          * Vector of CKeySection
          */
      typedef std::vector<CKeySection> CKeyVector;

      enum  mt_eStatus { mc_eOK, mc_eErrorValueNoExist, mc_eErrorKeyNoExist, mc_eFileOpenError, mc_eIndexOutOfRange };

      CIniFile();
      virtual ~CIniFile();

      bool                mp_AddKey( const QString & ac_sKey );      //adds a key if does not exist, returns true if existed
      void                mp_RemoveKey( const QString & ac_sKey );   //removes the key and all its values

        //if nocreate == true, the value will not be created if it does not exist
        //if the key does not exist the status will be mc_eErrorValueNoExist
      void                mp_SetValueString(const QString & ac_sKey, const QString & ac_sValueName, const QString & ac_sValue, const bool  ac_bNoCreate = false );
      void                mp_SetValueBool(  const QString & ac_sKey, const QString & ac_sValueName, const bool      ac_bValue, const bool  ac_bNoCreate = false );
      void                mp_SetValueFloat( const QString & ac_sKey, const QString & ac_sValueName, const double    ac_fValue, const bool  ac_bNoCreate = false );
      void                mp_SetValueInt(   const QString & ac_sKey, const QString & ac_sValueName, const int       ac_iValue, const bool  ac_bNoCreate = false );

        //
      void                mp_RemoveValue( const QString & ac_sKey, const QString & ac_sValueName );

        //
      void                mp_Clear();   //erase all keys everything.

        //if key or value does not exist --> return value is undefined
      QString             mf_sGetValueString( const QString & ac_sKey, const QString & ac_sValueName ) const;
      bool                mf_bGetValueBool(   const QString & ac_sKey, const QString & ac_sValueName ) const;
      double              mf_fGetValueFloat(  const QString & ac_sKey, const QString & ac_sValueName ) const;
      int                 mf_iGetValueInt(    const QString & ac_sKey, const QString & ac_sValueName ) const;

        //
      void                mp_CommitToFile() const;
      void                mp_LoadFromFile();

      mt_eStatus          mf_eGetStatus() const;

        //
      unsigned            mf_nGetKeyCount() const;
      unsigned            mf_nGetValueCount( const QString & ac_sKey ) const;    //result is undefined if key does not exist
      unsigned            mf_nGetTotalValueCount() const;

      QString             mf_sGetKey( const unsigned i ) const;   //get key based on index ( useful for iteration )

      void                mp_SetFilename( const QString & ac_sFilename );
      const QString&      mf_sGetFilename() const;

        //creates a new inifile that does not contain any of the matching values of the matching keys
        //if keys are empty they are removed unless ac_bRemoveEmptyKeys == false
      CIniFile            mf_CalcDiff( const CIniFile & ac_IniFile, const bool  ac_bRemoveEmptyKeys = true ) const;
        //merges values of matching keys
      //CIniFile          mf_Merge() const;

      bool                operator==( const CIniFile & ac_IniFile ) const;

        /**
          * Convert eols for strings.
          */
      void                mp_SetConvertEol( const bool ac_bSet = true )
      { mv_bConvertEol = ac_bSet; }

    private:

      //CIniFile          mf_Copy() const;    //make a deepcopy of this

      void                mp_SetStatus( const mt_eStatus  ac_eStatus ) const;

      QString             mv_sFilename;

      CKeyVector          mv_Keys;

      mutable mt_eStatus  mv_eStatus;

      bool                mv_bConvertEol;
      unsigned            mv_nTotalValueCount;

      CKeyVector::iterator        mf_LookupKey( const QString & ac_sKey );
      CKeyVector::const_iterator  mf_LookupKey( const QString & ac_sKey ) const;

  };

}

#endif //__INIFILE_H__
