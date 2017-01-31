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
#ifndef __BATCHFILES_C__
#define __BATCHFILES_C__

#include "universalprefix.h"

#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "batchfiles.h"

#define NewLine(c)	((c)=='\n' || (c)=='\r')
int FindIdentifierInBatch(BatchPtr b, int p, char *identifier);
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
BatchPtr BatchString(char * stringData, size_t length, boolean disposeable)
{
	BatchPtr b;
	size_t i;
	
	b = New(Batch, 1);
	b->buffer = stringData;
	b->length = length;
	b->position = 0;
	b->disposeable = disposeable;
	
        // [Tom] Check if buffer is empty
	for(i = 0; i < b->length; i++) if(!isspace(b->buffer[i])) break;
	if(i == b->length) {
		DisposeBatch(b);
		return NULL;
	}
	
	return b;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
BatchPtr ConcatenateBatchStrings(BatchPtr first, BatchPtr second, boolean disposeFirst, boolean disposeSecond)
{
	BatchPtr b;
	b = New(Batch, 1);
	b->length = 0;
	b->position = 0;
	b->disposeable = TRUE;
	if(first && first->buffer) b->length += first->length;
	if(second && second->buffer) b->length += second->length;
	if(b->length == 0) {Destroy(b); return NULL;}
	b->buffer = New(char, b->length);
	if(first && first->buffer) memcpy(b->buffer, first->buffer, (b->position = first->length));
	if(second && second->buffer) memcpy(b->buffer + b->position, second->buffer, second->length);
	b->position = 0;
	
	if(first && disposeFirst) DisposeBatch(first);
	if(second && disposeSecond) DisposeBatch(second);
	
	return b;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
int DisposeBatch(BatchPtr b)
{
	if(b == NULL || b->buffer == NULL) return -1;
	if(b->disposeable) Destroy(b->buffer);
	Destroy(b);
	return 0;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
int FindIdentifierInBatch(BatchPtr b, int p, char * identifier)
{
	boolean newLine;
	int matched, len;
	char c;
	
	if(b == NULL || b->buffer == NULL) return EOF;
	
        if((unsigned)p >= b->length){JError("FindIdentifierInBatch(): start-point exceeds end of file"); return EOF;}
	
	matched = -1;
	len = strlen(identifier);
	newLine = TRUE;
        for(; (unsigned) p < b->length; p++) {
		c = b->buffer[p];
		if(matched<0 && c=='#' && newLine) matched=0;
		else if(matched>=0 && matched < len && toupper(c) == toupper(identifier[matched])) matched++;
		else if(matched>=len && isspace(c)) break;
		else matched = -1;
		if(!isspace(c)) newLine = FALSE;
		if(NewLine(c)) newLine = TRUE;
	}
	if(matched<len) return EOF;
	
	return p;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
char * FindVariableInBatch(BatchPtr b, char *identifier, int *length, BatchFindMode mode)
{
	int target, next;
	
	if((target = FindIdentifierInBatch(b, 0, identifier)) == EOF)
		{DisposeBatch(b); JError("could not find #%s in batch file", identifier); return NULL;}
	
        if(mode == uniqueOccurrence && target < (int) b->length && FindIdentifierInBatch(b, target, identifier) != EOF)
		{DisposeBatch(b); JError("multiple occurences of #%s in batch file", identifier); return NULL;}
	
	if(mode == lastOccurrence)
		while((next = FindIdentifierInBatch(b, target, identifier)) != EOF) target = next;
			
	return GetVariableSpace(b, &target, length);
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
char * GetVariableSpace(BatchPtr b, int *position, int *length)
{
	int p;
	char c;
	boolean newLine;
		
	*length = 0;
	if(*position == EOF) return NULL;
	newLine = FALSE;
        for(p = *position; p < (int) b->length; p++) {
		c = b->buffer[p];
		if(newLine && c=='#') break;
		if(!isspace(c)) newLine = FALSE;
		if(NewLine(c)) newLine = TRUE;
		if(*length == 0 && !isspace(c)) *position = p;
		if(*length > 0 || !isspace(c)) (*length)++;
	}
	while(*length && isspace(b->buffer[*position + *length - 1])) (*length)--;
	return b->buffer + *position;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
boolean IdentifierAppearsInBatch(BatchPtr b, char * identifier)
{
	return (boolean)(FindIdentifierInBatch(b, 0, identifier) != EOF);
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
boolean IsBatchFormat(char *s)
{
	while(*s && isspace(*s)) s++;
	return (*s == '#');
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
void JumpToPositionInBatch(BatchPtr b, size_t position)
{
	if(b == NULL) return;
        if(position >= b->length) Bug("attempt to jump outside of allocated area for batch");
	b->position = position;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
BatchPtr LoadBatchFromFile(char * name, boolean generateErrorIfNotFound)
{
	FILE * stream;
	size_t i, bufSize, bufIncrement = 1024, maxBufSize = 8192;
	char c;
	BatchPtr b;

	b = New(Batch, 1);
	bufSize = bufIncrement;
	b->buffer = New(char, (bufSize = bufIncrement));

	
	if((stream = fopen(name, "r"))==NULL) {
		if(strcmp(name, "stdin") == 0 || strcmp(name, "-") == 0) stream = stdin;
		else {
			Destroy(b->buffer); Destroy(b);
			if(generateErrorIfNotFound) JError("failed to open \"%s\"", name);
			return NULL;
		}
	}
	b->length = 0;
		
	while((c=fgetc(stream))!=EOF) {
		if(b->length >= bufSize) {
			if((bufSize += bufIncrement) > maxBufSize) {
				if(stream != stdin) fclose(stream);
				Destroy(b->buffer); Destroy(b);
				JError("input stream is too big");
				return NULL;
			}
			b->buffer = ResizeBlock(b->buffer, bufSize);
		}
		b->buffer[b->length++] = c;
	}
	for(i = 0; i < b->length; i++) if(!isspace(b->buffer[i]))break;
	if(i == b->length) { /* catches the cases in which there are no characters, or all whitespace */
		Destroy(b->buffer);
		Destroy(b);
		b = NULL;
	}
	else {
		b->buffer = ResizeBlock(b->buffer, b->length);
		b->position = 0;
		b->disposeable = TRUE;
	}

	if(stream != stdin) fclose(stream);
	
	return b;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
char * NextIdentifier(BatchPtr b, int *lengthPtr, char *buf, int bufSize, BatchFindMode mode)
{
	int p, i;
	boolean legal;
	char * returnVal;
	
	if(b == NULL || b->buffer == NULL) return NULL;
	if(buf == NULL || bufSize < 2) {JError("NextIdentifier(): buffer cannot be NULL, and buffer size must be at least 2"); return NULL;}
//	if(b->position < 0) b->position = 0;
	if(b->position >= b->length) return NULL;

	legal = TRUE;
	for(p = b->position; p > 0; p--) {
		if(NewLine(b->buffer[p-1])) break;
		if(!isspace(b->buffer[p-1])) {legal = FALSE; break;}
	}
	while(b->position < b->length - 1) {
		if(NewLine(b->buffer[b->position])) legal = TRUE;
		if(legal && b->buffer[b->position] == '#' && !isspace(b->buffer[(b->position)+1])) break;
		if(!isspace(b->buffer[b->position])) legal = FALSE;
		(b->position)++;
	}
	if(b->position >= b->length - 1) {
		buf[0] = 0; 
		return NULL;
	}
	
	i = 0;
	b->position++;
	while(b->position < b->length && !isspace(b->buffer[b->position]) && i < bufSize - 1) {
		buf[i++] = toupper(b->buffer[b->position++]);
	} 
	buf[i] = 0;

	returnVal = GetVariableSpace(b, (p = b->position, &p), lengthPtr);
	
        if((mode == firstOccurrence && FindIdentifierInBatch(b, 0, buf) != (int) b->position) ||
	   (mode == lastOccurrence && FindIdentifierInBatch(b, b->position, buf) != EOF))
		returnVal = NextIdentifier(b, lengthPtr, buf, bufSize, mode);
        if(mode == uniqueOccurrence && (FindIdentifierInBatch(b, 0, buf) != (int) b->position ||
								    FindIdentifierInBatch(b, b->position, buf) != EOF)) {
		DisposeBatch(b);
		JError("found multiple occurrences of \"%s\" in batch file", buf);
		return NULL;
	}

	return returnVal;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
boolean ReadBoolean(char *p, int inputLength, char *description)
{
	int i;
	char s[6];
	
	if(p == NULL) return FALSE;
	for(i = 0; i < inputLength && i < 5; i++) s[i] = toupper(p[i]);
	s[i] = 0;
	
	if(strcmp(s, "TRUE")==0 || strcmp(s, "1")==0) return TRUE;
	if(strcmp(s, "FALSE")==0 || strcmp(s, "0")==0) return FALSE;

	JError("Batch file error:\n%s must be \"TRUE\" (or 1) or \"FALSE\" (or 0) - found illegal entry \"%s\"", description, s);
	return FALSE;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
double * ReadDoubles(char *p, int inputLength, double * outBuffer,
			unsigned int * nVals, unsigned int minNVals, unsigned int maxNVals, char *description)
{
    double x, firstVal = 0.0;
	char *start, *end;
	unsigned int localNVals;
	
	if(p == NULL) return NULL;
	
	if(p[0] == '[' || p[0] == '(' || p[0] == '{') p++, inputLength--;
	if(p[inputLength-1] == ']' || p[inputLength-1] == ')' || p[inputLength-1] == '}') inputLength--;
	
	start = p;
	end = start + inputLength - 1;
	if(nVals == NULL) nVals = &localNVals;
	*nVals = 0;
	
	errno = 0;
	while(start <= end) {
	 	x = improved_strtod(start, &start);
		if(++(*nVals) == 1) firstVal = x;
		if(errno) break;
	}
	if(errno && start <= end)
		{JError("Bad numeric format in entry #%d of %s", *nVals, description); return NULL;}

	if(minNVals > maxNVals) {Bug("ReadDoublesFromBatch(): minNVals > maxNVals"); return NULL;}
	if(maxNVals > 0 && (*nVals < minNVals || *nVals > maxNVals)) {
		if(minNVals == maxNVals)
			JError("%s should contain %d numeric value%s", description, minNVals, (minNVals == 1) ? "" : "s");
		else
			JError("%s should contain between %u and %u values", description, minNVals, maxNVals);
		return NULL;
	}
	if(outBuffer==NULL) {
		if(*nVals < 1) return NULL;
		outBuffer = New(double, *nVals);
	}
	else if(maxNVals == 0)
		{Bug("ReadDoublesFromBatch(): if buffer is pre-allocated, size must be given in maxNVals"); return NULL;}

	start = p;
        for(inputLength = 0; inputLength < (int) *nVals; inputLength++)
		outBuffer[inputLength] = improved_strtod(start, &start);
	return outBuffer;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
double ReadScalar(char *p, int inputLength, char *description)
{
	unsigned int nVals;
	double val;
	
	ReadDoubles(p, inputLength, &val, &nVals, 1, 1, description);

	return val;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
char * ReadString(char *p, int inputLength, char * buf, unsigned int * siz)
/*	if buf is NULL on entry, a buffer is created and returned
	on entry, *siz is the size of the available buffer (including null termination) or the
		maximum buffer size desired if a new buffer is to be created (in this case passing
		siz = NULL or *siz = 0 allows free rein)
	on exit, *siz is the number of characters available in that field of the batch file (though
 		the returned buffer may be smaller than that if limited by the input value of *siz
*/
{
	size_t outBufSize;

	if(p == NULL) return NULL;

        outBufSize = ((siz != NULL && *siz > 0 && (int) *siz < inputLength+1) ? (int) *siz : inputLength+1);
	if(buf==NULL) buf = New(char, outBufSize);
	if(siz != NULL) *siz = inputLength;

        if(inputLength > (int) outBufSize - 1) inputLength = outBufSize - 1;
	memcpy(buf, p, inputLength);
	buf[inputLength] = 0;
	
	return buf;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/

#endif
