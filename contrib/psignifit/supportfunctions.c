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
#ifndef __SUPPORTFUNCTIONS_C__
#define __SUPPORTFUNCTIONS_C__


#include "universalprefix.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mathheader.h"

#ifdef MATLAB_MEX_FILE
#include "matlabtools.h"
#endif

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/

/*	STRINGS	*/

/* The strtod implementation is:
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 */

static int maxExponent = 511;	/* Largest possible base 10 exponent.  Any
				 * exponent larger than this will already
				 * produce underflow or overflow, so there's
				 * no need to worry about additional digits.
				 */
static double powersOf10[] = {	/* Table giving binary powers of 10.  Entry */
    10.,			/* is 10^2^i.  Used to convert decimal */
    100.,			/* exponents into floating-point numbers. */
    1.0e4,
    1.0e8,
    1.0e16,
    1.0e32,
    1.0e64,
    1.0e128,
    1.0e256
};

/*
 *----------------------------------------------------------------------
 *
 * strtod --
 *
 *	This procedure converts a floating-point number from an ASCII
 *	decimal representation to internal double-precision format.
 *
 * Results:
 *	The return value is the double-precision floating-point
 *	representation of the characters in string.  If endPtr isn't
 *	NULL, then *endPtr is filled in with the address of the
 *	next character after the last one that was part of the
 *	floating-point number.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

double
clocale_strtod(string, endPtr)
    const char *string;		/* A decimal ASCII floating-point number,
				 * optionally preceded by white space.
				 * Must have form "-I.FE-X", where I is the
				 * integer part of the mantissa, F is the
				 * fractional part of the mantissa, and X
				 * is the exponent.  Either of the signs
				 * may be "+", "-", or omitted.  Either I
				 * or F may be omitted, or both.  The decimal
				 * point isn't necessary unless F is present.
				 * The "E" may actually be an "e".  E and X
				 * may both be omitted (but not just one).
				 */
    char **endPtr;		/* If non-NULL, store terminating character's
				 * address here. */
{
    int sign, expSign = 0;
    double fraction, dblExp, *d;
    register const char *p;
    register int c;
    int exp = 0;		/* Exponent read from "EX" field. */
    int fracExp = 0;		/* Exponent that derives from the fractional
				 * part.  Under normal circumstatnces, it is
				 * the negative of the number of digits in F.
				 * However, if I is very long, the last digits
				 * of I get dropped (otherwise a long I with a
				 * large negative exponent could cause an
				 * unnecessary overflow on I alone).  In this
				 * case, fracExp is incremented one for each
				 * dropped digit. */
    int mantSize;		/* Number of digits in mantissa. */
    int decPt;			/* Number of mantissa digits BEFORE decimal
				 * point. */
    const char *pExp;		/* Temporarily holds location of exponent
				 * in string. */

    /*
     * Strip off leading blanks and check for a sign.
     */

    p = string;
    while (isspace(*p)) {
	p += 1;
    }
    if (*p == '-') {
	sign = 1;
	p += 1;
    } else {
	if (*p == '+') {
	    p += 1;
	}
	sign = 0;
    }

    /*
     * Count the number of digits in the mantissa (including the decimal
     * point), and also locate the decimal point.
     */

    decPt = -1;
    for (mantSize = 0; ; mantSize += 1)
    {
	c = *p;
	if (!isdigit(c)) {
	    if ((c != '.') || (decPt >= 0)) {
		break;
	    }
	    decPt = mantSize;
	}
	p += 1;
    }

    /*
     * Now suck up the digits in the mantissa.  Use two integers to
     * collect 9 digits each (this is faster than using floating-point).
     * If the mantissa has more than 18 digits, ignore the extras, since
     * they can't affect the value anyway.
     */

    pExp  = p;
    p -= mantSize;
    if (decPt < 0) {
	decPt = mantSize;
    } else {
	mantSize -= 1;			/* One of the digits was the point. */
    }
    if (mantSize > 18) {
	fracExp = decPt - 18;
	mantSize = 18;
    } else {
	fracExp = decPt - mantSize;
    }
    if (mantSize == 0) {
	fraction = 0.0;
	p = string;
	goto done;
    } else {
	int frac1, frac2;
	frac1 = 0;
	for ( ; mantSize > 9; mantSize -= 1)
	{
	    c = *p;
	    p += 1;
	    if (c == '.') {
		c = *p;
		p += 1;
	    }
	    frac1 = 10*frac1 + (c - '0');
	}
	frac2 = 0;
	for (; mantSize > 0; mantSize -= 1)
	{
	    c = *p;
	    p += 1;
	    if (c == '.') {
		c = *p;
		p += 1;
	    }
	    frac2 = 10*frac2 + (c - '0');
	}
	fraction = (1.0e9 * frac1) + frac2;
    }

    /*
     * Skim off the exponent.
     */

    p = pExp;
    if ((*p == 'E') || (*p == 'e')) {
	p += 1;
	if (*p == '-') {
	    expSign = 1;
	    p += 1;
	} else {
	    if (*p == '+') {
		p += 1;
	    }
	    expSign = 0;
	}
	while (isdigit(*p)) {
	    exp = exp * 10 + (*p - '0');
	    p += 1;
	}
    }
    if (expSign) {
	exp = fracExp - exp;
    } else {
	exp = fracExp + exp;
    }

    /*
     * Generate a floating-point number that represents the exponent.
     * Do this by processing the exponent one bit at a time to combine
     * many powers of 2 of 10. Then combine the exponent with the
     * fraction.
     */

    if (exp < 0) {
	expSign = 1;
	exp = -exp;
    } else {
	expSign = 0;
    }
    if (exp > maxExponent) {
	exp = maxExponent;
	errno = ERANGE;
    }
    dblExp = 1.0;
    for (d = powersOf10; exp != 0; exp >>= 1, d += 1) {
	if (exp & 01) {
	    dblExp *= *d;
	}
    }
    if (expSign) {
	fraction /= dblExp;
    } else {
	fraction *= dblExp;
    }

done:
    if (endPtr != NULL) {
	*endPtr = (char *) p;
    }

    if (sign) {
	return -fraction;
    }
    return fraction;
}

