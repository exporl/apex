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
#ifndef __PRIORS_C__
#define __PRIORS_C__

#include "universalprefix.h"
#include "mathheader.h"

#include "psychometric.h"
#include "priors.h"

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

char gPriorString[128];

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double DiffLogPrior(double arg, double diff_arg, ConstraintPtr W)
{
	double W0, W1;
	
	if(W == NULL || W->prior == NULL) return 0.0;
	W0 = (*(W->prior))(evaluatePrior, W->args, arg, 0);
	W1 = (*(W->prior))(evaluatePrior, W->args, arg, 1);
	
	return W1 * diff_arg / W0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double Diff2LogPrior(double arg, double da_du, double da_dv, double d2a_dudv, ConstraintPtr W)
{
	double W0, W1, W2;
	
	if(W == NULL || W->prior == NULL) return 0.0;
	W0 = (*(W->prior))(evaluatePrior, W->args, arg, 0);
	W1 = (*(W->prior))(evaluatePrior, W->args, arg, 1);
	W2 = (*(W->prior))(evaluatePrior, W->args, arg, 2);

	if(W0 == 0.0) return NAN;
	return (da_du * da_dv * (W2 - W1 * W1 / W0) + d2a_dudv * W1) / W0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int ReportPrior(char *s, ConstraintPtr c)
{
	if(c == NULL || c->prior == NULL) return 0;
	return printf(PriorDescription(c), s);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void SetPrior(ConstraintPtr constraint, BayesianPriorPtr prior, unsigned short nArgs, double *args)
{
	unsigned short nArgsExpected;

	constraint->prior = prior;
	if(prior == NULL) return;	
	nArgsExpected = ExpectedNumberOfPriorArgs(constraint);
	if(nArgs != nArgsExpected) JError("%s prior takes %hu numerical arguments - received %hu", PriorName(constraint), nArgsExpected, nArgs);
	CopyVals(constraint->args, args, nArgs, sizeof(double));
	VerifyPriorArgs(constraint);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#ifndef lgamma
#define lgamma(x)		(0.0)
/* Oh dear, lgamma doesn't seem to be ANSI, or at least not old enough to be counted on,
and I can't find a C source for it anywhere so the prior will have to remain unscaled. This
shouldn't make any difference in theory although it may cause loss of precision in practice. */
#endif
double BetaPrior(PriorMode mode, double *args, double val, unsigned short diff)
{
	double temp, scale, lo, hi, z, w, b, y;
	int i;
	
	switch(mode) {
		case evaluatePrior:
			if (val < (lo = args[0]) || val > (hi = args[1])) return 0.0;
			z = args[2]; w = args[3];
			val -= lo;
			scale = 1.0 / (hi - lo);
			val *= scale;
			b = lgamma(z) + lgamma(w) - lgamma(z + w);
			switch(diff) {
				case 0:
					y = xlogy_j(z - 1.0, val) + xlogy_j(w - 1.0, 1.0 - val) - b;
					return exp(y);
				case 1:
					y = 0.0;
					temp = xlogy_j(z - 2.0, val) + xlogy_j(w - 1.0, 1.0 - val) - b;
					y += exp(temp) * (z - 1.0);
					temp = xlogy_j(z - 1.0, val) + xlogy_j(w - 2.0, 1.0 - val) - b;
					y += exp(temp) * (1.0 - w);
					return scale * y;
				case 2:
					y = 0.0;
					temp = xlogy_j(z - 3.0, val) + xlogy_j(w - 1.0, 1.0 - val) - b;
					y += exp(temp) * (z - 1.0) * (z - 2.0);
					temp = xlogy_j(z - 2.0, val) + xlogy_j(w - 2.0, 1.0 - val) - b;
					y += exp(temp) * (z - 1.0) * (1.0 - w) * 2.0;
					temp = xlogy_j(z - 1.0, val) + xlogy_j(w - 3.0, 1.0 - val) - b;
					y += exp(temp) * (1.0 - w) * (2.0 - w);
					return scale * scale * y;
			}
			return NAN;
		case getWorkingLimit:
			return ((val < 0.0) ? args[0] : args[1]);
		case nArgsForPrior:
			return 4.0;
		case verifyPriorArgs:
			for(i = 0; i < 4; i++) {
				if(isnan(args[i])) JError("beta prior arguments cannot be NaN");
				if(isinf(args[i])) JError("beta prior arguments cannot be infinite");
			}
			if(args[0] > args[1]) temp = args[0], args[0] = args[1], args[1] = temp;
			if(args[2] <= 0.0 || args[3] <= 0.0) JError("beta prior arguments 3 and 4 must be positive");
			return 1.0;
		case namePrior:
			sprintf(gPriorString, "beta");
			return 1.0;
		case describePrior:
			sprintf(gPriorString, "%%s is constrained within [%lg, %lg] using a beta function with params (%lg, %lg)", args[0], args[1], args[2], args[3]);
			return 1.0;
		default: Bug("unknown mode %d in Bayesian prior function", (int)mode);
	}
	return NAN;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double CosinePrior(PriorMode mode, double *args, double val, unsigned short diff)
{
	double temp, lo, hi;
	
	switch(mode) {
		case evaluatePrior:
			if (val < (lo = args[0]) || val > (hi = args[1])) return 0.0;
			temp = 0.5 * (lo + hi);
			val -= temp;
			temp = pi / (hi - temp);
			val *= temp;	
			switch(diff) {
				case 0: return 0.5 + 0.5 * cos(val);
				case 1: return -0.5 * temp * sin(val);
				case 2: return -0.5 * temp * temp * cos(val);
			}
			return NAN;
		case getWorkingLimit:
			return ((val < 0.0) ? args[0] : args[1]);
		case nArgsForPrior:
			return 2.0;
		case verifyPriorArgs:
			if(isnan(args[0]) || isnan(args[1])) JError("cosine prior arguments cannot be NaN");
			if(isinf(args[0]) || isinf(args[1])) JError("cosine prior arguments cannot be infinite");
			if(args[0] > args[1]) temp = args[0], args[0] = args[1], args[1] = temp;
			return 1.0;
		case namePrior:
			sprintf(gPriorString, "raised cosine");
			return 1.0;
		case describePrior:
			sprintf(gPriorString, "%%s is constrained using a raised cosine within [%lg, %lg]", args[0], args[1]);
			return 1.0;
		default: Bug("unknown mode %d in Bayesian prior function", (int)mode);
	}
	return NAN;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double FlatPrior(PriorMode mode, double *args, double val, unsigned short diff)
{
	double temp;
	
	switch(mode) {
		case evaluatePrior:
			if(diff > 0) return 0.0;
			return (val < args[0] || val > args[1]) ? 0.0 : 1.0;
		case getWorkingLimit:
			return ((val < 0.0) ? args[0] : args[1]);
		case nArgsForPrior:
			return 2.0;
		case verifyPriorArgs:
			if(isnan(args[0]) || isnan(args[1])) JError("flat prior arguments cannot be NaN");
			if(args[0] > args[1]) temp = args[0], args[0] = args[1], args[1] = temp;
			return 1.0;
		case namePrior:
			sprintf(gPriorString, "flat");
			return 1.0;
		case describePrior:
			sprintf(gPriorString, "%%s is constrained within [%lg, %lg]", args[0], args[1]);
			return 1.0;
		default: Bug("unknown mode %d in Bayesian prior function", (int)mode);
	}
	return NAN;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double GaussianPrior(PriorMode mode, double *args, double val, unsigned short diff)
{
	double temp, mu, sigma, y, dydx, d2ydx2;
	
	switch(mode) {
		case evaluatePrior:
			mu = args[0]; sigma = args[1];
			temp = (val - mu) / sigma;
			y = exp(-0.5 * temp * temp);
/*			y /= (sigma * sqrt(2.0 * pi));
*/			if(diff == 0) return y;
			temp = sigma * sigma;
			dydx = y * (mu - val) / temp;
			if(diff == 1) return dydx;
			d2ydx2 = ((mu - val) * dydx - y) / temp;
			if(diff == 2) return d2ydx2;
			return NAN;
		case getWorkingLimit:
			return args[0] + args[1] * 3.0 * val;
		case nArgsForPrior:
			return 2.0;
		case verifyPriorArgs:
			if(isnan(args[0]) || isnan(args[1])) JError("Gaussian prior arguments cannot be NaN");
			if(isinf(args[0]) || isinf(args[1])) JError("Gaussian prior arguments cannot be infinite");
			if(args[1] <= 0.0) JError("standard deviation of Gaussian prior cannot be <= 0");
			return 1.0;
		case namePrior:
			sprintf(gPriorString, "Gaussian");
			return 1.0;
		case describePrior:
			sprintf(gPriorString, "%%s is constrained using a Gaussian prior with mean = %lg, std = %lg", args[0], args[1]);
			return 1.0;
		default: Bug("unknown mode %d in Bayesian prior function", (int)mode);
	}
	return NAN;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#endif	/* __PRIORS_C__ */
