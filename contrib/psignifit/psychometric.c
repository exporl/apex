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
#ifndef __PSYCHOMETRIC_C__
#define __PSYCHOMETRIC_C__

#include "universalprefix.h"
#include "mathheader.h"

#include <string.h>
#include "psychometric.h"

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

char *gFunctionName;
SearchLimits gLegal;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
char *FunctionName(PsychDistribFuncPtr f)
{
	(void)(*f)(0.0, 0.0 ,0.0 ,0.0, functionName, F);
	return gFunctionName;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void TestPF(PsychDistribFuncPtr f, double x)
{
	double y, Q = 0.0;
	
	reports(FunctionName(f));
	
	report(f(Q, 3, 4, x, solve, F));
	report(f(Q, 3, 4, x, derivative, X));
	report(f(Q, 3, 4, x, derivative, ALPHA));
	report(f(Q, 3, 4, x, derivative, BETA));
	report(f(Q, 3, 4, x, derivative, wrt_both(ALPHA, ALPHA)));
	report(f(Q, 3, 4, x, derivative, wrt_both(ALPHA, BETA)));
	report(f(Q, 3, 4, x, derivative, wrt_both(BETA, ALPHA)));
	report(f(Q, 3, 4, x, derivative, wrt_both(BETA, BETA)));

	report(f(0.7, Q, 4, x, threshold_derivative, ALPHA));
	report(f(0.7, Q, 4, x, threshold_derivative, BETA));
	report(f(0.7, Q, 4, x, threshold_derivative, wrt_both(ALPHA, ALPHA)));
	report(f(0.7, Q, 4, x, threshold_derivative, wrt_both(ALPHA, BETA)));
	report(f(0.7, Q, 4, x, threshold_derivative, wrt_both(BETA, ALPHA)));
	report(f(0.7, Q, 4, x, threshold_derivative, wrt_both(BETA, BETA)));

	report(f(0.7, Q, 4, x, slope_derivative, ALPHA));
	report(f(0.7, Q, 4, x, slope_derivative, BETA));
	report(f(0.7, Q, 4, x, slope_derivative, wrt_both(ALPHA, ALPHA)));
	report(f(0.7, Q, 4, x, slope_derivative, wrt_both(ALPHA, BETA)));
	report(f(0.7, Q, 4, x, slope_derivative, wrt_both(BETA, ALPHA)));
	report(f(0.7, Q, 4, x, slope_derivative, wrt_both(BETA, BETA)));

	report(y = f(Q, 3, 4, x, solve, F));
	report(f(y, Q, 4, x, solve, X));
	report(f(0.7, NAN, 4, x, solve, DFDX));
	report(f(y, 3, Q, x, solve, ALPHA));
	report(f(y, 3, 4, Q, solve, BETA));

	report(inv_prob(f, get_alpha(f, 3, 4, 0.5), get_beta(f, 3, 4, 0.5), 0.5));
	report(slope(f, get_alpha(f, 3, 4, 0.5), get_beta(f, 3, 4, 0.5), 0.5));
	report(x);

}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean DoubleDiff(ArgIdentifier wrt, ArgIdentifier *wrt1, ArgIdentifier *wrt2)
{
	short input, mask, part1, part2;
	
	input = (short)wrt;
	mask = (short)1 << 15;
	if((input & mask) == 0) {
		*wrt1 = *wrt2 = wrt;
		return FALSE;
	}
	mask = (short)0x7F << 8;
	part1 = ((input & mask) >> 8);
	mask = (short)0x7F;
	part2 = (input & mask);
	
	if(part1 < part2) {*wrt1 = (ArgIdentifier)part1; *wrt2 = (ArgIdentifier)part2;}
	else {*wrt1 = (ArgIdentifier)part2; *wrt2 = (ArgIdentifier)part1;}
	return TRUE;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*/// DISTRIBUTION FUNCTIONS  F(x, alpha, beta):  range must be [0,1], and F(x, x, beta) must be invariant of beta  /////*/
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double JCumulativeGaussian(double f, double x, double a, double b, PsychSolveMode mode, ArgIdentifier wrt)
{
	double xma, bsq, gauss;
	static char name[] = "cumulative Gaussian";	

	switch(mode) {
		case solve:
			switch(wrt) {
                case F: return f = 0.5 + 0.5 * erf_pf((x - a) / (sqrt(2.0) * b));
				case X: return x = a + sqrt(2.0) * b * erf_inv(2.0 * f - 1.0);
				case DFDX: return f = exp(-pow(erf_inv(2.0 * f - 1.0), 2.0)) / (b * sqrt(2.0 * pi));
				case ALPHA: return a = x - sqrt(2.0) * b * erf_inv(2.0 * f - 1.0);
				case BETA: return b = (x - a) / (sqrt(2.0) * erf_inv(2.0 * f - 1.0));
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = a - exp(-pow(erf_inv(2.0 * f - 1.0), 2.0)) * erf_inv(2.0 * f - 1.0) / (b * sqrt(pi));
				case BETA_FROM_SHIFT_AND_SLOPE: return b = exp(-pow(erf_inv(2.0 * f - 1.0), 2.0)) / (b * sqrt(2.0 * pi));
			}
		case derivative:
			xma = x - a;
			bsq = b * b;
			gauss = exp(-xma * xma / (2 * bsq)) / (b * sqrt(2.0 * pi));
			switch(wrt) {
				case X: return f = gauss;
				case ALPHA: return f = -gauss;
				case BETA: return f =  -xma * gauss / b;
				case wrt_both(ALPHA, ALPHA):
					return f = -xma * gauss / bsq;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = (b - xma) * (b + xma) * gauss / (bsq * b);
				case wrt_both(BETA, BETA):
					return f = (2.0 - xma * xma / bsq) * xma * gauss / bsq;
			}
		case threshold_derivative:
			switch(wrt) {
				case ALPHA: return f = 1.0;
				case BETA: return f = sqrt(2.0) * erf_inv(2.0 * f - 1.0);
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = 0.0;
			}
		case slope_derivative:
			switch(wrt) {
				case ALPHA: return f = 0.0;
				case BETA: return f = -exp(-pow(erf_inv(2.0 * f - 1.0), 2.0)) / (b * b * sqrt(2.0 * pi));
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = sqrt(2.0 / pi) * exp(-pow(erf_inv(2.0 * f - 1.0), 2.0)) / (b * b * b);
			}
		case regression_transform:
			switch(wrt) {
				case X: return x = x;
				case F: return f = erf_inv(2.0 * f - 1.0);
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = -a / b;
				case BETA_FROM_SHIFT_AND_SLOPE: return b = sqrt(0.5) / b;
			}
		case legal: return 1.0;
		case limits: return (gLegal.min = -INF, gLegal.max = INF, 0.0);
		case functionName: gFunctionName = name; return 0.0;
	}
	Bug("unknown mode (%d, %d) in psychometric function", (int)mode, (int)wrt);
	return 0.0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double JGumbel(double f, double x, double a, double b, PsychSolveMode mode, ArgIdentifier wrt)
{
	double xma, exb, eab, exab;
	static char name[] = "Gumbel";	
	
	switch(mode) {
		case solve:
			switch(wrt) {
				case F: return f = 1 - exp(-exp((x - a) / b));
				case X: return x = a + b * log_j(-log_j(1.0 - f));
				case DFDX: return f = (f - 1.0) * log_j(1.0 - f) / b;
				case ALPHA: return a = x - b * log_j(-log_j(1.0 - f));
				case BETA: return b = (x - a) / log_j(-log_j(1.0 - f));
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = a - log_j(-log_j(1.0 - f)) * (f - 1.0) * log_j(1.0 - f) / b;
				case BETA_FROM_SHIFT_AND_SLOPE: return b = (f - 1.0) * log_j(1.0 - f) / b;
			}
		case derivative:
			xma = x - a;
			exb = exp(x / b);
			eab = exp(a / b);
			exab = exb / eab;
			switch(wrt) {
				case X: return f = exp(-exab + xma / b) / b;
				case ALPHA: return f = -exp(-exab + xma / b) / b;
				case BETA: return f = -xma * exp(-exab + xma / b) / (b * b);
				case wrt_both(ALPHA, ALPHA):
					return f = exp((xma - a) / b - exab) * (eab - exb) / (b * b);
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = exp((xma - a) / b - exab) * ((xma + b) * eab - xma * exb) / (b * b * b);
				case wrt_both(BETA, BETA):
					return f =  exp((xma - a) / b - exab) * xma * ((xma + b + b) * eab - xma * exb) / (b * b * b * b);
			}
		case threshold_derivative:
			switch(wrt) {
				case ALPHA: return f = 1.0;
				case BETA: return f =  log_j(-log_j(1.0 - f));
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = 0.0;
			}
		case slope_derivative:
			switch(wrt) {
				case ALPHA: return f = 0.0;
				case BETA: return f = (1.0 - f) * log_j(1.0 - f) / (b * b);
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = 2.0 * (f - 1.0) * log_j(1.0 - f) / (b * b * b);
			}
		case regression_transform:
			switch(wrt) {
				case X: return x = x;
				case F: return f = log_j(-log_j(1.0 - f));
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = -a / b;
				case BETA_FROM_SHIFT_AND_SLOPE: return b = 1.0 / b;
			}
		case legal: return 1.0;
		case limits: return (gLegal.min = -INF, gLegal.max = INF, 0.0);
		case functionName: gFunctionName = name; return 0.0;
	}
	Bug("unknown mode (%d, %d) in psychometric function", (int)mode, (int)wrt);
	return 0.0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double JLinear(double f, double x, double a, double b, PsychSolveMode mode, ArgIdentifier wrt)
{
	static char name[] = "linear";	
	switch(mode) {
		case solve:
			switch(wrt) {
				case F:	return ((f = (x - a) * b + 0.5) < 0.0) ? (f = 0.0) : (f > 1.0) ? (f = 1.0) : f;
				case X: return x = a + (f - 0.5) / b;
				case DFDX: return f = b;
				case ALPHA: return a = x - (f - 0.5) / b;
				case BETA:	return b = (f - 0.5) / (x - a);
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = a - (f - 0.5) / b;
				case BETA_FROM_SHIFT_AND_SLOPE: return b = b;
			}
		case derivative:
			if(fabs((x - a) * b) > 0.5) return 0.0; /* clipped */
			switch(wrt) {
				case X: return f = b;
				case ALPHA: return f = -b;
				case BETA: return f = x - a;
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = -1.0;
				case wrt_both(BETA, BETA):
					return f = 0.0;
			}
		case threshold_derivative:
			switch(wrt) {
				case ALPHA: return f = 1.0;
				case BETA: return f =  (0.5 - f) / (b * b);
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = (2.0 * f - 1.0) / (b * b * b);
			}
		case slope_derivative:
			switch(wrt) {
				case ALPHA: return f = 0.0;
				case BETA: return f = 1.0;
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = 0.0;
			}
		case regression_transform:
			switch(wrt) {
				case X: return x = x;
				case F: return f = f;
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = (0.5 - a) / b;
				case BETA_FROM_SHIFT_AND_SLOPE: return b = b;
			}
		case legal: return 1.0;
		case limits: return (gLegal.min = -INF, gLegal.max = INF, 0.0);
		case functionName: gFunctionName = name; return 0.0;
	}
	Bug("unknown mode (%d, %d) in psychometric function", (int)mode, (int)wrt);
	return 0.0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double JLogistic(double f, double x, double a, double b, PsychSolveMode mode, ArgIdentifier wrt)
{
	double eab, exb, eaxb;
	static char name[] = "logistic";	
	
	switch(mode) {
		case solve:
			switch(wrt) {
				case F:	return f = 1.0 / (1.0 + exp((a - x) / b));
				case X: return x = a + b * log_j(f / (1.0 - f));
				case DFDX: return f = f * (1.0 - f) / b;
				case ALPHA: return a = x - b * log_j(f / (1.0 - f));
				case BETA:	return b = (x - a) / log_j(f / (1.0 - f));
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = a + log_j((1.0 - f) / f) * f * (1.0 - f) / b;
				case BETA_FROM_SHIFT_AND_SLOPE: return b = f * (1.0 - f) / b;
			}
		case derivative:
			eab = exp(a / b);
			exb = exp(x / b);
			eaxb = eab / exb;
			switch(wrt) {
				case X: return f = eaxb / (b * (eaxb + 1.0) * (eaxb + 1.0));
				case ALPHA: return f = - eaxb / (b * (eaxb + 1.0) * (eaxb + 1.0));
				case BETA: return f = eaxb * (a - x) / (b * b * (eaxb + 1.0) * (eaxb + 1.0));
				case wrt_both(ALPHA, ALPHA):
					return f = exp((a + x) / b) * (eab - exb) / (b * b * pow(eab + exb, 3.0));
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = exp((a + x) / b) * ((x + b - a) * eab + (a + b - x) * exb) / pow(b * (eab + exb), 3.0);
				case wrt_both(BETA, BETA):
					return f = (x - a) * exp((a + x) / b) * ((x - a + b + b) * eab + (a - x + b + b) * exb) / (b * pow(b * (eab + exb), 3.0));
			}
		case threshold_derivative:
			switch(wrt) {
				case ALPHA: return f = 1.0;
				case BETA: return f = -log_j(1.0 / f - 1.0);
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = 0.0;
			}
		case slope_derivative:
			switch(wrt) {
				case ALPHA: return f = 0.0;
				case BETA: return f = (f - 1.0) * f / (b * b);
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = 0.0;
				case wrt_both(BETA, BETA):
					return f = 2.0 * (1.0 - f) * f / (b * b * b);
			}
		case regression_transform:
			switch(wrt) {
				case X: return x = x;
				case F: return f = -log_j(1.0 / f - 1.0);
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = -a / b;
				case BETA_FROM_SHIFT_AND_SLOPE: return b = 1.0 / b;
			}
		case legal: return 1.0;
		case limits: return (gLegal.min = -INF, gLegal.max = INF, 0.0);
		case functionName: gFunctionName = name; return 0.0;
	}
	Bug("unknown mode (%d, %d) in psychometric function", (int)mode, (int)wrt);
	return 0.0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double JWeibull(double f, double x, double a, double b, PsychSolveMode mode, ArgIdentifier wrt)
{
	double temp1, temp2;
	static char name[] = "Weibull";	
	
	if(mode == solve || mode == derivative) {
		if(!(mode == solve && wrt == X) && x < 0.0) Bug("Internal failure of constraints on Weibull arguments: received illegal value x = %lg", x);
		if(!(mode == solve && wrt == ALPHA) && a <= 0.0) Bug("Internal failure of constraints on Weibull arguments: received illegal value alpha = %lg", a);
		if(!(mode == solve && wrt == BETA) && b <= 0.0) Bug("Internal failure of constraints on Weibull arguments: received illegal value beta = %lg", b);
	}

	switch(mode) {
		case solve:
			switch(wrt) {
				case F: return f = 1.0 - exp(-pow(x / a, b));
				case X: return x = a * pow(-log_j(1.0 - f), 1.0 / b);
				case DFDX: return f = (1.0 - f) * pow(-log_j(1.0 - f), 1.0 - 1.0 / b) * b / a;
				case ALPHA: return a = x / pow(-log_j(1.0 - f), 1.0 / b);
				case BETA: return b = log_j(-log_j(1.0 - f)) / log_j(x / a);
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = a / pow(-log_j(1.0 - f), (f - 1.0) * log_j(1 - f) / (a * b));
				case BETA_FROM_SHIFT_AND_SLOPE: return b = a * b / ((f - 1.0) * log_j(1.0 - f));
			}
		case derivative:
			temp1 = pow(x / a, b);
			temp2 = exp(-temp1);
			switch(wrt) {
				case X: return f = b / a * pow(x / a, b - 1.0) * temp2;
				case ALPHA: return f = -b / a * temp1 * temp2;
				case BETA: return f =  log_j(x / a) * temp1 * temp2;
				case wrt_both(ALPHA, ALPHA):
					return f = b / (a * a) * (1 + b  * (1 - temp1)) * temp1 * temp2;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = (b * log_j(x / a) * (temp1 - 1.0) - 1.0) * temp1 * temp2 / a;
				case wrt_both(BETA, BETA):
					return f = (1.0 - temp1) * pow(log_j(x / a), 2.0) * temp1 * temp2;
			}
		case threshold_derivative:
			temp1 = -log_j(1.0 - f);
			switch(wrt) {
				case ALPHA: return f = pow(temp1, 1.0 / b);
				case BETA: return f =  -a * pow(temp1, 1.0 / b) * log_j(temp1) / (b * b);
				case wrt_both(ALPHA, ALPHA):
					return f = 0.0;
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = -pow(temp1, 1.0 / b) * log_j(temp1) / (b * b);
				case wrt_both(BETA, BETA):
					return f = a * pow(temp1, 1.0 / b) * log_j(temp1) * (2.0 * b + log_j(temp1)) / pow(b, 4.0);
			}
		case slope_derivative:
			temp1 = -log_j(1.0 - f);
			switch(wrt) {
				case ALPHA: return f = b * (f - 1.0) * temp1 / (a * a * pow(temp1, 1.0 / b));
				case BETA: return f = (1.0 - f) * pow(temp1, 1.0 - 1.0 / b) * (b + log_j(temp1)) / (a * b);
				case wrt_both(ALPHA, ALPHA):
					return f = 2.0 * b * (1.0 - f) * temp1 / (a * a * a * pow(temp1, 1.0 / b));
				case wrt_both(ALPHA, BETA): case wrt_both(BETA, ALPHA):
					return f = (f - 1.0) * pow(temp1, 1.0 - 1.0 / b) * (b + log_j(temp1)) / (a * a * b);
				case wrt_both(BETA, BETA):
					return f = (1.0 - f) * pow(temp1, 1.0 - 1.0 / b) * pow(log_j(temp1), 2.0) / (a * b * b * b);
			}
		case regression_transform:
			switch(wrt) {
				case X: return x = log_j(x);
				case F: return f = log_j(-log_j(1.0 - f));
				case ALPHA_FROM_SHIFT_AND_SLOPE: return a = exp(-a / b);
				case BETA_FROM_SHIFT_AND_SLOPE: return b = b;
			}
		
		case legal:
			switch(wrt) {
				case X: return (double)(x >= 0.0);
				case DFDX: return (double)(f >= 0.0);
				case ALPHA: return (double)(a > 0.0);
				case BETA: return (double)(b > 0.0);
			}
		case limits:
			switch(wrt) {
				case X: gLegal.min = 0.0; gLegal.max = INF; return 0.0;
				case DFDX: gLegal.min = 0.0; gLegal.max = INF; return 0.0;
				case ALPHA: gLegal.min = EPS; gLegal.max = INF; return 0.0;
				case BETA: gLegal.min = EPS; gLegal.max = INF; return 0.0;
			}
		case functionName: gFunctionName = name; return 0.0;
	}
	Bug("unknown mode (%d, %d) in psychometric function", (int)mode, (int)wrt);
	return 0.0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#endif /* __PSYCHOMETRIC_C__ */
