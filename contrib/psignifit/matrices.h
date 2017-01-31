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

#ifndef __MATRICES_H__
#define __MATRICES_H__

#define mMaxDims	10

#define mNewMatrix	((matrix)NULL)
#define mNewData	((double *)-1)
#define mNoData		((double *)0)
#define mCustomPacking	((long)-1)
#define mNumericFormatLength	12

#define m1D	1
#define m2D	2
#define m3D	3

#ifdef __cplusplus
extern "C" {
#endif

typedef struct matrix_s {
	struct matrix_s * previous;
	struct matrix_s * next;
	short nDims;
	long extents[mMaxDims];
	long positions[mMaxDims];
	long steps[mMaxDims];
	long index;
	double *vals;
	char *output;
	char *description;
	boolean disposeable;
	short refCon;
	char writeMode[4];
	char writeFormat[mNumericFormatLength+1];
	boolean warnIfEmpty;
} matrix_s;

typedef matrix_s * matrix;


double *m_addr(matrix m, short dimension, long pos);
matrix m_allocate(matrix m);
matrix m_anew(double *vals, short nDims, long *steps, long *extents);
boolean m_asetpoint(matrix m, long *pos);
matrix m_asetsize(matrix m, short nDims, long *extents);
matrix m_asetsteps(matrix m, long *steps);
matrix m_aslice(matrix m, short nDims, long *extents);
void m_clear(void);
double m_cofactor(matrix m, unsigned short row,  unsigned short col);
matrix m_copy(matrix dest, matrix src);
short m_countdims(matrix m);
#define m_defaultpacking(m)		m_asetsteps((m), NULL)
double m_determinant(matrix m);
matrix m_diag(matrix m, matrix square);
matrix m_fill(matrix m, double (*func)(short nDims, const long *pos));
boolean m_first(matrix m);
void m_free(matrix m);
long m_getpos(matrix m, short dimension);
long *m_getpoint(matrix m, long *buf);
long m_getsize(matrix m, short dimension);
long m_getstep(matrix m, short dimension);
matrix m_hessian(matrix m, matrix dirs, matrix square);
matrix m_identity(matrix m, long size);
void m_init(void);
matrix m_inverse(matrix dest, matrix src);
long m_mass(matrix m);
void m_moveslice(matrix slice, matrix parent, unsigned short dimension, long distance);
matrix m_mult(matrix result, matrix m1, matrix m2);
matrix m_new(double *vals, short nDims, ...);
boolean m_next(matrix m);
matrix m_normalize(matrix m, unsigned short dim);
int m_report(FILE *file, matrix m, char *colDelimStr, char *rowDelimStr);
boolean m_setpoint(matrix m, ...);
boolean m_setpos(matrix m, short dimension, long pos);
matrix m_setsize(matrix m, short nDims, ...);
matrix m_setsteps(matrix m, ...);
matrix m_setoutput(matrix m, char *output, char *writeMode, char *description);
matrix m_slice(matrix m, short nDims, ...);
double *m_sortvals(double *vals, matrix m);
boolean m_step(matrix m, short dimension, long distance);
matrix m_swapdims(matrix m, short dim1, short dim2);
#define m_val(mx)	((mx)->vals[(mx)->index])

#ifdef MATLAB_MEX_FILE
matrix mxArray2matrix(mxArray * mx, char *desc);
#endif /* MATLAB_MEX_FILE */

#ifdef __cplusplus
}       // extern "C"
#endif


#endif /* __MATRICES_H__ */
