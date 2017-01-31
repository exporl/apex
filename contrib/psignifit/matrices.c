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

#ifndef __MATRICES_C__
#define __MATRICES_C__

#include "universalprefix.h"
#include "mathheader.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "matlabtools.h"

#include "matrices.h"

/* //////////////////////////////////////////////////////////////////////////////////////////////// */

matrix M_LAST = NULL;

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
double *m_addr(matrix m, short dimension, long pos)
{
	long siz;
	if(dimension < 1 || dimension-- > mMaxDims) Bug("dimension argument to m_addr() must be from 1 to %d", mMaxDims);
	if(m == NULL || m->vals == NULL) return NULL;
	if((siz = m->extents[dimension]) == 0) return NULL;
	if(pos < 0) pos += siz;
	if((pos %= siz) < 0) pos += siz;
	return m->vals + m->index + (pos - m->positions[dimension]) * m->steps[dimension];
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_allocate(matrix m)
{
	long i, space;
	
	if(m == NULL) return NULL;
	if(m->vals && m->disposeable) Destroy(m->vals);
	for(space = 0, i = 0; i < mMaxDims; i++)
		if(m->steps[i] * m->extents[i] > space) space = m->steps[i] * m->extents[i];
	m->vals = ((space == 0) ? NULL : New(double , space));
	m->disposeable = TRUE;
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_anew(double *vals, short nDims, long *steps, long *extents)
{
	matrix m;
	long i;
	
	if(nDims < 1 || nDims > mMaxDims) Bug("dimension argument to m_new() must be from 1 to %d", mMaxDims);
	
	m = New(matrix_s, 1);
	m->nDims = nDims;
	m->output = NULL;
	m->description = NULL;
	m->index = 0;
	m->disposeable = FALSE;
	m->refCon = 0;
	m->warnIfEmpty = TRUE;
	strcpy(m->writeMode, "w");
	strcpy(m->writeFormat, "%lg");
	for(i = 0; i < mMaxDims; i++) m->positions[i] = 0;
	
	m_asetsize(m, nDims, extents);
	m_asetsteps(m, steps);
	if(vals == mNewData) m_allocate(m);
	else if(vals == mNoData && m) m->vals = NULL;
	else if(m) m->vals = vals;

	if(m == NULL) return NULL;
	m->next = NULL;
	m->previous = M_LAST;
	if(M_LAST) M_LAST->next = m;
	return(M_LAST = m);
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
boolean m_asetpoint(matrix m, long *pos)
{
	long i;
	boolean returnVal = TRUE;
	
	if(m == NULL) return FALSE;
	for(i = 0; i < mMaxDims; i++)
		returnVal &= m_setpos(m, i+1, ((i < m->nDims) ? pos[i] : 0));

	if(m->vals == NULL) return FALSE;
	return returnVal;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_asetsize(matrix m, short nDims, long *extents)
{
	long i;
	
	if(nDims < 1 || nDims > mMaxDims) Bug("dimension argument to m_asetsize() must be from 1 to %d", mMaxDims);
	if(m == NULL) return NULL;
	m->nDims = nDims;
	for(i = 0; i < mMaxDims; i++) m->extents[i] = ((i < nDims) ? extents[i] : 1);
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_asetsteps(matrix m, long *steps)
{
	long i, nEls;
	
	if(m == NULL) return NULL;
	for(nEls = 1, i = 0; i < mMaxDims; i++) {
		m->steps[i] = ((i >= m->nDims) ? 0 : steps ? steps[i] : nEls);
		nEls *= m->extents[i];
	}
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_aslice(matrix m, short nDims, long *extents)
{
	matrix slice;
	long i, siz, steps[mMaxDims];
	
	if(nDims < 1 || nDims > mMaxDims) Bug("dimension argument to m_aslice() must be from 1 to %d", mMaxDims);
	
	for(i = 0; i < nDims; i++) {
		siz = extents[i];
		if(siz > m->extents[i] - m->positions[i]) Bug("m_aslice(): requested slice overlaps boundaries of parent matrix");
		steps[i] = m->steps[i];
	}	
	slice = m_anew(m->vals + m->index, nDims, steps, extents);
	strcpy(slice->writeFormat, m->writeFormat);
	slice->nDims = m_countdims(slice);
	return slice;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
void m_clear(void) { while(M_LAST) m_free(M_LAST); }
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
double m_cofactor(matrix m, unsigned short row,  unsigned short col)
{
	unsigned short siz;
	long old_pos[mMaxDims];
	matrix sub;
	double result;
	
	if(m == NULL) Bug("m_cofactor() called with NULL matrix");
	if(m->nDims > 2) Bug("m_cofactor(): matrix must be two-dimensional");
	if((siz = m->extents[0]) != m->extents[1]) Bug("m_cofactor(): matrix must be square");
	if(row >= m->extents[0] || col >= m->extents[1]) Bug("m_cofactor(): indices exceed matrix dimensions");
	if(siz == 0) return 0.0;
	if(m->vals == NULL) Bug("m_cofactor() called with unallocated matrix");
	if(siz == 1) return 1.0;
	sub = m_new(mNewData, m2D, siz-1, siz-1);
	m_getpoint(m, old_pos);
	m_first(m);
	do {
		if(m_getpos(m, 1) != row && m_getpos(m, 2) != col) {
			m_val(sub) = m_val(m);
			m_next(sub);
		}
	} while(m_next(m));
	m_asetpoint(m, old_pos);
	result = m_determinant(sub);
	m_free(sub);
	return result;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_copy(matrix dest, matrix src)
{
	long i, nEls, old_src_pos[mMaxDims], old_dest_pos[mMaxDims];
	
	if(src == NULL) return NULL;
	if(dest == mNewMatrix) {
		dest = m_anew(((src->vals == NULL) ? mNoData : mNewData), src->nDims, NULL, src->extents);
		strcpy(dest->writeFormat, src->writeFormat);
	}
	else {
		for(i = 1; i <= mMaxDims; i++) if(m_getsize(src, i) != m_getsize(dest, i)) Bug("m_copy(): destination and source dimensions must match");
		if(dest->vals == NULL) m_allocate(dest);
	}
	m_getpoint(dest, old_dest_pos);
	m_getpoint(src, old_src_pos);
	for(nEls = 1, i = 0; i < mMaxDims; i++) {
		dest->steps[i] = nEls;
		nEls *= dest->extents[i];
	}	
	if(m_first(src) && m_first(dest)) {
		do {
			m_val(dest) = m_val(src);
		}while(m_next(src) && m_next(dest));
	}
	m_asetpoint(src, old_src_pos);
	m_asetpoint(dest, old_dest_pos);

	return dest;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
short m_countdims(matrix m)
{
	long i, nDims;
	if(m == NULL) return 0;
	for(nDims = 0, i = 0; i < mMaxDims; i++) if(m->extents[i] != 1) nDims = i + 1;
	return nDims;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
double m_determinant(matrix m)
{
	unsigned short siz, j;
	long old_pos[mMaxDims];
	double result, sign;
	
	if(m == NULL) Bug("m_determinant() called with NULL matrix");
	if(m->nDims > 2) Bug("m_determinant(): matrix must be two-dimensional");
	if((siz = m->extents[0]) != m->extents[1]) Bug("m_determinant(): matrix must be square");
	if(siz == 0) return 0.0;
	if(m->vals == NULL) Bug("m_determinant() called with unallocated matrix");
	if(siz == 1) return *m->vals;
	m_getpoint(m, old_pos);
	m_first(m);
	for(result = 0.0, sign = 1.0, j = 0; j < siz; j++, sign = -sign) {
		result += sign * m_val(m) * m_cofactor(m, 0, j);
		m_step(m, 2, 1);
	}
	m_asetpoint(m, old_pos);
	return result;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_diag(matrix m, matrix square)
{
	long size;
	if(square == NULL) Bug("m_diag(): received NULL input");
	if(square->nDims > 2 || (size = m_getsize(square, 1)) != m_getsize(square, 2)) Bug("m_diag(): input must be a square 2-dimensional matrix");
	if(m == mNewMatrix) m = m_new(mNewData, m1D, size);
	else {
		if(m_mass(m) != size) Bug("m_diag(): output matrix has wrong number of elements");
		if(m->vals == NULL) m_allocate(m);
	}
	if(m_first(square) && m_first(m))
		do m_val(m) = m_val(square); while(m_next(m) && m_step(square, 1, 1) && m_step(square, 2, 1));
	
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_fill(matrix m, double (*func)(short nDims, const long *pos))
{
	long old_pos[mMaxDims];
	m_getpoint(m, old_pos);
	if(m_first(m)) do m_val(m) = (*func)(m->nDims, m->positions); while(m_next(m));
	m_asetpoint(m, old_pos);
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
boolean m_first(matrix m)
{
	long i;
	boolean returnVal;

	if(m == NULL) return FALSE;
	returnVal = (m->vals != NULL);
	for(i = 0; i < mMaxDims; i++) {
		m->positions[i] = 0;
		returnVal &= (m->extents[i] != 0);
	}
	m->index = 0;
	return returnVal;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
void m_free(matrix m)
{
#ifdef MATLAB_MEX_FILE
	matrix dup;
	mxArray * mx;
	int dims[mMaxDims];
	long i, nEls;
	char temp[8], *s;
	
	if(m == NULL) return;
	if(m->output) {
		strncpy(temp, m->output, 7); for(s = temp; *s; s++) *s = tolower(*s);
		if(strlen(temp) == 0 || strcmp(temp, "null") == 0 || strcmp(temp, "false") == 0 || strcmp(temp, "0") == 0) {Destroy(m->output); m->output = NULL;}
	}

	if(m->output || m->refCon) {
		for(nEls = 1, i = 0; i <  mMaxDims; i++) {
			dims[i] = ((m->vals == NULL) ? 0 : m->extents[i]);
			if(i < m->nDims && m->steps[i] != nEls) break;
			nEls *= m->extents[i];
		}
		if(nEls > 0 && m->vals != NULL && (!m->disposeable || i < mMaxDims)) {
			dup = m_copy(mNewMatrix, m);
			dup->output = m->output; m->output = NULL;
			dup->refCon = m->refCon; m->refCon = 0;
			m_free(dup);
		}
		else {
			if(nEls == 0 || m->vals == NULL) {
				mx = mxCreateDoubleMatrix(0, 0, mxREAL);
/*				if(m->output && *m->writeMode == 'w') JWarning("no data were available for the requested assignment to %s", m->output);
*/				if(m->output && *m->writeMode == 'a') JWarning("no data were available for concatenation with %s", m->output);
			}
			else {
				mx = mxCreateDoubleMatrix(1, 1, mxREAL);
				mxFree(mxGetPr(mx));
				mxSetPr(mx, m->vals);
			}
			mxSetDimensions(mx, dims, (((m->nDims) < 2) ? 2 : m->nDims));
			if(m->disposeable && m->vals != NULL) ProtectBlock(m->vals);
			m->disposeable = FALSE;
			if(m->output != NULL && mexAssignArray(mx, "MEX__TEMP") == 0) {
				if(*m->writeMode == 'w' && mexEvalf("%s = MEX__TEMP; clear MEX__TEMP", m->output) != 0)
					JWarning("could not assign data to %s, because the assignment produced an error in MATLAB", m->output);
				if(*m->writeMode == 'a' && mexEvalf("%s = [[%s];MEX__TEMP]; clear MEX__TEMP", m->output, m->output) != 0)
					JWarning("could not append data to %s, because the vertcat operation produced an error in MATLAB", m->output);
			}
			if(m->refCon) mexAddArrayToOutputList(mx, m->refCon);
			else mxDestroyArray(mx);
		}
	}
#else
	FILE * file = NULL;
	char temp[8], *s;

	if(m == NULL) return;
	if(m->output) {
		strncpy(temp, m->output, 7); for(s = temp; *s; s++) *s = tolower(*s);
		if(strlen(temp) == 0 || strcmp(temp, "null") == 0 || strcmp(temp, "false") == 0 || strcmp(temp, "0") == 0) {Destroy(m->output); m->output = NULL;}
		if(strcmp(temp, "stderr") == 0) file = stderr;
                if(strcmp(temp, "stdout") == 0 || strcmp(temp, "-") == 0) file = stdout;
	}
	
	if(m->output) {
/*		if(file == NULL && m->vals == NULL && *m->writeMode == 'w')
			JWarning("%s was not (over)written because the requested data were not available", m->output);
		else
*/		if(file == NULL && (file = fopen(m->output, m->writeMode)) == NULL)
			JWarning("failed to write to %s", m->output);
		else {
			if(m->vals == NULL && m->description == NULL && m->warnIfEmpty)
				JWarning("some data were unavailable for %s to %s", ((*m->writeMode == 'a') ? "append" : "output"), m->output);
			m_report(file, m, ", ", "\n");
			fprintf(file, "\n");
			if(file != stderr && file != stdout) fclose(file);
		}
		
	}
#endif /* MATLAB_MEX_FILE */
	if(DEBUG)DEBUG=1;
	if(m->output != NULL) Destroy(m->output);
	if(DEBUG)DEBUG=2;
	if(m->description != NULL) Destroy(m->description);
	if(DEBUG)DEBUG=3;
	if(m->vals != NULL && m->disposeable) Destroy(m->vals);
	if(DEBUG)DEBUG=4;
	if(m->previous) m->previous->next = m->next;
	if(DEBUG)DEBUG=5;
	if(m->next) m->next->previous = m->previous;
	if(M_LAST == m) M_LAST = m->previous;
	Destroy(m);
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
long *m_getpoint(matrix m, long *buf)
{
	if(m == NULL) return NULL;
	if(buf == NULL) buf = New(long, mMaxDims);
	memcpy(buf, m->positions, mMaxDims * sizeof(long));
	return buf;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
long m_getpos(matrix m, short dimension)
{
	if(dimension < 1 || dimension-- > mMaxDims) Bug("dimension argument to m_getpos() must be from 1 to %d", mMaxDims);
	if(m == NULL) return 0;
	return m->positions[dimension];
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
long m_getsize(matrix m, short dimension)
{
	if(dimension < 1 || dimension-- > mMaxDims) Bug("dimension argument to m_getsize() must be from 1 to %d", mMaxDims);
	if(m == NULL) return 0;
	return m->extents[dimension];
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
long m_getstep(matrix m, short dimension)
{
	if(dimension < 1 || dimension-- > mMaxDims) Bug("dimension argument to m_getsize() must be from 1 to %d", mMaxDims);
	if(m == NULL) return 0;
	return m->steps[dimension];
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_hessian(matrix m, matrix dirs, matrix square)
{
	unsigned short i, j, nPartials, nResults;
	matrix temp;
	
	if(dirs == NULL || square == NULL) Bug("m_hessian(): received NULL input");
	if(dirs->nDims > 2 || square->nDims > 2) Bug("m_hessian(): inputs must be two-dimensional");
	
	nPartials = m_getsize(dirs, 1);
	nResults = m_getsize(dirs, 2);
	if(nPartials != m_getsize(square, 1)) Bug("m_hessian(): dimensions mismatch");
	if(nPartials != m_getsize(square, 2)) Bug("m_hessian(): central matrix must be square");

	if(m == mNewMatrix) m = m_new(mNewData, m2D, 1, nResults);
	else {
		if(m_getsize(m, 1) != 1) Bug("m_hessian(): output must have 1 row");
		if(m_getsize(m, 2) != nResults) Bug("m_hessian(): wrong number of output columns");
		if(m->vals == NULL) m_allocate(m);
	}
	if(!m_first(m)) return m;
	
	temp = m_mult(mNewMatrix, square, dirs);
	m_first(temp);
	m_first(dirs);
	for(j = 0; j < nResults; j++) {
		m_val(m) = 0.0;
		for(i = 0; i < nPartials; i++) {
			m_val(m) += m_val(temp) * m_val(dirs);
			m_next(temp);
			m_next(dirs);
		}
		m_next(m);
	}
	m_free(temp);
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_identity(matrix m, long size)
{
	unsigned short i, j;
	if(m == mNewMatrix) m = m_new(mNoData, m2D, size, size);
	else if(m->nDims > 2 || (size = m_getsize(m, 1)) != m_getsize(m, 2))
		Bug("m_identity(): matrix must be square and two-dimensional");

	m_first(m);
	if(m->vals == NULL) {
		m_allocate(m);
		if(size > 0) do m_val(m) = 1.0; while(m_step(m, 1, 1) && m_step(m, 2, 1));
	}
	else {
		for(i = 0; i < size; i++) {
			for(j = 0; j < size; j++) {
				m_val(m) = ((i == j) ? 1.0 : 0.0);
				m_next(m);
			}
		}
	}
	m_first(m);
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
void m_init(void){M_LAST = NULL;}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_inverse(matrix dest, matrix src)
{
	long i, j, siz, old_src_pos[mMaxDims], old_dest_pos[mMaxDims];
	double sign, det;
	
	if(src == NULL) Bug("m_inverse(): called with NULL matrix");
	if(src->nDims > 2 || (siz = m_getsize(src, 1)) != m_getsize(src, 2))
		Bug("m_inverse(): input must be a square 2D matrix");
	if(dest == mNewMatrix) {
		dest = m_new(mNewData, m2D, m_getsize(src, 1), m_getsize(src, 2));
		strcpy(dest->writeFormat, src->writeFormat);
	}
	if(dest->nDims > 2 || m_getsize(dest, 1) != siz || m_getsize(dest, 2) != siz)
		Bug("m_inverse(): dimensions of output matrix must match those of input");
	if(siz > 0 && src->vals == NULL) Bug("m_inverse(): called with unallocated matrix");
	if(dest->vals == NULL) m_allocate(dest);
	det = 0.0;
	m_getpoint(src, old_src_pos);
	m_getpoint(dest, old_dest_pos);
	for(i = 0; i < siz; i++) {
		m_setpoint(src, i, 0);
		m_setpoint(dest, 0, i);
		for(j = 0; j < siz; j++) {
			sign = (((i + j) % 2) ? -1.0 : 1.0);
			m_val(dest) = sign * m_cofactor(src, i, j);
			if(i == 0) det += m_val(src) * m_val(dest);
			m_step(src, 2, 1);
			m_step(dest, 1, 1);
		}
	}
	if(m_first(dest)) do m_val(dest) /= det; while(m_next(dest));
	m_asetpoint(dest, old_dest_pos);	
	m_asetpoint(src, old_src_pos);
	return dest;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
long m_mass(matrix m)
{
	long i, nEls;
	if(m == NULL) return 0;
	for(nEls = 1, i = 0; i < mMaxDims; i++) nEls *= m->extents[i];
	return nEls;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
void m_moveslice(matrix slice, matrix parent, unsigned short dimension, long distance)
{
	if(slice == NULL || parent == NULL || slice->vals == NULL) Bug("m_moveslice(): called with NULL or invalid matrix");
	if(dimension < 1 || dimension-- > mMaxDims) Bug("dimension argument to m_movelice() must be from 1 to %d", mMaxDims);
	slice->vals += parent->steps[dimension] * distance;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_mult(matrix result, matrix m1, matrix m2)
{
	long i, j, k, nEls, rows, cols, inner, m1Step, m2Step;
	double *m1Ptr, *m2Ptr, *resultPtr;
	
	if(m1 == NULL || m2 == NULL) Bug("m_mult() received one or more NULL matrices");
	if(m1->vals == NULL || m2->vals == NULL) Bug("m_mult() received one or more unallocated matrices");
	if(m_countdims(m1) > 2 || m_countdims(m2) > 2) Bug("m_mult() cannot multiply matrices of more than 2 dimensions");
	if((inner = m_getsize(m1, 2)) != m_getsize(m2, 1)) Bug("m_mult(): inner dimensions of matrices must match");
	rows = m_getsize(m1, 1); cols = m_getsize(m2, 2);
	if(result == mNewMatrix) result = m_new(mNewData, m2D, rows, cols);
	else {
		if(m_getsize(result, 1) != rows || m_getsize(result, 2) != cols)
			Bug("m_mult(): dimensions of pre-existing result matrix are incorrect");
		for(nEls = 1, i = 0; i < result->nDims; nEls *= result->extents[i], i++)
			if(result->steps[i] != nEls) break;
		if(i < result->nDims) Bug("m_mult(): if a pre-existing result matrix is used, it must be packed in the default manner");
		if(result->vals == NULL) m_allocate(result);
	}

	m1Step = m1->steps[0] - inner * m1->steps[1];
	m2Step = -inner * m2->steps[0];

	resultPtr = result->vals;
	m2Ptr = m2->vals;
	for(j = cols; j; j--) {
		m1Ptr = m1->vals;
		for(i = rows; i; i--) {
			*resultPtr = 0;
			for(k = inner; k; k--) {
				*resultPtr += *m1Ptr * *m2Ptr;
				m1Ptr += m1->steps[1];
				m2Ptr += m2->steps[0];
			}
			resultPtr++;
			m1Ptr += m1Step;
			m2Ptr += m2Step;
		}
		m2Ptr += m2->steps[1];
	}
	
	return result;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_new(double *vals, short nDims, ...)
{
	va_list ap;
	long i, extents[mMaxDims], steps[mMaxDims];
	boolean customPacking = FALSE;
	
	va_start(ap, nDims);	
	for(i = 0; i < mMaxDims; i++) {
		if(customPacking) steps[i] = ((i < nDims) ? va_arg(ap, long) : 1);
		extents[i] = ((i < nDims) ? va_arg(ap, long) : 1);
		if(i == 0 && !customPacking && extents[i] == mCustomPacking)
			{customPacking = TRUE; i--; continue;}		
	}
	va_end(ap);	
	
	return m_anew(vals, nDims, (customPacking ? steps : NULL), extents);
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
boolean m_next(matrix m)
{
	long i;
	
	if(m == NULL) return FALSE;
	for(i = 1; i <= m->nDims; i++)
		if(m_step(m, i, 1)) return (m->vals != NULL);
	return FALSE;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_normalize(matrix m, unsigned short dim)
{
	unsigned short i, siz;
	double len;
	boolean more;
	
	if(dim < 1 || dim > mMaxDims) Bug("m_normalize(): dimension must be from 1 to %d", mMaxDims);
	if(!m_first(m)) return m;
	m_swapdims(m, dim, 1);
	siz = m_getsize(m, dim);
	do {
		len = 0.0;
		do len += m_val(m) * m_val(m); while(m_step(m, 1, 1));
		len = sqrt(len);
		m_setpos(m, 1, 0);
		for(i = 0; i < siz; i++) {
			m_val(m) /= len;
			more = m_next(m);
		}
	} while(more);
	m_swapdims(m, dim, 1);
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
int m_report(FILE *file, matrix m, char *colDelimStr, char *rowDelimStr)
{
	int nc = 0;
	boolean started;
	
	if(m->description && strlen(m->description) > 0) nc += fprintf(file, "#%s\n", m->description);
	if(m_mass(m) == 0 || !m_setpos(m, 1, 0) || !m_setpos(m, 2, 0)) return nc;
	do {
		started = FALSE;
		do {
			if(started) nc += fprintf(file, "%s", colDelimStr);
			nc += fprintf(file, m->writeFormat, m_val(m));
			started = TRUE;
		} while(m_step(m, 2, 1));
		nc += fprintf(file, "%s", rowDelimStr);
	} while(m_step(m, 1, 1));
	return nc;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_setoutput(matrix m, char *output, char *writeMode, char *description)
{
	if(m == NULL) return NULL;
	
	if(m->output) Destroy(m->output);
	if(output == NULL || strlen(output) == 0) m->output = NULL;
	else strcpy((m->output = New(char, strlen(output)+1)), output);
	
	if(writeMode != NULL) strncpy(m->writeMode, writeMode, 3);
	
	if(m->description) Destroy(m->description);
	if(description == NULL || strlen(description) == 0) m->description = NULL;
	else strcpy((m->description = New(char, strlen(description)+1)), description);
	
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
boolean m_setpoint(matrix m, ...)
{
	va_list ap;
	long i, pos[mMaxDims];
	
	if(m == NULL) return FALSE;
	va_start(ap, m);
	for(i = 0; i < mMaxDims; i++) pos[i] = ((i < m->nDims) ? va_arg(ap, long) : 0);
	va_end(ap);
	return m_asetpoint(m, pos);
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
boolean m_setpos(matrix m, short dimension, long pos)
{
	boolean inRange;
	long siz;
	
	if(dimension < 1 || dimension-- > mMaxDims) Bug("dimension argument to m_setpos() must be from 1 to %d", mMaxDims);
	if(m == NULL) return FALSE;
	siz = m->extents[dimension];
	if(pos < 0) pos += siz;
	inRange = (pos >= 0 && pos < siz);
	if(siz == 0) pos = 0;
	else if(!inRange && (pos %= siz) < 0) pos += siz;
	m->index += m->steps[dimension] * (pos - m->positions[dimension]);
	m->positions[dimension] = pos;
	if(m->vals == NULL) return FALSE;
	return inRange;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_setsize(matrix m, short nDims, ...)
{
	va_list ap;
	long i, dims[mMaxDims];
	
	if(m == NULL) return NULL;
	va_start(ap, nDims);
	for(i = 0; i < mMaxDims; i++) dims[i] = ((i < nDims) ? va_arg(ap, long) : 1);
	va_end(ap);
	return m_asetsize(m, nDims, dims);
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_setsteps(matrix m, ...)
{
	va_list ap;
	long i, steps[mMaxDims];
	
	if(m == NULL) return NULL;
	va_start(ap, m);
	for(i = 0; i < mMaxDims; i++) steps[i] = ((i < m->nDims) ? va_arg(ap, long) : 0);
	va_end(ap);
	return m_asetsteps(m, steps);
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_slice(matrix m, short nDims, ...)
{
	va_list ap;
	long i, extents[mMaxDims];
	
	va_start(ap, nDims);
	for(i = 0; i < mMaxDims; i++) extents[i] = ((i < nDims) ? va_arg(ap, long) : 1);
	va_end(ap);
	return m_aslice(m, nDims, extents);
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
double *m_sortvals(double *vals, matrix m)
{
	long i = 0, nVals, old_pos[mMaxDims];
	
	if(m == NULL || m->vals == NULL) return NULL;
	if((nVals = m_mass(m)) == 0) return NULL;
	if(vals == NULL) vals = New(double, nVals);
	m_getpoint(m, old_pos);
	if(m_first(m)) do vals[i++] = m_val(m); while(m_next(m));
	m_asetpoint(m, old_pos);
	qsort(vals, nVals, sizeof(double), dcmp);
	return vals;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
boolean m_step(matrix m, short dimension, long distance)
{
	long pos, siz;
	boolean returnVal;

	if(dimension < 1 || dimension-- > mMaxDims) Bug("dimension argument to m_step() must be from 1 to %d", mMaxDims);
	
	if(m == NULL) return FALSE;
	siz = m->extents[dimension];
	pos = m->positions[dimension] + distance;
	returnVal = (pos >= 0 && pos < siz);
	if(siz == 0) pos = 0;
	else if(!returnVal && (pos %= siz) < 0) pos += siz;
	m->index += (pos - m->positions[dimension]) * m->steps[dimension];
	m->positions[dimension] = pos;
	if(m->vals == NULL) return FALSE;
	return returnVal;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
matrix m_swapdims(matrix m, short dim1, short dim2)
{
	long temp;
	if(dim1 < 1 || dim1-- > mMaxDims || dim2 < 1 || dim2-- > mMaxDims)
		Bug("dimension argument to m_swapdims() must be from 1 to %d", mMaxDims);
	if(m == NULL) return NULL;
	temp = m->steps[dim1];
	m->steps[dim1] = m->steps[dim2];
	m->steps[dim2] = temp;
	temp = m->extents[dim1];
	m->extents[dim1] = m->extents[dim2];
	m->extents[dim2] = temp;
	temp = m->positions[dim1];
	m->positions[dim1] = m->positions[dim2];
	m->positions[dim2] = temp;
	
	m->nDims = m_countdims(m);
	return m;
}
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
#ifdef MATLAB_MEX_FILE
matrix mxArray2matrix(mxArray * mx, char *desc)
{
	matrix m = NULL;
	short nDims, i;
	long extents[mMaxDims];
	const int *d;
	
	if((nDims = mxGetNumberOfDimensions(mx)) > mMaxDims) JError("%s has too many dimensions", desc);
	if(mxIsSparse(mx) || !mxIsDouble(mx)) JError("%s must be a full double matrix", desc);
	d = mxGetDimensions(mx);
	for(i = 0; i < mMaxDims; i++) extents[i] = ((i < nDims) ? d[i] : 1);
	m = m_anew(mNewData, nDims, NULL, extents);
	if(m->vals) CopyVals(m->vals, mxGetPr(mx), m_mass(m), sizeof(double));
	return m;
}
#endif /* MATLAB_MEX_FILE */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
#endif /* __MATRICES_C__ */
