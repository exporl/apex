/*
	Part of the psignifit engine source distribution version 2.5.6.
	Copyright (c) J.Hill 1999-2005.
	mailto:psignifit@bootstrap-software.org
	http://bootstrap-software.org/psignifit/

	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation; either version 2 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place, Suite 330, Boston, MA  02111-1307  USA

	For more information, including the GNU General Public License, please read the
	document Legal.txt

*/
#ifndef __BATCHFILES_H__
#define __BATCHFILES_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	char *buffer;
	size_t length;
	size_t position;
	boolean disposeable;
} Batch;
typedef Batch * BatchPtr;

typedef enum {firstOccurrence, lastOccurrence, eachOccurrence, uniqueOccurrence} BatchFindMode;

BatchPtr BatchString(char * stringData, size_t length, boolean disposeable);
BatchPtr ConcatenateBatchStrings(BatchPtr first, BatchPtr second, boolean disposeFirst, boolean disposeSecond);
int DisposeBatch(BatchPtr b);
char * FindVariableInBatch(BatchPtr b, char *identifier, int *length, BatchFindMode mode);
char * GetVariableSpace(BatchPtr b, int *position, int *length);
boolean IdentifierAppearsInBatch(BatchPtr b, char * identifier);
boolean IsBatchFormat(char *s);
void JumpToPositionInBatch(BatchPtr b, size_t position);
BatchPtr LoadBatchFromFile(char * name, boolean generateErrorIfNotFound);
char * NextIdentifier(BatchPtr b, int *lengthPtr, char *buf, int bufSize, BatchFindMode mode);
boolean ReadBoolean(char *p, int inputLength, char *description);
double * ReadDoubles(char *p, int inputLength, double * outBuffer,
		unsigned int * nVals, unsigned int minNVals, unsigned int maxNVals, char *description);
double ReadScalar(char *p, int inputLength, char *description);
char * ReadString(char *p, int inputLength, char * buf, unsigned int * nChars);

#ifdef __cplusplus
}       // extern "C"
#endif


#endif