double improved_strtod(char * start, char ** end)
{
	double x = NAN, x2;
	char temp[5], *endLocal, c;
	int i;

	errno = 0;
	while(isspace(*start)) start++;
	strncpy(temp, start, 4);
	for(i = 0; i < 4; i++) temp[i] = toupper(temp[i]);

	     if(strncmp(temp, "EPS", 3)==0) {x = EPS; endLocal = start + 3;}
	else if(strncmp(temp, "INF", 3)==0) {x = INF; endLocal = start + 3;}
	else if(strncmp(temp, "NAN", 3)==0) {x = NAN; endLocal = start + 3;}
	else if(strncmp(temp, "-EPS", 4)==0) {x = -EPS; endLocal = start + 4;}
	else if(strncmp(temp, "-INF", 4)==0) {x = -INF; endLocal = start + 4;}
	else x = clocale_strtod(start, &endLocal);

	c = *endLocal;
	if(c == ',' || c == ';') endLocal++;
	else if (c == '/') {
		x2 = improved_strtod(++endLocal, &endLocal);
		if(isnan(x) || isnan(x2)) x = NAN;
		else if(isinf(x2)) x = (isinf(x) ? NAN : 0.0);
		else if(isinf(x)) x = ((x2 < 0.0) ? -x : (x2 > 0.0) ? x : NAN);
		else if(x2 == 0) x = ((x < 0.0) ? -INF : (x > 0.0) ? INF : NAN);
		else x /= x2;
	}
    else if(c != 0 && !isspace(c)) errno = ERANGE;

    while(isspace(*endLocal)) endLocal++;
    if(*endLocal == 0) endLocal++; // grrr

    if(end != NULL) *end = endLocal;

	return x;
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////*/
int MatchString (	char * variableDescription, char * stringToMatch,
					boolean caseSensitive, boolean autoComplete,
					boolean generateErrorIfNoMatch, int nPossibilities, ...)
{
	unsigned short i, j;
	va_list ap;
	size_t totalLen = 0;
	char nullString[1] = "", *possibilities, **match;

	match = New(char *, nPossibilities);

	if(stringToMatch==NULL) stringToMatch = nullString;
	va_start(ap, nPossibilities);
	for(i = 0; i < nPossibilities; i++) {
		match[i] = va_arg(ap, char*);
		if(match[i]==NULL) match[i] = nullString;
		totalLen += strlen(match[i]);
		for(j = 0; match[i][j] && stringToMatch[j]; j++) {
			if(caseSensitive && match[i][j] != stringToMatch[j]) break;
			if(!caseSensitive && tolower(match[i][j]) != tolower(stringToMatch[j])) break;
		}
		if(stringToMatch[j] == 0 && (autoComplete || match[i][j] == 0)) break;
	}
	va_end(ap);
	i = ((i==nPossibilities) ? 0 : i+1);
	if(i==0 && generateErrorIfNoMatch) {
		possibilities = New(char, totalLen + nPossibilities * 2 + 1);
		totalLen = 0;
		for(j = 0; j < nPossibilities; j++) {
			totalLen += sprintf(possibilities+totalLen, "\n\t%s", match[j]);
		}
		JError("Unrecognized %s \"%s\". Acceptable values are:%s", variableDescription, stringToMatch, possibilities);
		Destroy(possibilities);
	}
	Destroy(match);
	return i;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	ERRORS	*/

char *gExecName = NULL;
void (*JERROR_TRAP_PROC)(void) = NULL;
long gBugRef = 0;
int _JError(char * errorString, boolean internal);
void _JWarning(char * warnString);

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int Bug(char *fmt, ...)
{
	va_list ap;
	char temp[255];

	*temp = 0;
	if(fmt!=NULL && strlen(fmt)>0)
		{va_start(ap, fmt); vsprintf(temp, fmt, ap); va_end(ap);}

	return _JError(temp, TRUE);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int JError(char * fmt, ...)
{
	va_list ap;
	char temp[255];

	*temp = 0;
	if(fmt!=NULL && strlen(fmt)>0)
		{va_start(ap, fmt); vsprintf(temp, fmt, ap); va_end(ap);}

	return _JError(temp, FALSE);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int _JError(char * errorString, boolean internal)
{
	void (*proc)(void);
	char unspecifiedString[] = "<<unspecified error>>";

	if(errorString == NULL || strlen(errorString) == 0) errorString = unspecifiedString;

	if(internal) {
		fprintf(stderr, "\n");
		fprintf(stderr, "**************************************************************\n");
		fprintf(stderr, "The following error is an internal bug in program.\n");
		fprintf(stderr, "Please report it to %s giving details of\n", kAUTHOR_CONTACT);
		fprintf(stderr, "the error message and the input conditions that gave rise to it.\n");
        if(gBugRef) fprintf(stderr, "Quote the number %ld when reporting this error.\n", gBugRef);
		fprintf(stderr, "**************************************************************\n");
	}

	if(JERROR_TRAP_PROC != NULL) {
		proc = JERROR_TRAP_PROC;
		JERROR_TRAP_PROC = NULL;
		(*proc)();
	}

	DestroyAllBlocks();

#ifdef MATLAB_MEX_FILE
	mexErrMsgTxt(errorString);
#else
	if(gExecName) fprintf(stderr, "\n%s: ", gExecName);
	else fprintf(stderr, "\n");
	fprintf(stderr, "%s\n", errorString);
    //exit(EXIT_FAILURE);       [Tom] For APEX
#endif
	return -1;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void JWarning(char * fmt, ...)
{
	va_list ap;
	char temp[255];

	*temp = 0;
	if(fmt!=NULL && strlen(fmt)>0)
		{va_start(ap, fmt); vsprintf(temp, fmt, ap); va_end(ap);}

	_JWarning(temp);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void _JWarning(char * warnString)
{
	char unspecifiedString[] = "<<unspecified warning>>";

	if(warnString == NULL || strlen(warnString) == 0) warnString = unspecifiedString;

#ifdef MATLAB_MEX_FILE
	mexWarnMsgTxt(warnString);
#else
	if(strlen(warnString)>0) {
		if(gExecName) fprintf(stderr, "\n%s ", gExecName);
		else fprintf(stderr, "\n");
		fprintf(stderr, "\nWARNING: %s\n\n", warnString);
	}
#endif
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void RemoveErrorTrap(void (*proc)(void)) {if(proc == JERROR_TRAP_PROC) JERROR_TRAP_PROC = NULL;}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void SetErrorTrap(void (*ErrProc)(void))
{
	if(ErrProc != NULL && JERROR_TRAP_PROC != NULL)
		Bug("SetErrorTrap(): trap already set.\nMust be deliberately wiped with SetErrorTrap(NULL) before being replaced.");
	JERROR_TRAP_PROC = ErrProc;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	MEMORY MANAGEMENT	*/

void **gBlock;
unsigned int *gNumberOfElements;
size_t *gElementSize;
unsigned int gMaxBlocks = 256;
boolean gBlocksInited = FALSE;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void *CopyVals(void *dest, void *src, size_t nElements, size_t elementSize)
{
	if(dest == NULL) dest = _New(nElements, elementSize);
	if(dest != src && src != NULL) memcpy(dest, src, nElements*elementSize);
	return dest;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void Destroy(void * p)
{
    unsigned int i;
	if(!gBlocksInited || p==NULL) return;
	for(i = 0; i < gMaxBlocks; i++) if(gBlock[i] == p) break;
	if(i == gMaxBlocks) {JError("attempt to free unlisted block 0x%08x", (unsigned long)p); return;}

	gBlock[i] = NULL;

	free(p);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void DestroyAllBlocks(void)
{
    unsigned int i;

	if(!gBlocksInited) return;
	for(i = 0; i < gMaxBlocks; i++) {
		if(gBlock[i] != NULL) {
			free(gBlock[i]);
			gBlock[i] = NULL;
		}
	}
	gBlocksInited = FALSE;
	free(gBlock);
	free(gNumberOfElements);
	free(gElementSize);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void ProtectBlock(void * p)
{
    unsigned int i;

	for(i = 0; i < gMaxBlocks; i++)
		if(gBlock[i] == p) break;
	if(i == gMaxBlocks || !gBlocksInited || p == NULL)
		JError("Cannot protect block %X: may be already protected, or not created with New()", p);
	gBlock[i] = NULL;
#ifdef MATLAB_MEX_FILE
	mexMakeMemoryPersistent(p);
#endif
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean ReportBlocks(void)
{
    unsigned int i, j;
	boolean result = FALSE;
	if(!gBlocksInited) return FALSE;
	for(i = 0; i < gMaxBlocks; i++) {
		if(gBlock[i]!=NULL) result = TRUE;
        if(gBlock[i]!=NULL) fprintf(stderr, "Memory leak @ 0x%p:  %6u x %zu bytes\n", gBlock[i], gNumberOfElements[i], gElementSize[i]);
		if(gBlock[i]!=NULL && gElementSize[i] == 8) {for(j = 0; j < gNumberOfElements[i] && j < 10; j++) fprintf(stderr, "%lg, ", ((double *)(gBlock[i]))[j]); fprintf(stderr, "%c%c%s\n", 8, 8, ((j < gNumberOfElements[i])?"...":""));}
		if(gBlock[i]!=NULL && gElementSize[i] == 1) {for(j = 0; j < gNumberOfElements[i]; j++) fprintf(stderr, "%c", ((char *)(gBlock[i]))[j]); fprintf(stderr, "\n");}
	}
	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void * ResizeBlock(void * p, unsigned int newNumberOfElements)
{
    unsigned int i;

	for(i = 0; i < gMaxBlocks; i++)
		if(gBlock[i] == p) break;
	if(i == gMaxBlocks || !gBlocksInited || p == NULL)
		JError("Illegal operation: attempt to resize an unallocated block");

	if((p = realloc(p, newNumberOfElements * gElementSize[i])) == NULL)
		JError("Failed to resize memory block from %u to %u elements (element size %u)", gNumberOfElements[i], newNumberOfElements, gElementSize[i]);

	gBlock[i] = p;
	gNumberOfElements[i] = newNumberOfElements;

	return p;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void * _New(unsigned int n, size_t size)
{
	void * p;

    unsigned int i;
	if(!gBlocksInited) {
		gBlock = (void **)calloc(gMaxBlocks, sizeof(void *));
		gNumberOfElements = (unsigned int *)calloc(gMaxBlocks, sizeof(unsigned int));
		gElementSize = (size_t *)calloc(gMaxBlocks, sizeof(size_t));
		gBlocksInited = TRUE;
		for(i = 0; i < gMaxBlocks; i++) gBlock[i] = NULL;
	}
	for(i = 0; i < gMaxBlocks; i++)
		if(gBlock[i] == NULL) break;
	if(i == gMaxBlocks) JError("run out of table space to allocate new pointers");

	if((p = calloc(n, size)) == NULL)
		JError("Memory error: failed to allocate block of %u x %u bytes", n, size);

	gBlock[i] = p;
	gNumberOfElements[i] = n;
	gElementSize[i] = size;

	return p;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	SIMPLEX SEARCH	*/

#define kMaxDimensions	10
#define kMaxIterations	1600
#define kUsualDelta		0.05
#define kZeroTermDelta	0.00025
#define kTolerance		1.0e-6

double *SIZES;
double POINTS[kMaxDimensions+1][kMaxDimensions];
double SCORES[kMaxDimensions+1];
double TOTALS[kMaxDimensions+1];
boolean *FREE;
unsigned short NPARAMS, NPOINTS;
short ITERATIONS;
double (*FUNCTION)(double * params);

#define DEBUG_SIMPLEX 0

double MovePoint(unsigned short p, double factor);
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#if DEBUG_SIMPLEX
#include "matrices.h"
void ReportSimplex(short nParams, double *p, double score);
void ReportSimplex(short nParams, double *p, double score)
{
	static matrix m = NULL;

	if(p == NULL && m != NULL) {
		JWarning("simplex path is being recorded - see simplex_report");
		m_setsize(m, m2D, m_getpos(m, 1), m_getsize(m, 2));
		m_setoutput(m, "simplex_report", "w", "");
		m_free(m);
		m = NULL;
		return;
	}
	if(m == NULL) m = m_new(mNewData, m2D, mCustomPacking, 1+nParams, kMaxIterations, 1, 1+nParams);
	if(m == NULL || m->vals == NULL) Bug("failed to allocate simplex report matrix");
	if(m_getsize(m, 2) != nParams + 1) Bug("simplex report matrix has wrong size");
	m_val(m) = score;
	CopyVals(m_addr(m, 2, 1), p, nParams, sizeof(double));
	if(!m_step(m, 1, 1)) Bug("simplex report matrix ran out of room");
}
#else
#define ReportSimplex(a,b,c) ((void)0)
#endif
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double MovePoint(unsigned short p, double factor)
{
	unsigned short i;
	double temp1, temp2, newscore, newpos[kMaxDimensions];

	temp2 = (temp1 = (1.0 - factor) / (double)(NPOINTS-1)) - factor;
	for(i = 0; i < NPARAMS; i++) newpos[i] = (FREE[i] ? TOTALS[i] * temp1 - POINTS[p][i] * temp2 : POINTS[p][i]);
	if((newscore = (*FUNCTION)(newpos)) < SCORES[p]) {
		for(i = 0; i < NPARAMS; i++) {
		    if(FREE[i]) {
		    	TOTALS[i] -= POINTS[p][i] - newpos[i];
		    	POINTS[p][i] = newpos[i];
		    }
		}
		SCORES[p] = newscore;
	}

	ITERATIONS++;
	return newscore;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
short SimplexSearch(unsigned short nParams, double * params, boolean *pfree, double * sizes, double (*function)(double * params))
{
	unsigned short i, p, nFreeParams, highest, nextHighest, lowest;
	double worstCase;

	FUNCTION = function;
	NPARAMS = nParams;
	FREE = pfree;
	ITERATIONS = 0;

	if(NPARAMS < 1 || NPARAMS > kMaxDimensions)
		{Bug("SimplexSearch() cannot deal with %hu dimensions: must be from 1 to %hu.", NPARAMS, kMaxDimensions); return -1;}
	for(NPOINTS = 1, i = 0; i < NPARAMS; i++) if(FREE[i]) NPOINTS++;
	if(NPOINTS == 1) return 0; /*	no free parameters -- return	*/

	for(p = 0; p < NPOINTS; p++) {
/*		Set up points in the initial simplex    */
		for(nFreeParams = 0, i = 0; i < NPARAMS; i++) {
			POINTS[p][i] = params[i];
			if(FREE[i] && p == ++nFreeParams)
				POINTS[p][i] += (sizes ? sizes[i] : (params[i] == 0.0) ? kZeroTermDelta : kUsualDelta * params[i]);
		}
/*		Fill in function() value for the new point	*/
		SCORES[p] = (*FUNCTION)(POINTS[p]);
	}

/*	Calculate the sum, across points, in each dimension    */
	for(i = 0;i < NPARAMS; i++)
		for(TOTALS[i] = 0.0, p = 0; p < NPOINTS; p++)
			if(FREE[i]) TOTALS[i] += POINTS[p][i];

	while(TRUE) {
/*		Determine which points give the highest, next-highest and lowest function() values    */
		lowest = 0;
		highest = (SCORES[0] > SCORES[1]) ? (nextHighest = 1, 0) : (nextHighest = 0, 1);
		for(p = 0; p < NPOINTS; p++) {
			if (SCORES[p] <= SCORES[lowest]) lowest = p;
			if (SCORES[p] > SCORES[highest]) nextHighest = highest, highest = p;
			else if (SCORES[p] > SCORES[nextHighest] && p != highest) nextHighest = p;
		}
		ReportSimplex(NPARAMS, POINTS[lowest], SCORES[lowest]);
/*		Return if done    */
		if (kTolerance >= 2.0 * fabs(SCORES[highest] - SCORES[lowest])/(fabs(SCORES[highest]) + fabs(SCORES[lowest]) )) {
			for(i = 0; i < NPARAMS; i++) params[i] = POINTS[lowest][i];
			ReportSimplex(0, NULL, 0.0);
			return ITERATIONS;
		}
/*		Crash out with error if kMaxIterations  is exceeded    */
		if(ITERATIONS >= kMaxIterations)  {
			ReportSimplex(0, NULL, 0.0);
			return -ITERATIONS;
		}
/*		Jiggle the points about    */
		if(MovePoint(highest, -1.0) <= SCORES[lowest]) MovePoint(highest, 2.0);
		else if((worstCase = SCORES[highest]) >= SCORES[nextHighest] && MovePoint(highest, 0.5) >= worstCase) {
			for(p = 0; p < NPOINTS; p++) {
				if(p == lowest) continue;
				for(i = 0; i < NPARAMS; i++)
					if(FREE[i]) {
					    TOTALS[i] -= POINTS[p][i] - (0.5 * (POINTS[p][i] + POINTS[lowest][i]));
					    POINTS[p][i] = 0.5 * (POINTS[p][i] + POINTS[lowest][i]);
					}
				SCORES[p] = (*FUNCTION)(POINTS[p]);
				ITERATIONS++;
			}
		}
	}
	return -1;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	MATHEMATICAL FUNCTIONS	*/

double EPS, INF, NAN;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double cg(double x) {return 0.5 + 0.5 * erf_pf(x / sqrt(2.0));}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double cg_inv(double x) {return sqrt(2.0) * erf_inv(2.0 * x - 1.0);}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean detect_inf(double x)
{
	double y;

	if(x == 0.0) return FALSE;
	if(isnan(x)) return FALSE;
	y = x / 1000.0;

	return (y == x);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean detect_nan(double x)
{
	if(x > 0.0) return FALSE;
	if(x < 0.0) return FALSE;
	if(x == 0.0) return FALSE;
	return TRUE;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double erf_pf(double x)
{
	double y, coeffs[6] = {1.061405429e+00, -1.453152026e+00, 1.421413741e+00, -2.844967366e-01, 2.548295918e-01, 0.0};

	if(x == 0.0) return 0.0;
	y = exp(-x * x) * polynomial(1.0 / (1.0 + 3.275911166e-01 * fabs(x)), coeffs, 5);
	return ((x < 0.0) ? y - 1.0 : 1.0 - y);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double erf_inv(double y)
{
	double ya, ys, x, z;
	double nc1[4] = {-0.140543331, 0.914624893, -1.64534962, 0.886226899};
	double dc1[5] = {0.012229801, -0.329097515, 1.44271046, -2.11837773, 1.0};
	double nc2[4] = {1.64134531, 3.4295678, -1.62490649, -1.97084045};
	double dc2[3] = {1.6370678, 3.5438892, 1.0};

	if(y == 0.0) return 0.0;
	ya = fabs(y);
	ys = ((y < 0.0) ? -1.0 : (y > 0.0) ? 1.0 : 0.0);
	if(ya == 1.0) return ys * INF;
	if(ya > 1.0) return NAN;

	if(ya <= 0.7) {
		z = y * y;
		x = y * polynomial(z, nc1, 3) / polynomial(z, dc1, 4);
	}
	else {
		z = sqrt(-log_j((1.0 - ya) / 2.0));
		x = ys * polynomial(z, nc2, 3) / polynomial(z, dc2, 2);
	}
    x -= (erf_pf(x) - y) / (exp(-x * x) * 2.0 / sqrt(pi));
    x -= (erf_pf(x) - y) / (exp(-x * x) * 2.0 / sqrt(pi));

	return x;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double log_j(double x)
{
  return ((x < 0.0) ? NAN : (x == 0.0) ? -INF : log(x));
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double polynomial(double x, double *coeffs, unsigned short order)
{
	unsigned short i;
	double result;
	for(result = coeffs[0], i = 1; i <= order; i++) result = result * x + coeffs[i];
	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double xlogy_j(double x, double y)
{
  return ((y < 0.0) ? NAN : (x == 0.0) ? 0.0 : (y == 0.0) ? -INF : x * log(y));
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double CheckValue(double x, char *description, double min, double max,
					boolean rejectNonInteger, boolean rejectInf, boolean rejectNaN)
{
	if(rejectInf && isinf(x)) JError("%s must be finite", description);
	if(rejectNaN && isnan(x)) JError("%s cannot be NaN", description);
	if(rejectNonInteger && x != floor(x)) JError("%s must be a whole number", description);
	if(x < min) JError("%s cannot be less than %lg", description, min);
	if(x > max) JError("%s cannot be greater than %lg", description, max);
	return x;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double CorrelationCoefficient(int nPoints, double * x, double * y)
{
	int i;
	double xBar = 0.0, yBar = 0.0;
	boolean xDiff = FALSE, yDiff = FALSE;
	double numerator, denominator1, denominator2, xTerm, yTerm;

	if(nPoints == 0) return NAN;
	for(i = 0; i < nPoints; i++) {
		if(!xDiff && x[i] != x[0]) xDiff = TRUE;
		if(!yDiff && y[i] != y[0]) yDiff = TRUE;
		xBar += x[i];
		yBar += y[i];
	}
	if(!xDiff && !yDiff) return NAN;
	if(!xDiff || !yDiff) return 0.0;

	xBar /= (double)nPoints;
	yBar /= (double)nPoints;

	numerator = denominator1 = denominator2 = 0.0;
	for(i = 0; i < nPoints; i++) {
		xTerm = x[i] - xBar;
		yTerm = y[i] - yBar;
		numerator += xTerm * yTerm;
		denominator1 += xTerm * xTerm;
		denominator2 += yTerm * yTerm;
	}
	return numerator / sqrt(denominator1 * denominator2);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

void InitMathConstants(void)
{
#if defined MATLAB_MEX_FILE
	INF = mxGetInf();
	NAN = mxGetNaN();
	EPS = mxGetEps();
#else
#if defined DBL_QNAN
	NAN = DBL_QNAN;
#else
    #ifdef _MSC_VER
        #define INFINITY (DBL_MAX+DBL_MAX)
    //    #define NAN (INFINITY-INFINITY)

//        #define NAN (*(const double *) __nan)
//        #define NAN ((const double) _Nan)
        unsigned long t[2] = {0xffffffff, 0x7fffffff};
        NAN = (*(const double *) t);
    #else
        NAN = 0.0 / 0.0;
    #endif
#endif
#if defined DBL_INFINITY
	INF = DBL_INFINITY;
#elif defined INFINITY
	INF = INFINITY;
#else
	INF = 1.0 / 0.0;
#endif
	EPS = pow(2.0, -52.0);
#endif /* MATLAB_MEX_FILE */
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void WeightedLinearRegression(int nPoints, double * x, double * y, double * weights, double * m, double *c)
{
	double xWeightedMean, yWeightedMean, totalWeight;
	double w, xR, mTop, mBottom;
	int i;

	xWeightedMean = yWeightedMean = totalWeight = 0.0;
	for(i = 0; i < nPoints; i++) {
		w = (weights == NULL) ? 1.0 : weights[i];
		xWeightedMean += x[i] * w;
		yWeightedMean += y[i] * w;
		totalWeight += w;
	}
	xWeightedMean /= totalWeight;
	yWeightedMean /= totalWeight;

	mTop = mBottom = 0.0;
	for(i = 0; i < nPoints; i++) {
		w = (weights == NULL) ? 1.0 : weights[i];
		xR = x[i] - xWeightedMean;
		mTop += xR * y[i] * w;
		mBottom += xR * xR * w;
	}
	if(m!=NULL) *m = mTop/mBottom;
	if(c!=NULL) *c = yWeightedMean - mTop/mBottom * xWeightedMean;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	DEBUGGING & PORTABILITY	*/

boolean DEBUG = FALSE;
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean db(char * message)
{
#ifdef MATLAB_MEX_FILE
	mexEvalf("input('%s... ', 's');", ((message != NULL && strlen(message) > 0) ? message : "press return"));
#else
	// double a;
	fprintf(stderr, "%s... ", ((message != NULL && strlen(message) > 0) ? message : "press return"));
	// scanf("%lg", &a);
#endif
	return TRUE;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int TestFloatingPointBehaviour(void)
{
	double ONE = 1.0, ZERO = 0.0;
	double thisTest;
	boolean allTests = TRUE;

	fprintf(stderr, "Testing IEEE floating point behaviour of your compiled program.\n\n");
	fprintf(stderr, "If the program crashes with the message \"floating exception\" at any\n");
	fprintf(stderr, "point, it has failed the test. Also, if any further tests are reported\n");
	fprintf(stderr, "\"FAILED\" then your program may behave unreliably with regard to some\n");
	fprintf(stderr, "floating-point calculations. In either case you should re-compile using\n");
	fprintf(stderr, "the -ieee switch (if available), or using a floating-point library that\n");
	fprintf(stderr, "supports IEEE standard behaviour of INF and NaN\n\n");

	fprintf(stderr, "Attempting 1.0/0.0: ");
	fprintf(stderr, "%lg\n", ONE / ZERO);
	fprintf(stderr, "Attempting 0.0/0.0: ");
	fprintf(stderr, "%lg\n", ZERO / ZERO);
	fprintf(stderr, "\nGood, no crashes.\n");

#define tryTest(a, expected)	{	\
/*									fprintf(stderr, "Attempting %s: ", #a); \
*/									thisTest = (a); \
									allTests &= (thisTest == expected); \
									if(thisTest != expected) fprintf(stderr, "TEST FAILED: your program thinks that %s is %s\n", #a, thisTest ? "TRUE" : "FALSE"); \
/*									fprintf(stderr, "%s%s\n", thisTest ? "TRUE" : "FALSE", (thisTest == expected) ? "" : "*** FAILED"); \
*/								}
	InitMathConstants();
	tryTest(isinf(ONE / ZERO), TRUE);
	tryTest(isinf(INF), TRUE);
	tryTest(isinf(-INF), TRUE);
	tryTest(isinf(NAN), FALSE);
	tryTest(isinf(-ONE), FALSE);
	tryTest(isinf(ZERO), FALSE);
	tryTest(isinf(ONE), FALSE);

	tryTest((INF * EPS == INF), TRUE);
	tryTest((INF * -INF == -INF), TRUE);
	tryTest(isnan(INF * ZERO), TRUE);

	tryTest(isnan(ZERO / ZERO), TRUE);
	tryTest(isnan(INF), FALSE);
	tryTest(isnan(-INF), FALSE);
	tryTest(isnan(NAN), TRUE);
	tryTest(isnan(-ONE), FALSE);
	tryTest(isnan(ZERO), FALSE);
	tryTest(isnan(ONE), FALSE);

	tryTest( INF > ZERO, TRUE);
	tryTest(-INF < ZERO, TRUE);
	tryTest(NAN <= ZERO, FALSE);
	tryTest(NAN >= ZERO, FALSE);

	if(allTests) fprintf(stderr, "All further floating-point tests were successful.\n");
	return allTests;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int _ReportChar(char *name, int a)
{
	if(a == EOF) return fprintf(stderr, "%s = EOF\n", name);
	else if(a < 32 || a > 126) return fprintf(stderr, "%s = %%%02X\n", name, a);
	else return fprintf(stderr, "%s = '%c'\n", name, a);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int _ReportCString(char *name, char *a)
{
	if(a==NULL) return fprintf(stderr, "%s = NULL\n", name);
	else return fprintf(stderr, "%s = \"%s\"\n", name, a);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int _ReportListOfDoubles(char *name, double *a, int n)
{
	int i, c = 0;
	if(a==NULL) return fprintf(stderr, "%s = NULL\n", name);
	c += fprintf(stderr, "%s = {", name);
	for(i = 0; i < n; i++) c += fprintf(stderr, "%lg%s", a[i], (i == n - 1) ? "" : ", ");
	fprintf(stderr, "}\n");
	return c;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	RANDOM NUMBERS	*/

#define BDTABLE_LENGTH	32
long BDTABLE[BDTABLE_LENGTH], S1, S2 = 123456789, S3 = 0;
boolean SEEDED_BY_TIME = FALSE; /* only needs to be initialized when seeds have been zapped => no need to include in TabulaRasa() */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void InitRandomSeed(long seed)
{
	int i; long temp;

	S1 = S2 = ((seed > 0) ? seed : (seed < 0) ? -seed : 1);
	for(i = BDTABLE_LENGTH + 7; i >= 0; i--) {
		temp = S1 / 53668;
		S1 = (S1 - 53668 * temp) * 40014 - temp * 12211;
		S1 += ((S1 < 0) ? 2147483563 : 0);
		if(i < BDTABLE_LENGTH) BDTABLE[i] = S1;
	}
	S3 = S1;
	UniformRandomDouble();
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
long RandomRandomSeed(void)
{
	long seed = labs(time(NULL));
	if(SEEDED_BY_TIME) return -1;
	SEEDED_BY_TIME = TRUE;
	InitRandomSeed(seed);
	return seed;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double UniformRandomDouble(void)
{
	long temp;
	int ind;
	double result;

	temp = S1 / 53668;
	S1 = (S1 - 53668 * temp) * 40014 - temp * 12211;
	S1 += ((S1 < 0) ? 2147483563 : 0);
	temp = S2 / 52774;
	S2 = (S2 - 52774 * temp) * 40692 - temp * 3791;
	S2 += ((S2 < 0) ? 2147483399 : 0);

	S3 = BDTABLE[ind = S3 / (2147483562 / BDTABLE_LENGTH + 1)] - S2;
	S3 += ((S3 < 1) ? 2147483562 : 0);
	BDTABLE[ind] = S1;

	return ((result = (double)S3 / 2147483563.0) < 1.0 - 1.2E-7) ? result : 1.0 - 1.2E-7;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
long UniformRandomInt(long min, long max)
	{return min + (long)(0.5 + (double)(max - min) * UniformRandomDouble());}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	SORTING */

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int dcmp(const void *p1, const void *p2) /* for use with ANSI qsort() */
{
	double a, b;

	a = *(double *)p1;
	b = *(double *)p2;
	if(isnan(a)) return (isnan(b) ? 0 : 1); /* NaNs are sorted to the end */
	else if(isnan(b)) return -1;
	else return ((a < b) ? -1 : (a > b) ? 1 : 0);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double median(unsigned short nPoints, double *list) /* for one-offs only */
{
	double *local, midIndex, returnVal;
	if(nPoints == 0 || list == NULL) return NAN;
	local = CopyVals(NULL, list, nPoints, sizeof(double));
	SortDoubles(1, nPoints, local);
	midIndex = 0.5 * (double)(nPoints - 1);
	returnVal = (local[(int)floor(midIndex)] + local[(int)ceil(midIndex)]) / 2.0;
	Destroy(local);
	return returnVal;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void SortDoubles(unsigned short numberOfLists, unsigned short lengthOfLists, double * first, ...)
{
	va_list ap;
	double ** otherLists = NULL;
	double temp;
	int i, src, dest;

#define swapem(a, b)	(temp = (a), (a) = (b), (b) = temp)

	if(numberOfLists < 1 || lengthOfLists < 2 || first == NULL) return;
	if(numberOfLists > 1) {
		otherLists = New(double *, numberOfLists - 1);
		va_start(ap, first);
		for(i = 0; i < numberOfLists - 1; i++) otherLists[i] = va_arg(ap, double *);
		va_end(ap);
	}

	for(src = 1; src < lengthOfLists;) {
		dest = src;
		while(dest > 0 && first[dest-1] > first[src]) dest--;
		if(dest == src) src++;
		else {
			swapem(first[src], first[dest]);
			for(i = 0; i < numberOfLists - 1; i++)
				swapem(otherLists[i][src], otherLists[i][dest]);
		}
	}

	if(otherLists) Destroy(otherLists);

#undef swapem
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	PREVENT HANGOVER OF GLOBAL VARIABLES IN CASES WHERE THE ZONE ISN'T CLEARED	*/

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void TabulaRasa(void)
{
	gBugRef = 0;
	gBlocksInited = FALSE;
	SetErrorTrap(NULL);
	InitMathConstants();
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	TIMER	*/

clock_t TIC_TOC = 0;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double ReadClock(void) {return (double)(clock() - TIC_TOC)/ (double)CLOCKS_PER_SEC;}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double ResetClock(void) {
	double val = ReadClock();
	TIC_TOC = clock();
	return val;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

/*	PRINTING	*/

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void FlushPrintBuffer(boolean eraseNewLineAfterwards)
{
	int i;
#ifdef MATLAB_MEX_FILE
	i = fprintf(stderr, "\n"); mexEvalString("disp('')");
#else
	i = fprintf(stderr, "\n");
#endif
	if(eraseNewLineAfterwards) while(i--) fprintf(stderr, "%c", 8);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#endif /* __SUPPORTFUNCTIONS_C__ */
