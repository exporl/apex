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
#ifndef __SUPPORTFUNCTIONS_H__
#define __SUPPORTFUNCTIONS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*	STRINGS	*/
double improved_strtod(char * start, char ** end);
int MatchString (	char * variableDescription, char * stringToMatch,
					boolean caseSensitive, boolean autoComplete,
					boolean generateErrorIfNoMatch, int nPossibilities, ...);

/*	ERRORS	*/
extern char *gExecName;
extern long gBugRef;
int Bug(char *fmt, ...);
int JError(char * errorString, ...);
void JWarning(char * errorString, ...);
void RemoveErrorTrap(void (*proc)(void));
void SetErrorTrap(void (*ErrProc)(void));

/*	MEMORY MANAGEMENT	*/
void *CopyVals(void *dest, void *src, size_t nElements, size_t elementSize);
void Destroy(void * p);
void DestroyAllBlocks(void);
void ProtectBlock(void * p);
boolean ReportBlocks(void);
void * ResizeBlock(void * p, unsigned int newNumberOfElements);
void * _New(unsigned int n, size_t size);
#define New(type, n)		((type *)_New((n), sizeof(type)))

/*	SIMPLEX SEARCH	*/
short SimplexSearch(unsigned short nParams, double * params, boolean *pfree, double * sizes, double (*function)(double * params));

/*	MATHEMATICAL FUNCTIONS	*/

double cg(double x);
double cg_inv(double x);
boolean detect_inf(double x);
boolean detect_nan(double x);
double erf_pf(double x);
double erf_inv(double x);
double log_j(double x);
double polynomial(double x, double *coeffs, unsigned short order);
double xlogy_j(double x, double y);

double CheckValue(double x, char *description, double min, double max,
					boolean rejectNonInteger, boolean rejectInf, boolean rejectNaN);
double CorrelationCoefficient(int nPoints, double * x, double * y);
void InitMathConstants(void);
void WeightedLinearRegression(int nPoints, double * x, double * y, double * weights, double * m, double *c);


/*	DEBUGGING & PORTABILITY	*/
extern boolean DEBUG;
boolean db(char * message);
#define report(a)		printf("%s = %lg\n", #a, (double)(a))
#define reportb(a) 		printf("%s = %s\n", #a, ((a)?"TRUE":"FALSE"))
#define reportc(a)		_ReportChar(#a, (a))
#define reportl(a, n)	_ReportListOfDoubles(#a, (a), n)
#define reportp(a)  	printf("%s = 0x%X\n", #a, (int)(a))
#define reports(a)		_ReportCString(#a, (a))
int TestFloatingPointBehaviour(void);
int _ReportChar(char *name, int a);
int _ReportCString(char *name, char *a);
int _ReportListOfDoubles(char *name, double *a, int n);

/*	RANDOM NUMBERS	*/
void InitRandomSeed(long seedVal);
long RandomRandomSeed(void);
double UniformRandomDouble(void);
long UniformRandomInt(long min, long max);

/*	SORTING	*/
int dcmp(const void *p1, const void *p2);
double median(unsigned short nPoints, double *list);
void SortDoubles(unsigned short numberOfLists, unsigned short lengthOfLists, double * first, ...);

/*	PREVENT HANGOVER OF GLOBAL VARIABLES	*/
void TabulaRasa(void);

/*	TIMER	*/
double ReadClock(void);
double ResetClock(void);

/*	PRINTING	*/
void FlushPrintBuffer(boolean eraseNewLineAfterwards);

#ifdef __cplusplus
}       // extern "C"
#endif

#endif /* __SUPPORTFUNCTIONS_H__ */

