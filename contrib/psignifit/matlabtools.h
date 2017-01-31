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
#ifndef __MATLABTOOLS_H__
#define __MATLABTOOLS_H__

#ifdef MATLAB_MEX_FILE

#if V4_COMPAT == 1
#define MATLAB_GATEWAY(a) void a(int nargout, mxArray * _argout[], int nargin, mxArray * _argin[])
#else
#define MATLAB_GATEWAY(a) void a(int nargout, mxArray * _argout[], int nargin, const mxArray * _argin[])
#endif

#define argin(a)	(mxArray *)mxArg((mxArray**)_argin, nargin, (a), TRUE)
#define argout(a)	*(mxArray**)mxArg((mxArray**)_argout, nargout, (a), FALSE)
#define MEX_END		_argout;_argin;nargout;nargin;

void * mxArg(mxArray * argArray[], unsigned short nArgs, unsigned short argNumber, boolean input);
boolean mexAddArrayToOutputList(mxArray *array, unsigned short position);
int mexAssignArray(mxArray *array, char *name);
int mexEvalf(char * fmt, ...);
unsigned short mexGetNextOutputPosition(void);
void mexInitOutputList(int maxNargOut, int * ptrToCounter, mxArray **arrayHandle);
char * mexLastErr(boolean clear);
int mex_fprintf(FILE *file, char *fmt, ...);

#endif /* MATLAB_MEX_FILE */

#endif /* __MATLABTOOLS_H__ */

