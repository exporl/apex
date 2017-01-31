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
 
#ifndef __MATRIX_H__
#define __MATRIX_H__

namespace streamapp
{

    /**
      * IMatrix
      *   interface for simple matrix operations
      *   the implementation supplying the Get and Set functions gets
      *   access to some basic functionality
      *************************************************************** */
  template<class tType>
  class IMatrix
  {
  protected:
    IMatrix()
    {}
  public:
    virtual ~IMatrix()
    {}

      /**
        * Set a specified node
        * @param ac_tValue the value
        * @param ac_nRow the row
        * @param ac_nCol the column
        */
    virtual void mp_Set( const unsigned ac_nRow, const unsigned ac_nCol, const tType& ac_tValue ) = 0;

      /**
        * Get a specified node
        * @param ac_nRow the row
        * @param ac_nCol the column
        * @return the value
        */
    virtual const tType& mf_Get( const unsigned ac_nRow, const unsigned ac_nCol ) = 0;

     /**
        * Number of Rows
        * @return number
        */
    virtual void mf_nGetNumRows() const = 0;

     /**
        * Number of Columns
        * @return number
        */
    virtual void mf_nGetNumCols() const = 0;

      /**
        * Sets all Nodes
        * @param ac_tValue the value
        */
    virtual void mp_SetAll( const tType& ac_tValue )
    {
      const unsigned c_nCols = mf_nGetNumCols();
      const unsigned c_nRows = mf_nGetNumRows();
      for( unsigned i = 0 ; i < c_nRows ; ++i )
        for( unsigned j = 0 ; j < c_nCols ; ++j )
        mp_Set( i, j, ac_tValue );
    }

      /**
        * Set a Diagonal
        *   only complete for square matrices
        * @param ac_tValue the value
        */
    virtual void mp_SetDiagonal( const tType& ac_tValue )
    {
      const unsigned c_nTemp = s_min( mf_nGetNumRows(), mf_nGetNumCols() );
      for( unsigned i = 0 ; i < c_nTemp ; ++i )
        mp_Set( i, i, ac_tValue );
    }

      /**
        * Set a whole Row
        * @param ac_tValue the value
        * @param ac_nRow the row to set
        */
    virtual void mp_SetRow( const unsigned ac_nRow, const tType& ac_tValue )
    {
      const unsigned c_nCols = mf_nGetNumCols();
      assert( ac_nRow < mf_nGetNumRows() );
      for( unsigned i = 0 ; i < c_nCols ; ++i )
        mp_Set( ac_nRow, i, ac_tValue );
    }

      /**
        * Set a whole Column
        * @param ac_tValue the value
        * @param ac_nCol the column to set
        */
    virtual void mp_SetCol( const unsigned ac_nCol, const tType& ac_tValue )
    {
      const unsigned c_nRows = mf_nGetNumRows();
      assert( ac_nCol < mf_nGetNumCols() );
      for( unsigned i = 0 ; i < c_nRows ; ++i )
        mp_Set( i, ac_nCol, ac_tValue );
    }

  };

}

#endif //#ifndef __IMATRIX_H__
