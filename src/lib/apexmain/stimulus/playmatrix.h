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

#include "apextools/apextypedefs.h"

#include "streamapptypedefs.h"

namespace apex
{
namespace data
{
class ConnectionsData;
struct StimulusDatablocksContainer;
}
namespace stimulus
{

class tConnectionsMap;

/**
  * PlayMatrixCreator
  *   creates a playmatrix given a 'datablocks' node.
  *   A playmatrix is a matrix specifying which datablock
  *   should be played when. Datablcoks are referred to by their ID
  *   All subsequent data in a row have to be played right after
  *   each other without any gaps, at the same time with the data of other rows.
  *   A matrix node with an empty ID means nothing should be played.
  *   All matrices returned here must be deleted by the callee.
  ******************************************************************************
  */
class PlayMatrixCreator
{
public:
    /**
      * Constructor.
      */
    PlayMatrixCreator();

    /**
      * Destructor.
      */
    ~PlayMatrixCreator();

    /**
      * This will create the actual matrix
      * given a datablocks container.
      * @param data a StimulusDatablocksContainer
      */
    PlayMatrix *createMatrix(const data::StimulusDatablocksContainer &data);

    /**
      * Displays the matrix using std::printf.
      * @param ac_pMat the matrix to display.
      */
    static void sf_DoDisplay(const PlayMatrix *ac_pMat);

    /**
      * Remove duplicates between rows.
      * A single DataBlock can not be used twice at the same time.
      * This method removes all duplicates, creating a new DataBlock with a _n
     * postfix,
      * where n is the number of occurrences, and adding copies of connections.
      * @param a_pMatrix the matrix to check
      * @param a_DataBlocks the DataBlock map used by the experiment
      * @param a_Connections the connections used by the experiment
      */
    static void sf_FixDuplicateIDs(PlayMatrix *a_pMatrix,
                                   tDataBlockMap &a_DataBlocks,
                                   data::ConnectionsData &a_Connections);

    /**
      * Check if a matrix is valid.
      * A matrix is valid when all datablocks on one row are for the same
     * device.
      * @param a_pMatrix the matrix to check
      * @param a_DataBlocks the DataBlock map used by the experiment
      * @param a_sError string receiving an error if return value is false
      * @return true if check succeeds
      */
    static bool sf_bCheckMatrixValid(const PlayMatrix *a_pMatrix,
                                     const tDataBlockMap &a_DataBlocks,
                                     QString &a_sError);

    /**
      * Create a matrix containing only datablocks for the given device.
      * Nodes containing datablocks for other devices will be set to an empty
     * string.
      * @param a_pMatrix the matrix to check
      * @param a_DataBlocks the DataBlock map used by the experiment
      * @param ac_sDevice the device to create a matrix for
      * @return new'd PlayMatrix
      */
    static PlayMatrix *sf_pCreateSubMatrix(const PlayMatrix *a_pMatrix,
                                           const tDataBlockMap &a_DataBlocks,
                                           const QString &ac_sDevice);

    /**
      * Shrink a matrix.
      * The MatrixStorage class doesn't use dynamic resizing,
      * so all methods creating a matrix start with a 16*16 sized one containing
     * all empty strings.
      * This method returns a new matrix that has the given size.
      * @param ac_pSrc the original matrix
      * @param ac_nSim the new number of rows
      * @param ac_nSeq the new number of columns
      * @return a new ac_nSim * ac_nSeq matrix
      */
    static PlayMatrix *sf_pShrinkedMatrix(const PlayMatrix *ac_pSrc,
                                          const unsigned ac_nSim,
                                          const unsigned ac_nSeq);

    /**
      * Convert PlayMatrix to DataBlockMatrix.
      * @param ac_pSrc the matrix with IDs
      * @param a_DataBlocks the map with DataBlocks
      * @return a matrix with DataBlock pointers
      */
    static DataBlockMatrix *sf_pConvert(const PlayMatrix *ac_pSrc,
                                        const tDataBlockMap &a_DataBlocks);

private:
    /**
      * Recursive parsing method.
      */
    void parseDatablocks(const QString &ac_sPrevTag,
                         const data::StimulusDatablocksContainer &data,
                         unsigned &a_nSeq, unsigned &a_nSim);

    PlayMatrix m_WorkHere;
    unsigned m_nSeq;
    unsigned m_nSim;
    unsigned m_nSimRec;
};
}
} // end namespace apex
