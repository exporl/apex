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
#ifndef __PRIORS_H__
#define __PRIORS_H__

typedef enum {evaluatePrior, getWorkingLimit, nArgsForPrior, verifyPriorArgs, namePrior, describePrior} PriorMode;
typedef double(BayesianPrior)(PriorMode, double *, double, unsigned short);
typedef BayesianPrior * BayesianPriorPtr;
#define prior(p, c, val)		(((c) == NULL || (c)->prior == NULL) ? (p) : (p) * (*(c)->prior)(evaluatePrior, (c)->args, val, 0))
#define GetWorkingMin(c)		(((c) == NULL || (c)->prior == NULL) ? -INF : (*(c)->prior)(getWorkingLimit, (c)->args, -1.0, 0))
#define GetWorkingMax(c)		(((c) == NULL || (c)->prior == NULL) ?  INF : (*(c)->prior)(getWorkingLimit, (c)->args,  1.0, 0))
#define ExpectedNumberOfPriorArgs(c)	((unsigned short)(0.5 + (*(c)->prior)(nArgsForPrior, NULL, 0.0, 0)))
#define VerifyPriorArgs(c)		((*(c)->prior)(verifyPriorArgs, (c)->args, 0.0, 0) != 0.0)
#define PriorName(c)			((void *)(long)(*(c)->prior)(namePrior, (c)->args, 0.0, 0), gPriorString)
#define PriorDescription(c)		((void *)(long)(*(c)->prior)(describePrior, (c)->args, 0.0, 0), gPriorString)

#define kMaxPriorArgs	5
typedef struct {
	BayesianPriorPtr prior;
	double args[kMaxPriorArgs];
} Constraint;
typedef Constraint * ConstraintPtr;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

extern char gPriorString[];

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

double DiffLogPrior(double arg, double diff_arg, ConstraintPtr W);
double Diff2LogPrior(double arg, double da_du, double da_dv, double d2a_dudv, ConstraintPtr W);
BayesianPrior BetaPrior, CosinePrior, FlatPrior, GaussianPrior;
int ReportPrior(char *s, ConstraintPtr c);
void SetPrior(ConstraintPtr constraint, BayesianPriorPtr prior, unsigned short nArgs, double *args);

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#endif	/* __PRIORS_H__	*/
