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
#ifndef __PSYCHOMETRIC_H__
#define __PSYCHOMETRIC_H__

typedef enum{solve, derivative, threshold_derivative, slope_derivative, regression_transform, legal, limits, functionName} PsychSolveMode;
typedef short ArgIdentifier;
enum {ALPHA = 0, BETA, GAMMA, LAMBDA, X, F,
		ALPHA_FROM_SHIFT_AND_SLOPE, BETA_FROM_SHIFT_AND_SLOPE, DFDX, NONE};

typedef double(PsychDistribFunc)(double, double, double, double, PsychSolveMode, ArgIdentifier);
typedef PsychDistribFunc * PsychDistribFuncPtr;

#define prob(shape, a, b, x)		((*(shape))(0.0, (x), (a), (b), solve, F))
#define inv_prob(shape, a, b, f)	((*(shape))((f), 0.0, (a), (b), solve, X))
#define slope(shape, a, b, f)		((*(shape))((f), 0.0, (a), (b), solve, DFDX))
#define diff_prob(shape, a, b, x)	((*(shape))(0.0, (x), (a), (b), derivative, X))
#define get_alpha(shape, t, s, f)	((*(shape))((f), 0.0, (t), (s), solve, ALPHA_FROM_SHIFT_AND_SLOPE))
#define get_beta(shape, t, s, f)	((*(shape))((f), 0.0, (t), (s), solve, BETA_FROM_SHIFT_AND_SLOPE))

#define rtx(shape, x)				((*(shape))(0.0, (x), 0.0, 0.0, regression_transform, X))
#define rtf(shape, f)				((*(shape))((f), 0.0, 0.0, 0.0, regression_transform, F))
#define rtcm_a(shape, c, m)			((*(shape))(0.0, 0.0, (c), (m), regression_transform, ALPHA_FROM_SHIFT_AND_SLOPE))
#define rtcm_b(shape, c, m)			((*(shape))(0.0, 0.0, (c), (m), regression_transform, BETA_FROM_SHIFT_AND_SLOPE))

#define get_limits(shape, what)		((*(shape))(0.0, 0.0, 0.0, 0.0, limits, what))
#define legal_gradient(shape, dfdx)	((*(shape))((dfdx), 0.0, 0.0, 0.0, legal, DFDX) != 0.0)
#define legal_x(shape, x)			((*(shape))(0.0, (x), 0.0, 0.0, legal, X) != 0.0)
#define legal_alpha(shape, a)		((*(shape))(0.0, 0.0, (a), 0.0, legal, ALPHA) != 0.0)
#define legal_beta(shape, b)		((*(shape))(0.0, 0.0, 0.0, (b), legal, BETA) != 0.0)

typedef struct {
	double min;
	double max;
} SearchLimits;
typedef SearchLimits * SearchLimitsPtr;
extern SearchLimits gLegal;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

PsychDistribFunc JCumulativeGaussian, JGumbel, JLinear, JLogistic, JWeibull;
boolean DoubleDiff(ArgIdentifier wrt, ArgIdentifier *wrt1, ArgIdentifier *wrt2);
char *FunctionName(PsychDistribFuncPtr f);
void TestPF(PsychDistribFuncPtr f, double x);
#define wrt_both(u, v)	((ArgIdentifier)(((short)1 << 15) + ((short)(u) << 8) + (short)(v)))

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

#endif /* __PSYCHOMETRIC_H__ */
