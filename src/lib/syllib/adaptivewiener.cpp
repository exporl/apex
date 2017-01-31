/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "adaptivewiener.h"
#include "exception.h"

#include <fftw3.h>

#include <QList>
#include <QVector>

#include <cmath>

namespace
{

/**********************/
/* General operations */
/**********************/
void InitialiseZero(double *ptr, int N)
/* Initialise matrix to zero */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  double *tmp;
  int i;

  tmp = ptr;
  for (i=0; i<N; i++) {
    *tmp++ = 0;
  }

}
/*-----------------------------------------------------------------*/
void Initialise(double *ptr, int N, double val)
/* Initialise matrix to value */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  double *tmp;
  int i;

  tmp = ptr;
  for (i=0; i<N; i++) {
    *tmp++ = val;
  }

}
/*-----------------------------------------------------------------*/
void AbsoluteMatrix_data(double *mat_data, int N)
/* Absolute value of matrix */
/* mat = abs(mat) */
/* COMPLEXITY = 0 ADD + 0 MULT (N ABS) */
{
  int i;
  double *tmp;

  tmp = mat_data;
  for (i=0;i<N;i++) {
    *tmp = fabs(*tmp);
    tmp++;
  }

}
/*-----------------------------------------------------------------*/
void Negative2ZeroMatrix_data(double *mat_data, int N)
/* Put negative elements in matrix to zero */
/* mat = mat.*(mat>0) */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  int i;
  double *tmp;

  tmp = mat_data;
  for (i=0;i<N;i++) {
    if (*tmp < 0) {*tmp = 0;}
    tmp++;
  }

}

/************************/
/* data-based functions */
/************************/
double min_data(double *ptr, int N)
/* Calculate minimum of x (real data) */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  int i;
  double minimum;

  minimum = *ptr;
  for (i = 0;i < N;i++)
    {
    if (*ptr < minimum)
      minimum = *ptr;
    ptr++;
    }
  return (minimum);
}
/*-----------------------------------------------------------------*/
double max_data(double *ptr, int N)
/* Calculate maximum of x (real data) */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  int i;
  double maximum;

  maximum = *ptr;
  for (i = 0;i < N;i++)
    {
    if (*ptr > maximum)
      maximum = *ptr;
    ptr++;
    }
  return (maximum);
}
/*-----------------------------------------------------------------*/
double maxabs_data(double *ptr, int N)
/* Calculate maximum of abs(x) (real data) */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  int i;
  double maximum, tmp;

  maximum = 0;
  for (i = 0;i < N;i++)
    {
    tmp = *ptr++;
    if (tmp < 0)
      tmp=-tmp;
    if (tmp > maximum)
      maximum = tmp;
    }
  return (maximum);
}
/*-----------------------------------------------------------------*/
double sum_data(double *ptr, int N)
/* Calculate sum of all elements in matrix x (real data) */
/* COMPLEXITY = (N-1) ADD + 0 MULT */
{
  int i;
  double s;

  s = *ptr++;
  for (i = 1;i < N ;i++)
    {
    s += *ptr++;
    }
  return (s);
}
/*-----------------------------------------------------------------*/
double sum2_data(double *ptr, int N)
/* Calculate squared sum of all elements in matrix x (real data) */
/* COMPLEXITY = (N-1) ADD + N MULT */
{
  int i;
  double s;

  s = (*ptr)*(*ptr);
  for (i = 1;i < N ;i++)
    {
    ptr++;
    s += (*ptr)*(*ptr);
    }
  return (s);
}
/*-----------------------------------------------------------------*/
double sum2Reverse_data(double *ptr, int N)
/* Calculate squared sum of all elements in matrix x (real data) */
/* REVERSE ADDRESSING */
/* COMPLEXITY = (N-1) ADD + N MULT */
{
  int i;
  double s;

  s = (*ptr)*(*ptr);
  for (i = 1;i < N ;i++)
    {
    ptr--;
    s += (*ptr)*(*ptr);
    }
  return (s);
}
/*-----------------------------------------------------------------*/
double mean_data(double *ptr, int N)
/* Calculate mean of x (real data) */
/* COMPLEXITY = (N-1) ADD + 1 MULT */
{
  return ((sum_data(ptr,N)/N));
}

