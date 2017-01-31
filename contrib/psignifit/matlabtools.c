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
#ifndef __MATLABTOOLS_C__
#define __MATLABTOOLS_C__

#include "universalprefix.h"

#ifdef MATLAB_MEX_FILE

#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "matlabtools.h"
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

mxArray **gArgoutList;
int gMaxNargOut, *gArgoutCounterPtr; 

#define kLastErrBufferSize 64
char gLastErrBuffer[kLastErrBufferSize];

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void * mxArg(mxArray * argArray[], unsigned short nArgs, unsigned short argNumber, boolean input)
{
	if(argNumber < 1 || argNumber > nArgs)
		Bug("illegal reference to non-existent %s argument #%hu", (input?"input":"output"), argNumber);
	return (input ? (void*)(argArray[argNumber-1]) : (void*)(argArray + argNumber -1));
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean mexAddArrayToOutputList(mxArray *array, unsigned short position)
{
	if(gArgoutCounterPtr == NULL || gArgoutList == NULL)
		Bug("mexAddArrayToOutputList() called without first calling mexInitOutputList()");
	if(array == NULL)
		Bug("mexAddArrayToOutputList() called with NULL array pointer");
	if(position == 0 || position > gMaxNargOut) return FALSE;
	if(*gArgoutCounterPtr < position) *gArgoutCounterPtr = position;
	position--;
	if(gArgoutList[position]) mxDestroyArray(gArgoutList[position]);
	gArgoutList[position] = array;
	return TRUE;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int mexAssignArray(mxArray *array, char *name)
{
	char *s;
	int result;
	
	if(name == NULL || strlen(name) == 0)
		Bug("failed to assign an array in the caller workspace, because no name was supplied");
	if(strlen(name) > mxMAXNAM - 1) {
		JWarning("could not assign array '%s': name too long", name);
		return -1;
	}
	for(s = name; *s; s++)
		if((!isalnum(*s) && *s != '_') || (s == name && !isalpha(*s))) {
			JWarning("could not assign array '%s': illegal name string", name);
			return -1;
		}
	if(array == NULL) {
		JWarning("no new content was available for the requested assignment to '%s'");
		return -1;
	}
#if defined V4_COMPAT || defined V5_COMPAT
	mxSetName(array, name);
	result = mexPutArray(array, "caller");
#else
	result = mexPutVariable("caller", name, array);
#endif
	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int mexEvalf(char * fmt, ...)
{
	char temp[256];
	va_list ap;
	int result;
	
	va_start(ap, fmt);
	vsprintf(temp, fmt, ap);
	va_end(ap);
	result = mexEvalString(temp);
	if(result != 0 && mexLastErr(FALSE) == NULL) JError("aborted by user");
	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
unsigned short mexGetNextOutputPosition(void)
{
	if(gArgoutCounterPtr == NULL || gArgoutList == NULL)
		Bug("mexGetNextOutputPosition() called without first calling mexInitOutputList()");
	return (*gArgoutCounterPtr) + 1;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void mexInitOutputList(int maxNargOut, int * ptrToCounter, mxArray **arrayHandle)
{
	int i;
	
	if(ptrToCounter == NULL || arrayHandle == NULL)
		Bug("mexInitOutputList() called with NULL pointer");
	
	gArgoutList = arrayHandle;
	gMaxNargOut = maxNargOut;
	*(gArgoutCounterPtr = ptrToCounter) = 0;

	for(i = 0; i < gMaxNargOut; i++) gArgoutList[i] = mxCreateDoubleMatrix(0, 0, mxREAL);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
char * mexLastErr(boolean clear)
{
	mxArray *in[1], *out[1];
	if(mexCallMATLAB(1, out, 0, in, "lasterr") != 0) Bug("mexLastErr() failed (mexCallMATLAB)");
	if(mxGetString(*out, gLastErrBuffer, kLastErrBufferSize) != 0) Bug("mexLastErr() failed (mxGetString)");
	mxDestroyArray(*out);
	if(clear) mexEvalString("lasterr('')");
	return ((strlen(gLastErrBuffer) > 0) ? gLastErrBuffer : NULL);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int mex_fprintf(FILE *file, char *fmt, ...)
{
	char temp[256];
	va_list ap;
	int nc;
	
	va_start(ap, fmt);
	if(file == stdout || file == stderr) {
		vsprintf(temp, fmt, ap);
		nc = mexPrintf("%s", temp);
	}
	else nc = vfprintf(file, fmt, ap);
	va_end(ap);
	return nc;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

#endif /* MATLAB_MEX_FILE */

#endif /* __MATLABTOOLS_C__ */

