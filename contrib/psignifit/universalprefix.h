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
#ifndef __UNIVERSALPREFIX_H__
#define __UNIVERSALPREFIX_H__

#ifdef MATLAB_MEX_FILE

#include "mex.h"
#define fprintf	mex_fprintf
#define calloc(n, s)		mxCalloc((n), (s))
#define malloc(s)			mxCalloc((s), 1)
#define realloc(p, s)		mxRealloc((p), (s))
#define free(p)				mxFree(p)

#else

#include <stdio.h>

#endif


typedef unsigned char boolean;
#ifndef TRUE
#define TRUE	((boolean)1)
#endif
#ifndef FALSE
#define FALSE	((boolean)0)
#endif

#define kAUTHOR_CONTACT 	"psignifit@bootstrap-software.org"

#include "supportfunctions.h"

#endif /* __UNIVERSALPREFIX_H__ */
