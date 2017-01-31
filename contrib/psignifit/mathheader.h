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

#include <float.h>
#include <math.h>
/*
#include <fp.h>
*/

#undef NAN
#undef INF
#undef EPS

#if !defined pi
#define pi	3.14159265358979311599796346854418516
#endif

#ifdef MATLAB_MEX_FILE

#undef isnan
#define isnan(x)		mxIsNaN(x)
#undef isinf
#define isinf(x)		mxIsInf(x)

#else

// Tom
#undef isnan
#undef isinf

#if !defined isnan
#define isnan(x)		detect_nan(x) /*	(!((x)<=0.0) && !((x)>=0.0))	*/
#endif
#if !defined isinf
#define isinf(x)       	detect_inf(x) /*	(fabs(x)>=INF) */
#endif
#endif /* MATLAB_MEX_FILE */

extern double INF, NAN, EPS;