/******************/
/* Real operation */
/******************/
void TransposeMatrix_data(double *mat_data, double *result_data, int M, int P)
/* Transpose real matrix (correct dimensions) */
/* out = mat' */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  int i,j;
  double *tmp;

  for (i=0;i<P;i++) {
    tmp = result_data++;
    for (j=0;j<M;j++) {
      *tmp = *mat_data++;
      tmp += P;
    }
  }
}
/*-----------------------------------------------------------------*/
void MultiplyMatrix_data(double *mat1_data, double factor, int N)
/* Multiply real matrix with scalar */
/* mat1 = factor*mat1 */
/* COMPLEXITY = 0 ADD + N MULT */
{
  int i;

  for (i=0;i<N;i++) {
    *mat1_data *= factor;
    mat1_data++;
  }
}
/*-----------------------------------------------------------------*/
void SumMatrix_data(double *mat1_data, double *mat2_data, double *result_data, int N)
/* Calculate sum of 2 real matrices (of equal dimensions) */
/* out = mat1+mat2; */
/* COMPLEXITY = N ADD + 0 MULT */
{
  int i;

  for (i=0;i<N;i++) {
    *result_data++ = (*mat1_data++)+(*mat2_data++);
  }
}
/*-----------------------------------------------------------------*/
void SubtractMatrix_data(double *mat1_data, double *mat2_data, double *result_data, int N)
/* Calculate subtraction of 2 real matrices (of equal dimensions) */
/* out = mat1-mat2; */
/* COMPLEXITY = N ADD + 0 MULT */
{
  int i;

  for (i=0;i<N;i++) {
    *result_data++ = (*mat1_data++)-(*mat2_data++);
  }
}
/*-----------------------------------------------------------------*/
void AddMatrix_data(double *mat1_data, double *result_data, int N, double lambda)
/* Add scaled matrix to other matrix */
/* out = out+lambda*mat1; */
/* COMPLEXITY = N ADD + N MULT (lambda ~= 1) */
/*            = N ADD + 0 MULT (lambda == 1) */
{
  int i;

  for (i=0;i<N;i++) {
    *result_data += lambda*(*mat1_data++);
    result_data++;
  }
}
/*-----------------------------------------------------------------*/
double InnerProduct_data(double *vec1_data, double *vec2_data, int N)
/* Calculate inner product of 2 real vectors (of equal length) */
/* out = vec1'*vec2; */
/* COMPLEXITY = (N-1) ADD + N MULT */
{
  double sum;
  int i;

  sum = (*vec1_data++)*(*vec2_data++);
  for (i=1;i<N;i++) {
    sum += (*vec1_data++)*(*vec2_data++);
  }
  return (sum);
}
/*-----------------------------------------------------------------*/
double InnerProductReverse_data(double *vec1_data, double *vec2_data, int N)
/* Calculate inner product of 2 real vectors (of equal length) */
/* REVERSE ADDRESSING for vec2_data */
/* out = vec1'*flipud(vec2); */
/* COMPLEXITY = (N-1) ADD + N MULT */
{
  double sum;
  int i;

  sum = (*vec1_data++)*(*vec2_data--);
  for (i=1;i<N;i++) {
    sum += (*vec1_data++)*(*vec2_data--);
  }
  return (sum);
}
/*-----------------------------------------------------------------*/
void AddOuterProduct_data(double *mat1_data, double *vec1_data, double factor, int N)
/* Add scaled outer product of vector to (square) matrix */
/* mat1 = mat1+factor*vec1*vec1'; */
/* COMPLEXITY = N^2 ADD + (N^2+N) MULT (factor ~= 1) not exploiting symmetry */
/*            = N^2 ADD + N^2 MULT     (factor == 1) not exploiting symmetry */
{
  int i,j;
  double *tmp1, *tmp2;

  tmp1 = vec1_data;
  for (i=0;i<N;i++) {
    tmp2 = vec1_data;
    for (j=0;j<N;j++) {
      *mat1_data += factor*(*tmp2++)*(*tmp1);
      mat1_data++;
    }
    tmp1++;
  }
}
/*-----------------------------------------------------------------*/
void MatrixMult_data(double *mat1_data, double *mat2_data, double *result_data, int M, int P, int N)
/* Calculate product of two real matrices (correct dimensions) */
/* out = mat1*mat2 */
/* COMPLEXITY = (P-1)MN ADD + PMN MULT */
{
  double sum;
  int i,j,k;
  double *tmp,*tmp1,*tmp2;

  for (k=0;k<N;k++) {
    tmp1 = mat1_data;
    for (i=0;i<M;i++) {
      sum = 0;
      tmp2 = mat2_data+(k*P);
      tmp = tmp1++;
      for (j=0;j<P;j++) {
    sum += (*tmp)*(*tmp2++);
    tmp += M;
      }
      *result_data++ = sum;
    }
  }
}
/*-----------------------------------------------------------------*/
void MatrixVecMult_data(double *mat1_data, double *vec1_data, double *result_data, int M, int P)
/* Calculate product of real matrix and real vector (correct dimensions) */
/* out = mat1*vec1 */
/* COMPLEXITY = (P-1)M ADD + PM MULT */
{
  double sum;
  int i,j;
  double *tmp,*tmp2;

    for (i=0;i<M;i++) {
      sum = 0;
      tmp2 = vec1_data;
      tmp = mat1_data++;
      for (j=0;j<P;j++) {
    sum += (*tmp)*(*tmp2++);
    tmp += M;
      }
      *result_data++ = sum;
    }
}
/*-----------------------------------------------------------------*/
void MatrixVecMultReverse_data(double *mat1_data, double *vec1_data, double *result_data, int M, int P)
/* Calculate product of real matrix and real vector (correct dimensions) */
/* REVERSE ADDRESSING for vec1_data */
/* out = mat1*flipud(vec1) */
/* COMPLEXITY = (P-1)M ADD + PM MULT */
{
  double sum;
  int i,j;
  double *tmp,*tmp2;

    for (i=0;i<M;i++) {
      sum = 0;
      tmp2 = vec1_data;
      tmp = mat1_data++;
      for (j=0;j<P;j++) {
    sum += (*tmp)*(*tmp2--);
    tmp += M;
      }
      *result_data++ = sum;
    }
}
/*-----------------------------------------------------------------*/
void AddMatrixVecMultReverse_data(double *mat1_data, double *vec1_data, double *result_data, int M, int P)
/* Add product of real matrix and real vector (correct dimensions) to vector */
/* REVERSE ADDRESSING for vec1_data */
/* out = out + mat1*flipud(vec1) */
/* COMPLEXITY = PM ADD + PM MULT */
{
  double sum;
  int i,j;
  double *tmp,*tmp2;

    for (i=0;i<M;i++) {
      sum = 0;
      tmp2 = vec1_data;
      tmp = mat1_data++;
      for (j=0;j<P;j++) {
    sum += (*tmp)*(*tmp2--);
    tmp += M;
      }
      *result_data++ += sum;
    }
}
/*-----------------------------------------------------------------*/
void SubtractMatrixVecMultReverse_data(double *mat1_data, double *vec1_data, double *result_data, int M, int P)
/* Subtract product of real matrix and real vector (correct dimensions) to vector */
/* REVERSE ADDRESSING for vec1_data */
/* out = out - mat1*flipud(vec1) */
/* COMPLEXITY = PM ADD + PM MULT */
{
  double sum;
  int i,j;
  double *tmp,*tmp2;

    for (i=0;i<M;i++) {
      sum = 0;
      tmp2 = vec1_data;
      tmp = mat1_data++;
      for (j=0;j<P;j++) {
    sum += (*tmp)*(*tmp2--);
    tmp += M;
      }
      *result_data++ -= sum;
    }
}
/*-----------------------------------------------------------------*/
void PointWiseProduct_data(double *vec1_data, double *vec2_data, double *result_data, int N)
/* Calculate pointwise product of 2 real vectors (of equal length) */
/* out = vec1.*vec2; */
/* COMPLEXITY = 0 ADD + N MULT */
{
  int i;

  for (i=0;i<N;i++) {
    *result_data++ = (*vec1_data++)*(*vec2_data++);
  }
}
/*-----------------------------------------------------------------*/

/********************************************/
/* Complex operations (without <complex.h>) */
/********************************************/
void HermitianMatrix_data(double *mat_data_real, double *mat_data_imag, double *result_data_real, double *result_data_imag, int M, int P)
/* Hermitian of complex matrix (correct dimensions) */
/* out = mat' */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
  int i,j;
  double *tmp_real, *tmp_imag;

  for (i=0;i<P;i++) {
    tmp_real = result_data_real++;
    tmp_imag = result_data_imag++;
    for (j=0;j<M;j++) {
      *tmp_real = *mat_data_real++;
      tmp_real += P;
      *tmp_imag = -(*mat_data_imag++);
      tmp_imag += P;
    }
  }
}
/*-----------------------------------------------------------------*/
void ConjugateMatrix_data(double *mat_data_real, double *mat_data_imag, double *result_data_real, double *result_data_imag, int N)
/* Complex conjugate of complex matrix (correct dimensions) */
/* out = conj(mat) */
/* COMPLEXITY = 0 ADD + 0 MULT */
{
    int i;

    for (i=0;i<N;i++) {
        *result_data_real++ = *mat_data_real++;
        *result_data_imag++ = -*mat_data_imag++;
    }
}
/*-----------------------------------------------------------------*/
void MultiplyMatrix_cplx_data(double *mat1_data_real, double *mat1_data_imag, double factor, int N)
/* Multiply complex matrix with scalar */
/* mat1 = factor*mat1 */
/* COMPLEXITY = 0 ADD + 2N MULT */
{
  int i;

  for (i=0;i<N;i++) {
    *mat1_data_real *= factor;
    mat1_data_real++;
    *mat1_data_imag *= factor;
    mat1_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
void SumMatrix_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *mat2_data_real, double *mat2_data_imag, double *result_data_real, double *result_data_imag, int N)
/* Calculate sum of 2 complex matrices (of equal dimensions) */
/* out = mat1+mat2; */
/* COMPLEXITY = 2N ADD + 0 MULT */
{
  int i;

  for (i=0;i<N;i++) {
    *result_data_real++ = (*mat1_data_real++)+(*mat2_data_real++);
    *result_data_imag++ = (*mat1_data_imag++)+(*mat2_data_imag++);
  }
}
/*-----------------------------------------------------------------*/
void AddMatrix_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *result_data_real, double *result_data_imag, int N, double lambda)
/* Add scaled complex matrix to other complex matrix */
/* out = out+lambda*mat1; */
/* COMPLEXITY = 2N ADD + 2N MULT (lambda ~= 1) */
/*            = 2N ADD + 0 MULT  (lambda == 1) */
{
  int i;

  for (i=0;i<N;i++) {
    *result_data_real += lambda*(*mat1_data_real++);
    *result_data_imag += lambda*(*mat1_data_imag++);
    result_data_real++;
    result_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
void SubtractMatrix_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *result_data_real, double *result_data_imag, int N)
/* Subtract complex matrix from other complex matrix */
/* out = out-mat1; */
/* COMPLEXITY = 2N ADD + 0 MULT */
{
  int i;

  for (i=0;i<N;i++) {
    *result_data_real -= *mat1_data_real++;
    *result_data_imag -= *mat1_data_imag++;
    result_data_real++;
    result_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
void InnerProduct_cplx_data(double *vec1_data_real, double *vec1_data_imag, double *vec2_data_real, double *vec2_data_imag, double* result_data, int N)
/* Calculate inner product of 2 complex vectors (of equal length) */
/* out = vec1'*vec2; */
/* COMPLEXITY = (4N-2) ADD + 4N MULT */
{
  double sum_real,sum_imag;
  int i;
  double a1R,a2R,a1I,a2I;

  sum_real = 0;
  sum_imag = 0;
  for (i=0;i<N;i++) {
    a1R = *vec1_data_real++;
    a1I = *vec1_data_imag++;
    a2R = *vec2_data_real++;
    a2I = *vec2_data_imag++;

    sum_real += a1R*a2R+a1I*a2I;
    sum_imag += a1R*a2I-a1I*a2R;
  }

  *result_data++ = sum_real;
  *result_data   = sum_imag;
}
/*-----------------------------------------------------------------*/
void MatrixMult_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *mat2_data_real, double *mat2_data_imag, double *result_data_real, double *result_data_imag, int M, int P, int N)
/* Calculate product of two complex matrices (correct dimensions) */
/* out = mat1*mat2 */
/* COMPLEXITY = (4P-2)MN ADD + 4PMN MULT */
{
  double sum_real,sum_imag;
  int i,j,k;
  double *tmpR,*tmp1R,*tmp2R,*tmpI,*tmp1I,*tmp2I;
  double a1R,a2R,a1I,a2I;

  for (k=0;k<N;k++) {
    tmp1R = mat1_data_real;
    tmp1I = mat1_data_imag;
    for (i=0;i<M;i++) {
      sum_real = 0;
      sum_imag = 0;
      tmp2R = mat2_data_real+(k*P);
      tmp2I = mat2_data_imag+(k*P);
      tmpR = tmp1R++;
      tmpI = tmp1I++;
      for (j=0;j<P;j++) {
    a1R = *tmpR;tmpR += M;
    a1I = *tmpI;tmpI += M;
    a2R = *tmp2R++;
    a2I = *tmp2I++;

    sum_real += a1R*a2R-a1I*a2I;
    sum_imag += a1R*a2I+a1I*a2R;
      }
      *result_data_real++ = sum_real;
      *result_data_imag++ = sum_imag;
    }
  }
}
/*-----------------------------------------------------------------*/
void MatrixVecMult_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *vec1_data_real, double *vec1_data_imag, double *result_data_real, double *result_data_imag, int M, int P)
/* Calculate product of complex matrix and complex vector  (correct dimensions) */
/* out = mat1*vec1 */
/* COMPLEXITY = (4P-2)M ADD + 4PM MULT */
{
  double sum_real,sum_imag;
  int i,j;
  double *tmpR,*tmp2R,*tmpI,*tmp2I;
  double a1R,a2R,a1I,a2I;

  for (i=0;i<M;i++) {
      sum_real = 0;
      sum_imag = 0;
      tmp2R = vec1_data_real;
      tmp2I = vec1_data_imag;
      tmpR = mat1_data_real++;
      tmpI = mat1_data_imag++;
      for (j=0;j<P;j++) {
    a1R = *tmpR;tmpR += M;
    a1I = *tmpI;tmpI += M;
    a2R = *tmp2R++;
    a2I = *tmp2I++;

    sum_real += a1R*a2R-a1I*a2I;
    sum_imag += a1R*a2I+a1I*a2R;
      }
      *result_data_real++ = sum_real;
      *result_data_imag++ = sum_imag;
    }
}
/*-----------------------------------------------------------------*/
void AddMatrixVecMultSpecial_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *vec1_data_real, double *vec1_data_imag, double *result_data_real, double *result_data_imag, int M, int P)
/* Add special multiplication of complex matrix and complex vector to complex vector (size matrix = MxP, size vector = M) */
/* for i = 1:M, out(i) = out(i) + mat1(i,:)*vec1(i:i+P-1); end */
/* COMPLEXITY = (4P-2)M ADD + 4PM MULT */
{
  double sum_real,sum_imag;
  int i,j;
  double *tmpR,*tmp2R,*tmpI,*tmp2I;
  double a1R,a2R,a1I,a2I;

  for (i=0;i<M;i++) {
      sum_real = 0;
      sum_imag = 0;
      tmp2R = vec1_data_real++;
      tmp2I = vec1_data_imag++;
      tmpR = mat1_data_real++;
      tmpI = mat1_data_imag++;
      for (j=0;j<P;j++) {
        a1R = *tmpR;tmpR += M;
        a1I = *tmpI;tmpI += M;
        a2R = *tmp2R++;
        a2I = *tmp2I++;

        sum_real += a1R*a2R-a1I*a2I;
        sum_imag += a1R*a2I+a1I*a2R;
      }
      *result_data_real += sum_real;
      *result_data_imag += sum_imag;

      result_data_real++;
      result_data_imag++;
    }
}
/*-----------------------------------------------------------------*/
void PointWiseDivisionSpecial_cplx_data(double *vec1_data, double *result_data_real, double *result_data_imag, double fact, double delta, int N)
/* Calculate pointwise division of 2 vectors (of equal length), with vec1 real elements */
/* out = out./(vec1+delta)*fact; */
/* COMPLEXITY = N ADD + 2N MULT + N DIV (delta ~= 0) */
/*            = 0 ADD + 2N MULT + N DIV (delta == 0) */
{
  int i;
  double mult;

  for (i=0;i<N;i++) {
    mult = fact/(delta+*vec1_data++);
    *result_data_real *= mult;
    result_data_real++;
    *result_data_imag *= mult;
    result_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
void PointWiseProduct_cplx_data(double *vec1_data_real, double *vec1_data_imag, double *vec2_data_real, double *vec2_data_imag, double *result_data_real, double *result_data_imag, int N)
/* Calculate pointwise product of 2 complex vectors (of equal length) */
/* out = vec1.*vec2; */
/* COMPLEXITY = 2N ADD + 4N MULT */
{
  int i;
  double a1R,a2R,a1I,a2I;

  for (i=0;i<N;i++) {
    a1R = *vec1_data_real++;
    a1I = *vec1_data_imag++;
    a2R = *vec2_data_real++;
    a2I = *vec2_data_imag++;

    *result_data_real++ = a1R*a2R-a1I*a2I;
    *result_data_imag++ = a1R*a2I+a1I*a2R;
  }
}
/*-----------------------------------------------------------------*/
void PointWiseProductH_cplx_data(double *vec1_data_real, double *vec1_data_imag, double *vec2_data_real, double *vec2_data_imag, double *result_data_real, double *result_data_imag, int N)
/* Calculate pointwise product of 2 complex vectors (1 hermitian conjugate!) */
/* out = conj(vec1).*vec2; */
/* COMPLEXITY = 2N ADD + 4N MULT */
{
  int i;
  double a1R,a2R,a1I,a2I;

  for (i=0;i<N;i++) {
    a1R = *vec1_data_real++;
    a1I = *vec1_data_imag++;
    a2R = *vec2_data_real++;
    a2I = *vec2_data_imag++;

    *result_data_real++ = a1R*a2R+a1I*a2I;
    *result_data_imag++ = a1R*a2I-a1I*a2R;
  }
}
/*-----------------------------------------------------------------*/
void AddPointWiseProductH_cplx_data(double *vec1_data_real, double *vec1_data_imag, double *vec2_data_real, double *vec2_data_imag, double *result_data_real, double *result_data_imag, int N, double lambda1, double lambda2)
/* Add scaled pointwise product of 2 complex vectors (1 hermitian conjugate!) to scaled complex vector */
/* out = lambda1*out + lambda2*conj(vec1).*vec2; */
/* COMPLEXITY = 4N ADD + 4N MULT + 2N MULT (lambda1 ~= 1) + 2N MULT (lambda2 ~= 1) */
/* (vec1=vec2)= 2N ADD + 2N MULT + N MULT  (lambda1 ~= 1) + N MULT  (lambda2 ~= 1) */
{
  int i;
  double a1R,a2R,a1I,a2I;

  for (i=0;i<N;i++) {
    a1R = *vec1_data_real++;
    a1I = *vec1_data_imag++;
    a2R = *vec2_data_real++;
    a2I = *vec2_data_imag++;

    *result_data_real *= lambda1;
    *result_data_imag *= lambda1;

    *result_data_real += lambda2*(a1R*a2R+a1I*a2I);
    *result_data_imag += lambda2*(a1R*a2I-a1I*a2R);

    result_data_real++;
    result_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
void AddPointWiseProductH_Special1_cplx_data(double *vec1_data_real, double *vec1_data_imag, double *vec2_data_real, double *vec2_data_imag, double *result_data_real, double *result_data_imag, int N, double lambda1, double gR, double gI)
/* Add scaled pointwise product of 2 complex vectors (1 hermitian conjugate!) to scaled complex vector */
/* special version (nrdiag = 2) */
/* out = lambda1*out + g*conj(vec1).*[conj(vec2(2));vec2(1:end-1)] */
/* COMPLEXITY = 6N ADD + 8N MULT + 2N MULT (lambda1 ~= 1) */
{
  int i;
  double a1R,a2R,a1I,a2I,cR,cI;

  /* First element */
  a1R = *vec1_data_real++;
  a1I = *vec1_data_imag++;
  a2R = vec2_data_real[1];
  a2I = -vec2_data_imag[1];

  cR = a1R*a2R+a1I*a2I;
  cI = a1R*a2I-a1I*a2R;

  *result_data_real *= lambda1;
  *result_data_imag *= lambda1;

  *result_data_real += (gR*cR-gI*cI);
  *result_data_imag += (gR*cI+gI*cR);

  result_data_real++;
  result_data_imag++;

  /* Other elements */
  for (i=1;i<N;i++) {
    a1R = *vec1_data_real++;
    a1I = *vec1_data_imag++;
    a2R = *vec2_data_real++;
    a2I = *vec2_data_imag++;

    cR = a1R*a2R+a1I*a2I;
    cI = a1R*a2I-a1I*a2R;

    *result_data_real *= lambda1;
    *result_data_imag *= lambda1;

    *result_data_real += (gR*cR-gI*cI);
    *result_data_imag += (gR*cI+gI*cR);

    result_data_real++;
    result_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
void AddPointWiseProductH_Special2_cplx_data(double *vec1_data_real, double *vec1_data_imag, double *vec2_data_real, double *vec2_data_imag, double *result_data_real, double *result_data_imag, int N, double lambda1, double gR, double gI)
/* Add scaled pointwise product of 2 complex vectors (1 hermitian conjugate!) to scaled complex vector */
/* special version (nrdiag = 2) */
/* out = lambda1*out + g*conj(vec1).*[vec2(2:end);conj(vec2(end-1))] */
/* COMPLEXITY = 6N ADD + 8N MULT + 2N MULT (lambda1 ~= 1) */
{
  int i;
  double a1R,a2R,a1I,a2I,cR,cI;

  vec2_data_real++;
  vec2_data_imag++;

  /* Other elements */
  for (i=1;i<N;i++) {
    a1R = *vec1_data_real++;
    a1I = *vec1_data_imag++;
    a2R = *vec2_data_real++;
    a2I = *vec2_data_imag++;

    cR = a1R*a2R+a1I*a2I;
    cI = a1R*a2I-a1I*a2R;

    *result_data_real *= lambda1;
    *result_data_imag *= lambda1;

    *result_data_real += (gR*cR-gI*cI);
    *result_data_imag += (gR*cI+gI*cR);

    result_data_real++;
    result_data_imag++;
  }

  /* Last element */
  a1R = *vec1_data_real;
  a1I = *vec1_data_imag;
  vec2_data_real -= 2;
  vec2_data_imag -= 2;
  a2R = *vec2_data_real;
  a2I = -(*vec2_data_imag);

  cR = a1R*a2R+a1I*a2I;
  cI = a1R*a2I-a1I*a2R;

  *result_data_real *= lambda1;
  *result_data_imag *= lambda1;

  *result_data_real += (gR*cR-gI*cI);
  *result_data_imag += (gR*cI+gI*cR);

}
/*-----------------------------------------------------------------*/
void PointWiseProduct_cplx2_data(double *vec1_data_real, double *vec1_data_imag, double *result_data, int N)
/* Calculate energy in complex vector */
/* out = conj(vec1).*vec1; */
/* COMPLEXITY = N ADD + 2N MULT */
{
  int i;
  double a1R,a1I;

  for (i=0;i<N;i++) {
    a1R = *vec1_data_real++;
    a1I = *vec1_data_imag++;
    *result_data++ = a1R*a1R+a1I*a1I;
  }
}
/*-----------------------------------------------------------------*/
void PointWiseProductSum_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *mat2_data_real, double *mat2_data_imag, double *result_data_real, double *result_data_imag, int M, int N)
/* Calculate pointwise product of 2 complex matrices (of equal dimensions) + sum over rows */
/* out = sum(mat1.*mat2,2) */
/* COMPLEXITY = 2M(2N-1) ADD + 4MN MULT */
{
  int i,j;
  double a1R,a2R,a1I,a2I;
  double *tmpR,*tmpI;

  /* Initialisation */
  tmpR = result_data_real;
  tmpI = result_data_imag;
  for (i=0;i<M;i++) {
    *tmpR++ = 0;
    *tmpI++ = 0;
  }

  for (j=0;j<N;j++) {
    tmpR = result_data_real;
    tmpI = result_data_imag;
    for (i=0;i<M;i++) {
      a1R = *mat1_data_real++;
      a1I = *mat1_data_imag++;
      a2R = *mat2_data_real++;
      a2I = *mat2_data_imag++;

      *tmpR += a1R*a2R-a1I*a2I;
      *tmpI += a1R*a2I+a1I*a2R;
      tmpR++;
      tmpI++;
    }
  }
}
/*-----------------------------------------------------------------*/
void PointWiseProductSum_cplx2_data(double *mat1_data_real, double *mat1_data_imag, double *result_data, int M, int N)
/* Calculate energy in complex matrix */
/* out = sum(conj(mat1).*mat1,2); */
/* COMPLEXITY = M(2N-1) ADD + 2MN MULT */
{
  int i,j;
  double a1R,a1I;
  double *tmpR;

  /* Initialisation */
  tmpR = result_data;
  for (i=0;i<M;i++) {
    *tmpR++ = 0;
  }

  for (j=0;j<N;j++) {
    tmpR = result_data;
    for (i=0;i<M;i++) {
      a1R = *mat1_data_real++;
      a1I = *mat1_data_imag++;
      *tmpR += a1R*a1R+a1I*a1I;
      tmpR++;
    }
  }
}
/*-----------------------------------------------------------------*/
void MultiPointWiseProduct_cplx_data(double *mat1_data_real, double *mat1_data_imag, double *vec2_data_real, double *vec2_data_imag, double *result_data_real, double *result_data_imag, int M, int N)
/* Calculate pointwise product of matrix and vector */
/* N = size(mat1,2);mat2 = repmat(vec2,1,N);out = conj(mat1).*mat2; */
/* COMPLEXITY = 2MN ADD + 4MN MULT */
{
  int i,j;
  double *tmpR,*tmpI;
  double a1R,a2R,a1I,a2I;

  for (i=0;i<N;i++) {
    tmpR = vec2_data_real;
    tmpI = vec2_data_imag;
    for (j=0;j<M;j++) {
      a1R = *mat1_data_real++;
      a1I = *mat1_data_imag++;
      a2R = *tmpR++;
      a2I = *tmpI++;

      *result_data_real++ = a1R*a2R+a1I*a2I;
      *result_data_imag++ = a1R*a2I-a1I*a2R;
    }
  }
}
/*-----------------------------------------------------------------*/
void Inverse2_cplx_data(double ar, double br, double bi, double cr, double *out) {
/* Calculate inverse of 2D complex matrix (Hermitian + real elements on diagonal) */
/* [dr      er+j*ei] = [ar      br-j*bi]-1  */
/* [er+j*ei fr     ] = [br+j*bi cr     ]    */
/* COMPLEXITY = 2 ADD + 7 MULT + 1 DIV */

  double invdet;

  invdet = 1/(ar*cr-br*br-bi*bi);
  *out++ = invdet*cr;
  *out++ = -invdet*br;
  *out++ = -invdet*bi;
  *out = invdet*ar;
}
/*-----------------------------------------------------------------*/
void BlockInverse2_cplx_data(double *in_data_real, double *in_data_imag, int M, double delta) {
/* Calculate inverse (inline) of block-diagonal complex matrix (dimensions (2*M)*2) + regularisation delta*/
/* COMPLEXITY = 2M ADD + 7M MULT + M DIV */

  int i;
  double out[4];

  for (i=0; i<M; i++) {
    Inverse2_cplx_data((*in_data_real)+delta,*(in_data_real+M),*(in_data_imag+M),(*(in_data_real+3*M))+delta,out);

    *in_data_real = out[0];
    *(in_data_real+M) = out[1];
    *(in_data_real+2*M) = out[1];
    *(in_data_real+3*M) = out[3];

    *in_data_imag = 0;
    *(in_data_imag+M) = out[2];
    *(in_data_imag+2*M) = -out[2];
    *(in_data_imag+3*M) = 0;

    in_data_real++;
    in_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
void PositiveDefiniteBlockMatrix2_cplx_data(double *in_data_real, double *in_data_imag, int M) {
/* Make block-diagonal complex matrix (dimensions (2*M)*2) positive definite by adding regularisation */
/* i.e. calculate smallest eigenvalue of 2x2 subblocks -> add regularisation if smallest eigenvalue is negative */
/* COMPLEXITY = 7M ADD + 5M MULT + M SQRT */

  int i;
  double a,br,bi,c,delta;

  for (i=0; i<M; i++) {
    /* Smallest eigenvalue of [ar      br-j*bi] is equal to ((a+c)-sqrt((a-c)^2+4*|b|^2))/2  */
    /*                        [br+j*bi cr     ]                                              */

    a = *in_data_real;
    br = *(in_data_real+2*M);
    bi = *(in_data_imag+2*M);
    c = *(in_data_real+3*M);

    delta = ((a+c)-sqrt((a-c)*(a-c)+4*(br*br+bi*bi)))/2;

    if (delta < 0) {
      *in_data_real -= delta;
      *(in_data_real+3*M) -= delta;
    }

    in_data_real++;
    in_data_imag++;
  }
}
/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
void ConstrainedFreq2_cplx_data(double *fftR, double *fftI, double *real_data, int M, int P, fftw_plan pFFT, fftw_plan pIFFT)
/* Calculate IFFT + zeroing (last part) + FFT of vector */
/* In calling c-file:*/
     /* in_data  = (double *) fftw_malloc(M*P*sizeof(double)); */
     /* fftR = (double *) fftw_malloc(N*P*soutputStageBufferizeof(double)); */
     /* fftI = (double *) fftw_malloc(N*P*sizeof(double)); */
     /* rank = 1;dims[0].n = M;dims[0].is = 1;dims[0].os = 1;howmany_rank = 1;howmany_dims[0].n = P; */
     /* howmany_dims[0].is = M;howmany_dims[0].os = N; */
     /* pFFT  = fftw_plan_guru_split_dft_r2c(rank,dims,howmany_rank,howmany_dims,in_data,fftR,fftI,FFTW_ESTIMATE); */
     /* howmany_dims[0].is = N;howmany_dims[0].os = M; */
     /* pIFFT = fftw_plan_guru_split_dft_c2r(rank,dims,howmany_rank,howmany_dims,fftR,fftI,out_data,FFTW_ESTIMATE); */
/* [M,P] = size(vec1); tmp = ifft(vec1); vec1 = fft([tmp(1:M/2,:);zeros(M/2,P)]) */
/* COMPLEXITY = 2 FFT */
{
  int i,j,K;
  double *tmp;

  fftw_execute_split_dft_c2r(pIFFT,fftR,fftI,real_data);

  K = M/2;
  tmp = real_data;
  for (i=0; i<P; i++) {
    tmp += K;
    for (j=0; j<K; j++) {
      *tmp++ = 0;
    }
  }

  fftw_execute_split_dft_r2c(pFFT,real_data,fftR,fftI);
}

/* Online version of d:\Programs\c\SDW_MWF\SDW_MWF_freqlmsLP6b_stepFD (reg = 1) */
void SDW_MWF_freqlmsLP6b_stepFD(double * const *in_data, const double * const *in_dataSNROffset, double * const *in_delay, double * const *in_delaySNROffset,
                         double *z_data, const double *speech_data, int nr_samples, int nr_channels, int nr_ref, int L, int delay,
                         double rho, double mu_inv, double /*gamma*/, double lambda,
                         double *W_data_real, double *W_data_imag, double *P_data,
                         double *Sy_data_real, double *Sy_data_imag, double *Sn_data_real, double *Sn_data_imag,
                         fftw_plan pFFT1_2L, fftw_plan pIFFT1_2L, double *fftreal, double *fftR, double *fftI, int step, int constr, int negstep, double delta)
{
    /* Variables declaration */
    int i,j,k;
    int L1,L1tot,nr_blocks, offset_ref;
    double inv2L,invnr_ref,fact;

    double *y_data, *s_data;
    double *P2_data, *e_data;
    double *data_real, *data_imag;
    double *R_data_real, *R_data_imag;
    double *E_data_real, *E_data_imag;
    double *tmp_mat_real, *tmp_mat_imag, *tmpy, *tmps;
    double *fftreal2, *e_data2, *ref_data;

    /* Input parameters */

    L1 = L+1;
    nr_blocks = (int) (nr_samples/L); /* Should be integer !! */
    L1tot = L1*nr_ref;

    /* Initialisation + memory allocation */

    y_data = (double*) calloc((L+nr_samples)*nr_channels, sizeof(double));
    s_data = (double*) calloc((L+nr_samples)*nr_channels, sizeof(double));

    tmpy = y_data;
    tmps = s_data;

    for (i=0; i<nr_channels; i++) {
        memcpy(tmpy,in_delaySNROffset[i],L*sizeof(double));
        memcpy(tmps,in_delay[i],L*sizeof(double));
        tmpy += L;
        tmps += L;
        memcpy(tmpy,in_dataSNROffset[i],nr_samples*sizeof(double));
        memcpy(tmps,in_data[i],nr_samples*sizeof(double));
        tmpy += nr_samples;
        tmps += nr_samples;
    }

    P2_data = (double*) calloc(L1,sizeof(double)); /* Stepsize vector - part */
    R_data_real = (double*) calloc(L1tot,sizeof(double));
    R_data_imag = (double*) calloc(L1tot,sizeof(double));

    E_data_real = (double*) fftw_malloc(L1*sizeof(double)); /* Data vector (size L1) */
    E_data_imag = (double*) fftw_malloc(L1*sizeof(double));
    e_data = (double*) fftw_malloc(2*L*sizeof(double)); /* Data vector (size 2L) with first half zeroes */
    InitialiseZero(e_data,L);
    e_data2 = e_data + L;
    fftreal2 = fftreal + L;

    data_real = (double*) calloc(L1tot,sizeof(double)); /* Data matrix for various purposes */
    data_imag = (double*) calloc(L1tot,sizeof(double));
    tmp_mat_real = (double*) calloc(L1tot*nr_ref,sizeof(double)); /* Data matrix (size L1tot) for various purposes */
    tmp_mat_imag = (double*) calloc(L1tot*nr_ref,sizeof(double)); /* Data matrix (size L1tot) for various purposes */

    nr_samples += L; /* Addition of L samples in initialisation */
    offset_ref = (nr_channels-nr_ref)*nr_samples;

    inv2L = (double) 1/(2*L);
    fact = (double) rho*(1-lambda)/(2*L);
    invnr_ref = (double) 1/nr_ref;

    /*************/
    /* Algorithm */
    /*************/

    tmps = s_data;
    tmpy = y_data;

    for (k=0; k<nr_blocks; k++) {
        /*-------------------------*/
        /* Calculate output signal */
        /*-------------------------*/

        /* Lower SNR signal is only filtered */
        ref_data = tmps + offset_ref;
        tmps += L; /* Next frame */
        for (j=0; j<nr_ref; j++) {
            memcpy(fftreal,ref_data+j*nr_samples,2*L*sizeof(double));
            fftw_execute_split_dft_r2c(pFFT1_2L,fftreal,fftR,fftI);
            memcpy(data_real+j*L1,fftR,L1*sizeof(double));
            memcpy(data_imag+j*L1,fftI,L1*sizeof(double));
        }
        PointWiseProductSum_cplx_data(data_real,data_imag,W_data_real,W_data_imag,E_data_real,E_data_imag,L1,nr_ref);
        memcpy(fftR,E_data_real,L1*sizeof(double));
        memcpy(fftI,E_data_imag,L1*sizeof(double));
        fftw_execute_split_dft_c2r(pIFFT1_2L,fftR,fftI,fftreal); /* FACTOR 2L */
        MultiplyMatrix_data(fftreal2,inv2L,L);
        //SubtractMatrix_data(tmps-delay,fftreal2,zs_data,L);
        SubtractMatrix_data(tmps-delay,fftreal2,z_data,L);

        /* Noisy signal */
        ref_data = tmpy + offset_ref;
        tmpy += L; /* Next frame */

        for (j=0; j<nr_ref; j++) {
            memcpy(fftreal,ref_data+j*nr_samples,2*L*sizeof(double));
            fftw_execute_split_dft_r2c(pFFT1_2L,fftreal,fftR,fftI);
            memcpy(data_real+j*L1,fftR,L1*sizeof(double));
            memcpy(data_imag+j*L1,fftI,L1*sizeof(double));
        }

        PointWiseProductSum_cplx_data(data_real,data_imag,W_data_real,W_data_imag,E_data_real,E_data_imag,L1,nr_ref);
        memcpy(fftR,E_data_real,L1*sizeof(double));
        memcpy(fftI,E_data_imag,L1*sizeof(double));
        fftw_execute_split_dft_c2r(pIFFT1_2L,fftR,fftI,fftreal); /* FACTOR 2L */
        MultiplyMatrix_data(fftreal2,inv2L,L);
        SubtractMatrix_data(tmpy-delay,fftreal2,e_data2,L);
        //memcpy(z_data,e_data2,L*sizeof(double));

        /* Next frame */
        z_data  += L;

        /*---------------*/
        /* Noise periods */
        /*---------------*/

        if (*speech_data == 0) {

            /* Calculate error signal in frequency domain */
            memcpy(fftreal,e_data,2*L*sizeof(double));
            fftw_execute_split_dft_r2c(pFFT1_2L,fftreal,fftR,fftI);
            memcpy(E_data_real,fftR,L1*sizeof(double));
            memcpy(E_data_imag,fftI,L1*sizeof(double));

            /* Update noise correlation matrix (complex conjugate)*/
            for (i=0; i<nr_ref; i++) {
            for (j=0; j<i; j++) {
                AddPointWiseProductH_cplx_data(data_real+i*L1,data_imag+i*L1,data_real+j*L1,data_imag+j*L1,Sn_data_real+j*L1tot+i*L1,Sn_data_imag+j*L1tot+i*L1,L1,lambda,(1-lambda)/2);
                ConjugateMatrix_data(Sn_data_real+j*L1tot+i*L1,Sn_data_imag+j*L1tot+i*L1,Sn_data_real+i*L1tot+j*L1,Sn_data_imag+i*L1tot+j*L1,L1);
            }
            PointWiseProduct_cplx2_data(data_real+i*L1,data_imag+i*L1,tmp_mat_real,L1);
            MultiplyMatrix_data(Sn_data_real+i*(L1tot+L1),lambda,L1);
            AddMatrix_data(tmp_mat_real,Sn_data_real+i*(L1tot+L1),L1,(1-lambda)/2);
            }

            /* Calculate regularisation term */
            for (i=0; i<nr_ref; i++) {
                for (j=0; j<nr_ref; j++) { /* Difference between speech and noise correlation matrix */
                    SubtractMatrix_data(Sy_data_real+j*L1tot+i*L1,Sn_data_real+j*L1tot+i*L1,tmp_mat_real+j*L1,L1);
                    SubtractMatrix_data(Sy_data_imag+j*L1tot+i*L1,Sn_data_imag+j*L1tot+i*L1,tmp_mat_imag+j*L1,L1);
                }
                PointWiseProductSum_cplx_data(tmp_mat_real,tmp_mat_imag,W_data_real,W_data_imag,R_data_real+i*L1,R_data_imag+i*L1,L1,nr_ref);
            }
            MultiplyMatrix_cplx_data(R_data_real,R_data_imag,mu_inv,L1tot);

            if (step == 1) {
                /* Calculate step size */
                SubtractMatrix_data(Sy_data_real,Sn_data_real,tmp_mat_real,L1tot*nr_ref);
                SubtractMatrix_data(Sy_data_imag,Sn_data_imag,tmp_mat_imag,L1tot*nr_ref);
                MultiplyMatrix_cplx_data(tmp_mat_real,tmp_mat_imag,mu_inv,L1tot*nr_ref);
                if (negstep == 2) {
                    PositiveDefiniteBlockMatrix2_cplx_data(tmp_mat_real,tmp_mat_imag,L1); /* Make block matrix positive semi-definite */
                                                                                        /* Works only for 2 channels !!! */
                }
                AddMatrix_cplx_data(Sn_data_real,Sn_data_imag,tmp_mat_real,tmp_mat_imag,L1tot*nr_ref,1);
                BlockInverse2_cplx_data(tmp_mat_real,tmp_mat_imag,L1,delta); /* Works only for 2 channels !!! */
                MultiplyMatrix_cplx_data(tmp_mat_real,tmp_mat_imag,fact,L1tot*nr_ref);

                /* Filter update */
                for (i=0; i< nr_ref; i++) {
                    AddPointWiseProductH_cplx_data(data_real+i*L1,data_imag+i*L1,E_data_real,E_data_imag,R_data_real+i*L1,R_data_imag+i*L1,L1,1,-1);
                }
                for (i=0; i<nr_ref; i++) {
                    for (j=0; j<nr_ref; j++) {
                        memcpy(data_real+j*L1,tmp_mat_real+i*L1+j*L1tot,L1*sizeof(double));
                        memcpy(data_imag+j*L1,tmp_mat_imag+i*L1+j*L1tot,L1*sizeof(double));
                    }
                    PointWiseProductSum_cplx_data(data_real,data_imag,R_data_real,R_data_imag,E_data_real,E_data_imag,L1,nr_ref);

                    if (constr == 1) {
                        memcpy(fftR,E_data_real,L1*sizeof(double));
                        memcpy(fftI,E_data_imag,L1*sizeof(double));
                        ConstrainedFreq2_cplx_data(fftR,fftI,fftreal,2*L,1,pFFT1_2L,pIFFT1_2L);
                        memcpy(E_data_real,fftR,L1*sizeof(double));
                        memcpy(E_data_imag,fftI,L1*sizeof(double));
                    } else {
                        MultiplyMatrix_cplx_data(E_data_real,E_data_imag,L,L1); /* Factor 2L due to fftw-routine * Factor 0.5 */
                    }

                    SubtractMatrix_cplx_data(E_data_real,E_data_imag,W_data_real+i*L1,W_data_imag+i*L1,L1);
                }
            }

            if (step == 2) {
                for (i=0; i< nr_ref; i++) {
                    /* Calculate step size */
                    SubtractMatrix_data(Sy_data_real+i*L1tot+i*L1,Sn_data_real+i*L1tot+i*L1,P_data,L1);
                    MultiplyMatrix_data(P_data,mu_inv,L1);
                    if (negstep == 1) {
                        AbsoluteMatrix_data(P_data,L1);
                    }
                    if (negstep == 2) {
                        Negative2ZeroMatrix_data(P_data,L1);
                    }
                    AddMatrix_data(Sn_data_real+i*L1tot+i*L1,P_data,L1,1);

                    /* Filter update */
                    AddPointWiseProductH_cplx_data(data_real+i*L1,data_imag+i*L1,E_data_real,E_data_imag,R_data_real+i*L1,R_data_imag+i*L1,L1,1,-1);

                    PointWiseDivisionSpecial_cplx_data(P_data,R_data_real+i*L1,R_data_imag+i*L1,fact,delta,L1);

                    if (constr == 1) {
                        memcpy(fftR,R_data_real+i*L1,L1*sizeof(double));
                        memcpy(fftI,R_data_imag+i*L1,L1*sizeof(double));
                        ConstrainedFreq2_cplx_data(fftR,fftI,fftreal,2*L,1,pFFT1_2L,pIFFT1_2L);
                        memcpy(R_data_real+i*L1,fftR,L1*sizeof(double));
                        memcpy(R_data_imag+i*L1,fftI,L1*sizeof(double));
                    }
                    else {
                        MultiplyMatrix_cplx_data(R_data_real+i*L1,R_data_imag+i*L1,L,L1); /* Factor 2L due to fftw-routine * Factor 0.5 */
                    }

                    SubtractMatrix_cplx_data(R_data_real+i*L1,R_data_imag+i*L1,W_data_real+i*L1,W_data_imag+i*L1,L1);
                }
            }

            if (step == 3 || step == 4) {
                /* Calculate step size */
                InitialiseZero(P2_data,L1); /* Only noise (can be used 2 times) */
                for (i=0; i<nr_ref; i++) {
                    AddMatrix_data(Sn_data_real+i*L1tot+i*L1,P2_data,L1,1);
                }
                InitialiseZero(P_data,L1);
                for (i=0; i<nr_ref; i++) {
                    AddMatrix_data(Sy_data_real+i*L1tot+i*L1,P_data,L1,1);
                }
                AddMatrix_data(P2_data,P_data,L1,-1);
                MultiplyMatrix_data(P_data,mu_inv,L1);
                if (negstep == 1) {
                    AbsoluteMatrix_data(P_data,L1);
                }
                if (negstep == 2) {
                    Negative2ZeroMatrix_data(P_data,L1);
                }
                AddMatrix_data(P2_data,P_data,L1,1);

                if (step == 4) {
                    MultiplyMatrix_data(P_data,invnr_ref,L1); /* normalisation (same magnitude as step=3) */
                }

                /* Filter update */
                for (i=0; i< nr_ref; i++) {
                    AddPointWiseProductH_cplx_data(data_real+i*L1,data_imag+i*L1,E_data_real,E_data_imag,R_data_real+i*L1,R_data_imag+i*L1,L1,1,-1);

                    PointWiseDivisionSpecial_cplx_data(P_data,R_data_real+i*L1,R_data_imag+i*L1,fact,delta,L1);

                    if (constr == 1) {
                        memcpy(fftR,R_data_real+i*L1,L1*sizeof(double));
                        memcpy(fftI,R_data_imag+i*L1,L1*sizeof(double));
                        ConstrainedFreq2_cplx_data(fftR,fftI,fftreal,2*L,1,pFFT1_2L,pIFFT1_2L);
                        memcpy(R_data_real+i*L1,fftR,L1*sizeof(double));
                        memcpy(R_data_imag+i*L1,fftI,L1*sizeof(double));
                    }
                    else {
                        MultiplyMatrix_cplx_data(R_data_real+i*L1,R_data_imag+i*L1,L,L1); /* Factor 2L due to fftw-routine * Factor 0.5 */
                    }

                    SubtractMatrix_cplx_data(R_data_real+i*L1,R_data_imag+i*L1,W_data_real+i*L1,W_data_imag+i*L1,L1);
                }
            }
        }

        /*----------------*/
        /* Speech periods */
        /*----------------*/

        else if (*speech_data == 1) {

            /* Update speech correlation matrix (complex conjugate)*/
            for (i=0; i<nr_ref; i++) {
                for (j=0; j<i; j++) {
                    AddPointWiseProductH_cplx_data(data_real+i*L1,data_imag+i*L1,data_real+j*L1,data_imag+j*L1,Sy_data_real+j*L1tot+i*L1,Sy_data_imag+j*L1tot+i*L1,L1,lambda,(1-lambda)/2);
                    ConjugateMatrix_data(Sy_data_real+j*L1tot+i*L1,Sy_data_imag+j*L1tot+i*L1,Sy_data_real+i*L1tot+j*L1,Sy_data_imag+i*L1tot+j*L1,L1);
                }
                PointWiseProduct_cplx2_data(data_real+i*L1,data_imag+i*L1,tmp_mat_real,L1);
                MultiplyMatrix_data(Sy_data_real+i*(L1tot+L1),lambda,L1);
                AddMatrix_data(tmp_mat_real,Sy_data_real+i*(L1tot+L1),L1,(1-lambda)/2);
            }
        }
        /* Next frame */
        speech_data += L;
    }
    /* Memory deallocation */

    free(y_data);
    free(P2_data);
    free(R_data_real);
    free(R_data_imag);
    fftw_free(E_data_real);
    fftw_free(E_data_imag);
    fftw_free(e_data);

    free(data_real);
    free(data_imag);
    free(tmp_mat_real);
    free(tmp_mat_imag);
}

} // namespace

namespace syl
{

class AdaptiveWienerFilterPrivate
{
public:
    bool synchronous;
    unsigned sampleRate;
    unsigned length;
    unsigned channels;
    unsigned position;

    unsigned refs;
    unsigned optionStep;
    unsigned delayValue;
    double rho;
    double muInv;
    double gamma;
    double lambda;

    QList<QVector<double> > delay;
    QVector<double*> delayPtr;
    QList<QVector<double> > delaySnr;
    QVector<double*> delaySnrPtr;

    QVector<double> wDataReal;
    QVector<double> wDataImag;
    QVector<double> pData;
    QVector<double> sData[4];

    fftw_plan pFFT1_2L;
    fftw_plan pIFFT1_2L;
    double *fftreal;
    double *fftR;
    double *fftI;
};

// AdaptiveWienerFilter ========================================================

AdaptiveWienerFilter::AdaptiveWienerFilter (unsigned sampleRate, unsigned
        channels, unsigned length, double muInv, bool synchronous) :
    d (new AdaptiveWienerFilterPrivate)
{
    d->synchronous = synchronous;

    d->sampleRate = sampleRate;
    d->length = length;
    d->channels = channels;
    d->position = 0;

    // TODO implement and verify use_w0
    if (false) {// use_w0
        d->refs = 2;
        d->optionStep = 1;
    } else {
        d->refs = 1;
        d->optionStep = 2;
    }
    d->delayValue = length / 2 - 1;
    d->rho = 1.5;
    d->muInv = muInv;
    d->gamma = 0.95;
    // factor of 2: SDW_lambdasec
    d->lambda = 1 - 1 / (2 * double (sampleRate) / length);

    d->delayPtr.resize (channels);
    d->delaySnrPtr.resize (channels);
    for (unsigned i = 0; i < channels; ++i) {
        d->delay.append (QVector<double> (length));
        d->delaySnr.append (QVector<double> (length));
        d->delayPtr[i] = d->delay[i].data();
        d->delaySnrPtr[i] = d->delaySnr[i].data();
    }

    d->wDataReal.resize (d->refs * (length + 1));
    d->wDataImag.resize (d->refs * (length + 1));
    d->pData.resize (length + 1);
    d->sData[0].resize (d->refs * d->refs * (length + 1));
    d->sData[1].resize (d->refs * d->refs * (length + 1));
    d->sData[2].resize (d->refs * d->refs * (length + 1));
    d->sData[3].resize (d->refs * d->refs * (length + 1));

    int rank = 1;
    int howmany_rank = 1;
    fftw_iodim dims[1];
    fftw_iodim howmany_dims[1];
    dims[0].n = 2 * length; /* Size of FFT */
    dims[0].is = 1;         /* Input stride */
    dims[0].os = 1;         /* Output stride */
    howmany_dims[0].n = 1;  /* Number of transforms */
    howmany_dims[0].is = 1; /* Input stride */
    howmany_dims[0].os = 1; /* Output stride */

    d->fftreal = (double*) fftw_malloc (2 * length * sizeof (double));
    d->fftR = (double*) fftw_malloc ((length + 1) * sizeof (double));
    d->fftI = (double*) fftw_malloc ((length + 1) * sizeof (double));

    d->pFFT1_2L  = fftw_plan_guru_split_dft_r2c(rank, dims, howmany_rank,
            howmany_dims, d->fftreal, d->fftR, d->fftI, FFTW_ESTIMATE);
    d->pIFFT1_2L = fftw_plan_guru_split_dft_c2r(rank, dims, howmany_rank,
            howmany_dims, d->fftR, d->fftI, d->fftreal, FFTW_ESTIMATE);
}

AdaptiveWienerFilter::~AdaptiveWienerFilter()
{
    fftw_destroy_plan (d->pFFT1_2L);
    fftw_destroy_plan (d->pIFFT1_2L);
    fftw_free (d->fftreal);
    fftw_free (d->fftR);
    fftw_free (d->fftI);
}

unsigned AdaptiveWienerFilter::length() const
{
    return d->length;
}

unsigned AdaptiveWienerFilter::position() const
{
    return d->position;
}

// TODO: seperate update of coefficients and filtering, reuse FirFilter
// TODO: needs a better testbench for all the different parameters
void AdaptiveWienerFilter::process (double * const *data,
        const double * const *dataSnr, const double *vad, unsigned dataLength)
{
    if (!dataLength)
        return;

    // FIXME support for delayed operation
    Q_ASSERT (d->synchronous);

    Q_ASSERT (!d->synchronous || dataLength % d->length == 0);

    QVector<double> output (d->length);
    QVector<double*> dataPtr (d->channels);
    QVector<const double*> dataSnrPtr (d->channels);

    for (unsigned pos = 0; pos < dataLength; pos += d->length) {
        for (unsigned i = 0; i < d->channels; ++i) {
            dataPtr[i] = data[i] + pos;
            dataSnrPtr[i] = dataSnr[i] + pos;
        }
        const double * const vadPtr = vad + pos;

        SDW_MWF_freqlmsLP6b_stepFD (dataPtr.data(), dataSnrPtr.data(),
                d->delayPtr.data(), d->delaySnrPtr.data(), output.data(),
                vadPtr, d->length, d->channels, d->refs, d->length,
                d->delayValue, d->rho, d->muInv, d->gamma, d->lambda,
                d->wDataReal.data(), d->wDataImag.data(), d->pData.data(),
                d->sData[0].data(), d->sData[1].data(), d->sData[2].data(),
                d->sData[3].data(), d->pFFT1_2L, d->pIFFT1_2L, d->fftreal,
                d->fftR, d->fftI,
                d->optionStep, /* StepsizeSelect - step in Matlab code */
                0, /* 0 - Unconstrained, 1 - Constrained prefered!!! */
                2,/* negstep, LEAVE ALWAYS 2 !!!! */
                1e-6); /* factor 2^30 compared to MATLAB code (1e-6)delta) */

        for (unsigned i = 0; i < d->channels; ++i) {
            memcpy (d->delay[i].data(), dataPtr[i],
                    d->length * sizeof (double));
            memcpy (d->delaySnr[i].data(), dataSnrPtr[i],
                    d->length * sizeof (double));
        }
        memcpy (dataPtr[0], output.data(), d->length * sizeof (double));
    }

    d->position += dataLength;
}

} // namespace syl
