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
#ifndef __PREFS_C__
#define __PREFS_C__

#include "universalprefix.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "psignifit.h"
#include "adaptiveinterface.h"

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

unsigned short gMeshResolution, gMeshIterations;
double gEstimateGamma, gEstimateLambda;

boolean gLogSlopes, gCutPsi, gLambdaEqualsGamma;
DataFormat gDataFormat;
/* don't even think about initializing global variables here: values won't be re-initialized between calls to MEX file */

boolean gDoBootstrapT;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void AssignOutput(matrix m, BatchPtr batch, char *ident, char *extn, char *writeFormat)
{
	char temp[24];
	char * str, sw;
	int len;
	int mexEvalf(char * fmt, ...);
	boolean addTitle = FALSE;

	if(m == NULL) return;
	strcpy(m->writeMode, "w");
	strcpy(m->writeFormat, writeFormat);
	if(m->output) { Destroy(m->output); m->output = NULL;}

	sprintf(temp, "WRITE_%s", ident);
	if(extn != NULL) sprintf(temp + strlen(temp), "%s", extn);
	for(str = temp; *str; str++) *str = toupper(*str);

	if(IdentifierAppearsInBatch(batch, temp)) {
		addTitle = FALSE;
		m->warnIfEmpty = TRUE;
		str = FindVariableInBatch(batch, temp, &len, uniqueOccurrence);
		while(len > 3 && str[0] == '-' && isspace(str[2]) &&
			( (sw = tolower(str[1])) == 'a' || sw == 't' || sw == 'n')) {
			switch(sw) {
				case 'a': strcpy(m->writeMode, "a"); break;
				case 't': addTitle = TRUE; break;
				case 'n': addTitle = FALSE; break;
			}
			for(str += 3, len -= 3; isspace(*str); str++) len--;
		}
		m->output = ReadString(str, len, NULL, NULL);
	}
	else if(extn != NULL && strlen(extn) >= 2) {

		/* if an extension is supplied by the calling C routine, that indicates that the array could be output as part of a structure */
		/* if we have got this far, we know there are no specific instructions regarding this array - therefore look for a command to write the whole structure */

		temp[strlen(temp) - strlen(extn)] = 0;
		if(IdentifierAppearsInBatch(batch, temp)) {
			addTitle = TRUE;
			m->warnIfEmpty = FALSE;
			str = FindVariableInBatch(batch, temp, &len, uniqueOccurrence);
			while(len > 3 && str[0] == '-' && isspace(str[2]) &&
				( (sw = tolower(str[1])) == 'a' || sw == 't' || sw == 'n')) {
				switch(sw) {
					case 'a': strcpy(m->writeMode, "a"); break;
					case 't': addTitle = TRUE; break;
					case 'n': addTitle = FALSE; break;
				}
				for(str += 3, len -= 3; isspace(*str); str++) len--;
			}
			m->output = ReadString(str, len, NULL, NULL);
#ifdef MATLAB_MEX_FILE
			if(mexEvalf("MEX__TEMP = struct('a', 1); clear MEX__TEMP")!=0) JError("structs cannot be used in MATLAB v.4 : cannot implement  #%s", temp);
			if(*m->writeMode == 'a') {strcpy(m->writeMode, "w"); JWarning("in MATLAB the -a switch has no effect when using #%s to write a whole struct", temp);}
			sprintf(temp, ".%s", extn+1);
			m->output = ResizeBlock(m->output, strlen(m->output) + strlen(temp) + 1);
			sprintf(m->output + strlen(m->output), "%s", temp);
			addTitle = FALSE;
#else
			if(strcmp(extn+1, "est") != 0) strcpy(m->writeMode, "a"); /* after _EST, all the others are appended */
#endif
		}
	}
	if(addTitle) {
		if(m->description) { Destroy(m->description); m->description = NULL;}
		sprintf(temp, "%s%s", ident, (extn ? extn : ""));
		for(str = temp; *str; str++) *str = toupper(*str);
		strcpy((m->description = New(char, strlen(temp) + 1)), temp);
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void CleanUp(DataSetPtr data, ModelPtr model, GeneratingInfoPtr gen, OutputPtr out)
{
    DEBUG=0;
    m_clear();
	DisposeDataSet(data);
	if(data) Destroy(data);
	if(out && out->conf) Destroy(out->conf);
	if(out && out->cuts) Destroy(out->cuts);
	if(gen && gen->psi) Destroy(gen->psi);

	if(out) Destroy(out);
	if(gen) Destroy(gen);
	if(model) Destroy(model);
    DEBUG=0;
	ReportBlocks();
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
DataSetPtr ConstructDataSet(int nPoints, int rowSkip, double *x, double *y, double *n, double *r, double *w, char *sourceDescription)
{
	DataSetPtr d;
	int count, i;
	struct{boolean r; boolean w; boolean n; boolean y;} flags = {0, 0, 0, 0};
	struct{double r; double w; double n; double y;} vals = {0, 0, 0, 0};
	boolean already, yInt, yGtN;
	double previous, scale = 1.0;

	if(rowSkip < 1) Bug("ConstructDataSet() called with rowSkip < 1");
/*	if(x == NULL) JError("error in %s: x values must be supplied", sourceDescription);
*/

	if(n == NULL && w == NULL) JError("insufficient information in data set: need numbers of points in each block");
	if(gDataFormat == unknown_format) {
		if(!(n != NULL && r == NULL && w == NULL)) Bug("for ConstructDataSet() to guess format, data must be supplied in x,y and n");
		if(y == NULL) yInt = FALSE;
		else {
			for(yInt = TRUE, i = 0, count = 0; count < nPoints; count++, i += rowSkip)
				if(y[i] != floor(y[i])) {yInt = FALSE; break;}
		}
		if(!yInt) gDataFormat = xyn;
		else {
			for(yGtN = FALSE, i = 0, count = 0; count < nPoints; count++, i += rowSkip)
				if(y[i] > n[i]) {yGtN = TRUE; break;}
			if(yGtN) {gDataFormat = xrw; r = y; w = n; y = NULL; n = NULL;}
			else {gDataFormat = xrn; r = y; y = NULL;}
		}
	}

	for(count = 0, i = 0; count < nPoints; count++, i += rowSkip) {
        if((x && (isnan(x[i]) || isinf(x[i]))) || (y && (isnan(y[i]) || isinf(y[i]))) || (n && (isnan(n[i]) || isinf(n[i]))) || (r && (isnan(r[i]) || isinf(r[i]))) || (w && (isnan(w[i]) || isinf(w[i])))) {
			JError("error in %s: illegal non-real values", sourceDescription);
            return 0;
        }
		if(y && y[i] > 100.0) JError("error in %s: illegal y values > 100.0", sourceDescription);
		if(y && y[i] > 1.0) scale = 100.0;
        if((y && y[i] < 0.0) || (n && n[i] < 0.0) || (r && r[i] < 0.0) || (w && w[i] < 0.0)) {
			JError("error in %s: illegal negative values", sourceDescription);
            return 0;
        }
        if((n && n[i] != floor(n[i])) || (r && r[i] != floor(r[i])) || (w && w[i] != floor(w[i]))) {
			JError("error in %s: illegal non-integer numbers of responses", sourceDescription);
            return 0;
        }
	}

	d = New(DataSet, 1);
	AllocateDataSet(d, nPoints);

#define mismatch(t, v)	((already = flags.t, previous = vals.t, vals.t = (v), flags.t = TRUE, already) && (previous != vals.t))

	for(count = 0, i = 0; count < nPoints; count++, i += rowSkip) {
		d->x[count] = (x ? x[i] : NAN);

		if((flags.r = (r != NULL)) == TRUE) vals.r = floor(0.5 + r[i]);
		if((flags.w = (w != NULL)) == TRUE) vals.w = floor(0.5 + w[i]);
		if((flags.n = (n != NULL)) == TRUE) vals.n = floor(0.5 + n[i]);
		if((flags.y = (y != NULL)) == TRUE) vals.y = y[i] / scale;
		if(flags.y && flags.n && mismatch(r, floor(0.5 + vals.y * vals.n))) break;
		if(flags.r && flags.n && mismatch(w, vals.n - vals.r)) break;
		if(flags.w && flags.n && mismatch(r, vals.n - vals.w)) break;
		if(flags.r && flags.w && mismatch(n, vals.r + vals.w)) break;
		if(!flags.r && n != NULL && y == NULL) flags.r = TRUE, vals.r = 0.0;
		if(!flags.w && n != NULL && y == NULL) flags.w = TRUE, vals.w = floor(0.5 + n[i]);

		if(!flags.r || !flags.w) JError("insufficient information in %s", sourceDescription);
		d->nRight[count] = vals.r;
		d->nWrong[count] = vals.w;
		if(vals.n < 1.0) JError("error in %s: number of observations < 1 at one or more points", sourceDescription);
	}
	if(count < nPoints) JError("%s provide conflicting information", sourceDescription);
	return d;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void InitMatrixBundle(MatrixBundle *bndl, GeneratingInfoPtr gen, OutputPtr out,
					  long nCols, boolean valid, boolean doLimits, boolean bcaPossible,
					  char *identStem, BatchPtr batch)
{
	/* reverse order */
	if(doLimits) {

		if(gDoBootstrapT) {
			bndl->t2 = m_new(mNoData, m2D, ((valid && gen->nRuns > 0 && bcaPossible) ? gen->nRuns : 0), nCols);
			AssignOutput(bndl->t2, batch, identStem, "_t2", out->numericFormat);

			bndl->t1 = m_new(mNoData, m2D, ((valid && gen->nRuns > 0 && bcaPossible) ? gen->nRuns : 0), nCols);
			AssignOutput(bndl->t1, batch, identStem, "_t1", out->numericFormat);
		}
		else bndl->t2 = bndl->t1 = NULL;

		bndl->quant = m_new(mNoData, m2D, ((valid && gen->nRuns > 0) ? out->nConf : 0), nCols);
		AssignOutput(bndl->quant, batch, identStem, "_quant", out->numericFormat);

		bndl->lims = m_new(mNoData, m2D, ((valid && bcaPossible && gen->nRuns > 0) ? out->nConf : 0), nCols);
		AssignOutput(bndl->lims, batch, identStem, "_lims", out->numericFormat);

		bndl->acc = m_new(mNoData, m2D, ((valid && bcaPossible && gen->nRuns > 0) ? 1 : 0), nCols);
		AssignOutput(bndl->acc, batch, identStem, "_acc", out->numericFormat);

		bndl->bc = m_new(mNoData, m2D, ((valid && bcaPossible && gen->nRuns > 0) ? 1 : 0), nCols);
		AssignOutput(bndl->bc, batch, identStem, "_bc", out->numericFormat);

		bndl->lff = m_new(mNoData, m2D, ((valid && bcaPossible) ? kNumberOfParams : 0), nCols);
		AssignOutput(bndl->lff, batch, identStem, "_lff", out->numericFormat);

		bndl->deriv = m_new(mNoData, m2D, ((valid && bcaPossible) ? kNumberOfParams : 0), nCols);
		AssignOutput(bndl->deriv, batch, identStem, "_deriv", out->numericFormat);
	}
	else bndl->quant = bndl->lims = bndl->acc = bndl->bc = bndl->lff = bndl->deriv = NULL;

	bndl->cpe = m_new(mNoData, m2D, ((valid && gen->nRuns > 0) ? 1 : 0), nCols);
	AssignOutput(bndl->cpe, batch, identStem, "_cpe", out->numericFormat);

	bndl->sim = m_new(mNoData, m2D, (valid ? gen->nRuns : 0), nCols);
	AssignOutput(bndl->sim, batch, identStem, "_sim", out->numericFormat);

	bndl->est = m_new(mNoData, m2D, (valid ? 1 : 0), nCols);
	AssignOutput(bndl->est, batch, identStem, "_est", out->numericFormat);

}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void InitPrefs(BatchPtr prefs, ModelPtr * handleForModel,
							   DataSetPtr * handleForData,
							   GeneratingInfoPtr * handleForGeneratingInfo,
							   OutputPtr * handleForOutput,
							   matrix externalData)
{
#define kBufferLength	30
	int chosenOpt, thisOpt, fieldLen;
	char identBuffer[kBufferLength], nameBuffer[kBufferLength], tempBuffer[kBufferLength], *s, *fieldStart = NULL;
	unsigned int i, pNum, xLength = 0, yLength = 0, nLength = 0, rLength = 0, wLength = 0, dLength = 0, nPoints;
	double *x = NULL, *y = NULL, *n = NULL, *r = NULL, *w = NULL, *d = NULL;
	double temp[kNumberOfParams + 2], *fixedValPtr;
	ModelPtr model;
	GeneratingInfoPtr gen;
	OutputPtr out;
	boolean started, finished, *specified = NULL, needData, bcaPossible, gotX;
	boolean writeCommandsSpecified = FALSE;
	ConstraintPtr constraintPtr;
	double *col1, *col2, *col3;

	char *adaptiveMethod = NULL;
	double *adaptiveParams = NULL, *adaptiveLimits = NULL;
	unsigned int adaptiveParamCount = 0;

	model = *handleForModel = New(Model, 1);
	gen = *handleForGeneratingInfo = New(GeneratingInfo, 1);
	out = *handleForOutput = New(Output, 1);
	needData = (externalData == NULL);

	InitParam(model, ALPHA, "alpha");
	InitParam(model, BETA, "beta");
	InitParam(model, GAMMA, "gamma");
	InitParam(model, LAMBDA, "lambda");

#define kDefaultSubjectiveGammaMax		0.05
#define kDefaultLambdaMax				0.05

#define option(str, defaultAssign)											\
			thisOpt++;														\
			if(finished && !specified[thisOpt]) (defaultAssign);			\
			else if(started && strcmp(str, identBuffer) == 0 && !specified[(chosenOpt = thisOpt)] && (specified[thisOpt] = TRUE) != FALSE)

	finished = FALSE; started = FALSE; *identBuffer = *tempBuffer = 0;
	while(!finished) {
		chosenOpt = thisOpt = -1;

                // Get next #string from file
		if(started && (prefs == 0 || (fieldStart = NextIdentifier(prefs, &fieldLen, identBuffer, kBufferLength, lastOccurrence)) == NULL))
			finished = TRUE;

		option("SHAPE", model->shape = JLogistic) {
			ReadString(fieldStart, fieldLen, tempBuffer, (i = kBufferLength, &i));
			model->shape = MatchShape(tempBuffer, identBuffer);
		}
		option("GEN_SHAPE", gen->shape = model->shape) {
			ReadString(fieldStart, fieldLen, tempBuffer, (i = kBufferLength, &i));
			gen->shape = MatchShape(tempBuffer, identBuffer);
		}
		if(finished && gen->shape != model->shape && !specified[thisOpt + 1])
			JError("a different shape has been given for the generating function\nbut generating parameters have not been specified");
		/* GEN_PARAMS must come directly after GEN_SHAPE in this list because of the thisOpt+1 above */
		option("GEN_PARAMS", gen->gotParams = FALSE) {
			ReadDoubles(fieldStart, fieldLen, temp, &i, kNumberOfParams, kNumberOfParams, identBuffer);
			for(pNum = 0; pNum < kNumberOfParams; pNum++) {
				sprintf(tempBuffer, "GEN_PARAMS element #%d", pNum + 1);
				gen->params[pNum] = CheckValue(temp[pNum], tempBuffer, -INF, INF, FALSE, TRUE, TRUE);
			}
			gen->gotParams = TRUE;
		}
		/* GEN_VALUES must come directly after GEN_PARMS in this list because of the thisOpt-1 below */
		option("GEN_VALUES", (gen->psi = NULL, gen->nPoints = 0)) {
			gen->psi = ReadDoubles(fieldStart, fieldLen, NULL, &i, 1, (unsigned int)(-1), identBuffer);
			gen->nPoints = i;
		}
		if(finished && gen->psi != NULL && specified[thisOpt-1]) JError("conflicting options: generating distribution has been specified both as a parameter set and as a set of expected values");

		option("N_INTERVALS", model->nIntervals = 2)
			model->nIntervals = (unsigned short)CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 1.0, INF, TRUE, TRUE, TRUE);

		option("MAX_TAIL_DRIFT", model->tailConstraint.prior = NULL) {
			temp[0] = 0.0; temp[1] = ReadScalar(fieldStart, fieldLen, identBuffer);
			if(temp[1] <= 0.0) JError("%s cannot be <= 0.0: set to NaN or a value >= 1.0 to disable the prior", identBuffer);
			if(!isnan(temp[1]) && temp[1] < 1.0) SetPrior(&model->tailConstraint, FlatPrior, 2, temp);
			else model->tailConstraint.prior = NULL;
		}
		option("X_AT_CHANCE", model->xValAtChance = 0.0)
			model->xValAtChance = CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, -INF, INF, FALSE, FALSE, TRUE);

/*		start of priors / fixing section (take a deep breath) */
		for(pNum = 0; pNum < kNumberOfParams+2; pNum++) {

			switch(pNum) {
				case kNumberOfParams + 1:
					strcpy(nameBuffer, "SLOPE");
					constraintPtr = &model->slopeConstraint;
					fixedValPtr = &model->fixedSlope;
					break;
				case kNumberOfParams:
					strcpy(nameBuffer, "SHIFT");
					constraintPtr = &model->shiftConstraint;
					fixedValPtr = &model->fixedShift;
					break;
				default:
					strcpy(nameBuffer, model->theta[pNum].name);
					constraintPtr = &model->theta[pNum].constraint;
					fixedValPtr = temp;
					break;
			}
			for(s = nameBuffer; *s; s++) *s = toupper(*s);

			sprintf(tempBuffer, "%s_LIMITS", nameBuffer);
			option(tempBuffer, (void)0) {
				ReadDoubles(fieldStart, fieldLen, constraintPtr->args, &i, 2, 2, identBuffer);
				SetPrior(constraintPtr, FlatPrior, i, constraintPtr->args);
			}
			sprintf(tempBuffer, "%s_PRIOR", nameBuffer);
			option(tempBuffer, (void)0) {
/*				%s_LIMITS and %s_PRIOR must stay together in the list because of the occurrence of [thisOpt-1] below	*/
				if(specified[thisOpt-1]) JError("%s: cannot use _PRIOR and _LIMITS simultaneously on the same parameter", identBuffer);
				for(i = 0; fieldLen > 0 && i < kBufferLength-1; i++, fieldStart++, fieldLen--) {
					if(i == 1 && tempBuffer[0] == '-') i = 0;
					if(isspace(*fieldStart)) {do {fieldStart++; fieldLen--;} while(fieldLen > 0 && isspace(*fieldStart)); break;}
					if(isdigit(*fieldStart) || *fieldStart == '.') break;
					tempBuffer[i] = *fieldStart;
				}
				tempBuffer[i] = 0;
				if(i == 0) JError("%s: no functional form supplied", identBuffer);
				ReadDoubles(fieldStart, fieldLen, constraintPtr->args, &i, 0, kMaxPriorArgs, identBuffer);
				switch(MatchString(identBuffer, tempBuffer, FALSE, TRUE, TRUE, 6,
					"none", "flat", "cosine", "beta", "Gaussian", "fixed")) {
					case 1:
						SetPrior(constraintPtr, NULL, 0, NULL);
						if(i > 0) JWarning("ignoring redundant numeric arguments to %s -none", identBuffer);
						break;
					case 2: SetPrior(constraintPtr, FlatPrior, i, constraintPtr->args); break;
					case 3: SetPrior(constraintPtr, CosinePrior, i, constraintPtr->args); break;
					case 4: SetPrior(constraintPtr, BetaPrior, i, constraintPtr->args); break;
					case 5: SetPrior(constraintPtr, GaussianPrior, i, constraintPtr->args); break;
					case 6:
						if(i != 1) JError("%s with option \"fixed\" should have 1 numeric argument", identBuffer);
						SetPrior(constraintPtr, NULL, 0, NULL);
						specified[thisOpt] = FALSE;
						sprintf(identBuffer, "FIX_%s", nameBuffer);
						break;
					default: JError("Unknown prior function \"%s\"", tempBuffer);
				}
			}
/*			%s_LIMITS and %s_PRIOR must stay together in the list because of the occurrence of [thisOpt-1] below	*/
			if(finished && !specified[thisOpt] && !specified[thisOpt-1]) {
/*				%s_PRIOR and FIX_%s must stay together in this list because of the occurrences of [thisOpt+1] below	*/
				if(pNum == GAMMA && model->nIntervals == 1 && !specified[thisOpt+1]) {
					temp[0] = 0.0; temp[1] = kDefaultSubjectiveGammaMax;
					SetPrior(constraintPtr, FlatPrior, 2, temp);
				}
/*				%s_PRIOR and FIX_%s must stay together in this list because of the occurrences of [thisOpt+1] below	*/
				else if(pNum == LAMBDA && !specified[thisOpt+1]) {
					temp[0] = 0.0; temp[1] = kDefaultLambdaMax;
					SetPrior(constraintPtr, FlatPrior, 2, temp);
				}
				else constraintPtr->prior = NULL;
			}
			sprintf(tempBuffer, "FIX_%s", nameBuffer);
			option(tempBuffer, (void)0)
				if(!isnan(*fixedValPtr = CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, -INF, INF, FALSE, TRUE, FALSE)) && pNum < kNumberOfParams)
					FixParam(model->theta, pNum, *fixedValPtr);
			if(finished && !specified[thisOpt]) {
			 	if(pNum == GAMMA && model->nIntervals > 1)
					FixParam(model->theta, pNum, 1.0 / (double)(model->nIntervals));
				if(pNum >= kNumberOfParams) *fixedValPtr = NAN;
			}
		}
		if(finished) {
			if(!isnan(model->fixedShift) || !isnan(model->fixedSlope) || strcmp(FunctionName(model->shape), "cg2")==0) {
				if(!model->theta[ALPHA].free || !model->theta[BETA].free) JError("cannot use FIX_SHIFT or FIX_SLOPE in conjunction with FIX_ALPHA or FIX_BETA");
				if(model->theta[ALPHA].constraint.prior != NULL || model->theta[BETA].constraint.prior != NULL) JError("cannot use FIX_SHIFT or FIX_SLOPE in conjunction with ALPHA_PRIOR or BETA_PRIOR");
				if(!isnan(model->fixedShift)) FixParam(model->theta, ALPHA, model->fixedShift);
				if(!isnan(model->fixedSlope)) FixParam(model->theta, BETA, model->fixedSlope);
				model->convertParams = TRUE;
			}
			else model->convertParams = FALSE;
		}
/*		end of priors / fixing section */

		option("EST_GAMMA", gEstimateGamma = ((model->nIntervals == 1) ? 0.01 : 1.0 / model->nIntervals))
			gEstimateGamma = CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 0.0, 1.0, FALSE, TRUE, TRUE);

		option("EST_LAMBDA", gEstimateLambda = 0.01)
			gEstimateLambda = CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 0.0, 1.0, FALSE, TRUE, TRUE);

		option("MESH_RESOLUTION", gMeshResolution = 11)
			gMeshResolution = (unsigned short)CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 5.0, INF, TRUE, TRUE, TRUE);

		option("MESH_ITERATIONS", gMeshIterations = 10)
			gMeshIterations = (unsigned short)CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 3.0, INF, TRUE, TRUE, TRUE);

		option("RUNS", gen->nRuns = 0)
			gen->nRuns = (unsigned long)CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 0.0, INF, TRUE, TRUE, TRUE);

		option("RANDOM_SEED", gen->randomSeed = labs((long)time(NULL))) {
			gen->randomSeed = temp[0] = CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 0.0, INF, TRUE, TRUE, TRUE);
			if((double)gen->randomSeed != temp[0]) JError("the user-supplied random seed overflowed the internal integer representation for random numbers");
		}

		option("VERBOSE", out->verbose = TRUE)
			out->verbose = ReadBoolean(fieldStart, fieldLen, identBuffer);

		option("COMPUTE_PARAMS", out->doParams = TRUE)
			out->doParams = ReadBoolean(fieldStart, fieldLen, identBuffer);

		option("COMPUTE_STATS", out->doStats = TRUE)
			out->doStats = ReadBoolean(fieldStart, fieldLen, identBuffer);

		option("LAMBDA_EQUALS_GAMMA", gLambdaEqualsGamma = FALSE)
			gLambdaEqualsGamma = ReadBoolean(fieldStart, fieldLen, identBuffer);

/*		COMPUTE_STATS and ADAPTIVE_... options must stay together in this list because of the reference to [thisOpt-3], below. */

		option("ADAPTIVE_METHOD", adaptiveMethod = NULL)
			adaptiveMethod = ReadString(fieldStart, fieldLen, NULL, NULL);
		option("ADAPTIVE_PARAMS", adaptiveParams = NULL)
			adaptiveParams = ReadDoubles(fieldStart, fieldLen, NULL, &adaptiveParamCount, 0, 0, identBuffer);
		option("ADAPTIVE_LIMITS", adaptiveLimits = NULL)
			adaptiveLimits = ReadDoubles(fieldStart, fieldLen, NULL, NULL, 2, 2, identBuffer);
		if(finished) {
			gAdaptPtr = CSetUpAdaptiveProcedure(adaptiveMethod, adaptiveParamCount, adaptiveParams, adaptiveLimits);
			if(adaptiveMethod) Destroy(adaptiveMethod);
			if(adaptiveParams) Destroy(adaptiveParams);
			if(adaptiveLimits) Destroy(adaptiveLimits);
			if(gAdaptPtr) {
				if(gen->psi != NULL) JError("GEN_VALUES option cannot be used with adaptive procedures");
				if(gen->gotParams) needData = FALSE;
				if(specified[thisOpt - 3] && out->doStats == TRUE) JWarning("COMPUTE_STATS has been overridden because adaptive procedures are being used");
				out->doStats = FALSE;
			}
		}

		option("SENS", out->sensNPoints = 8)
			out->sensNPoints = (short)(0.5 + CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 0.0, 16.0, TRUE, TRUE, TRUE));

		option("SENS_COVERAGE", out->sensCoverage = 0.5) /* 0.683) */
			if((out->sensCoverage = CheckValue(ReadScalar(fieldStart, fieldLen, identBuffer), identBuffer, 0.0, 100.0, FALSE, TRUE, TRUE)) > 1.0)
				out->sensCoverage /= 100.0;

		option("SLOPE_OPT", gLogSlopes = FALSE) {
			ReadString(fieldStart, fieldLen, tempBuffer, (i = kBufferLength, &i));
			switch(MatchString(identBuffer, tempBuffer, FALSE, TRUE, TRUE, 2,
				"linear x", "log x")) {
				case 1: gLogSlopes = FALSE; break;
				case 2: gLogSlopes = TRUE; break;
				default: JError("Unknown %s \"%s\"", identBuffer, tempBuffer);
			}
		}

		gCutPsi = FALSE;
/*		N.B: the gCutPsi option was disabled 19/10/99 because of the unnecessary complications it causes in finding threshold and slope derivatives
		option("CUT_OPT", gCutPsi = FALSE) {
			ReadString(fieldStart, fieldLen, tempBuffer, (i = kBufferLength, &i));
			switch(MatchString(identBuffer, tempBuffer, FALSE, TRUE, TRUE, 2,
				"underlying", "performance")) {
				case 1: gCutPsi = FALSE; break;
				case 2: gCutPsi = TRUE; break;
				default: JError("Unknown %s \"%s\"", identBuffer, tempBuffer);
			}
		}
*/
		option("CUTS", (out->cuts = NULL, out->nCuts = 1)) {
			out->cuts = ReadDoubles(fieldStart, fieldLen, NULL, &out->nCuts, 0, 0, identBuffer);
			if(out->nCuts == 1 && isnan(out->cuts[0])) {Destroy(out->cuts); out->cuts = NULL; out->nCuts = 0;};
			for(i = 0; i < out->nCuts; i++) if(out->cuts[i] > 1.0) break;
			temp[0] = ((i < out->nCuts) ? 0.01 : 1.0);
			for(i = 0; i < out->nCuts; i++) out->cuts[i] = temp[0] * CheckValue(out->cuts[i], identBuffer, 0.0, 100.0, FALSE, TRUE, TRUE);
			if(out->nCuts) SortDoubles(1, out->nCuts, out->cuts);
		}
		if(finished && out->cuts == NULL && out->nCuts == 1) {
			out->cuts = New(double, (out->nCuts = 3));
			out->cuts[0] = 0.2; out->cuts[1] = 0.5; out->cuts[2] = 0.8;
			if(gCutPsi && model->nIntervals > 1) for(i = 0; i < out->nCuts; i++) out->cuts[i] = out->cuts[i] * (1.0 - 1.0/(double)model->nIntervals) + 1.0/(double)model->nIntervals;
		}

		option("CONF", (out->conf = NULL, out->nConf = 1)) {
			out->conf = ReadDoubles(fieldStart, fieldLen, NULL, &out->nConf, 0, 0, identBuffer);
			if(out->nConf == 1 && isnan(out->conf[0])) {Destroy(out->conf); out->conf = NULL; out->nConf = 0;};
			for(i = 0; i < out->nConf; i++) if(out->conf[i] > 1.0) break;
			temp[0] = ((i < out->nConf) ? 0.01 : 1.0);
			for(i = 0; i < out->nConf; i++) out->conf[i] = temp[0] * CheckValue(out->conf[i], identBuffer, 0.0, 100.0, FALSE, TRUE, TRUE);
			if(out->nConf) SortDoubles(1, out->nConf, out->conf);
		}
		if(finished && out->conf == NULL && out->nConf == 1) {
			out->conf = New(double, (out->nConf = 4));
			out->conf[0] = 0.023; out->conf[1] = 0.159; out->conf[2] = 0.841; out->conf[3] = 0.977;
		}

		option("REFIT", out->refit = (gen->nRuns > 0 && out->doParams && gen->psi == NULL && !gen->gotParams && gen->shape == model->shape))
			out->refit = ReadBoolean(fieldStart, fieldLen, identBuffer);
		if(finished && out->refit) {
			if(!out->doParams) JError("cannot use the REFIT option when COMPUTE_PARAMS is disabled");
			if(gen->psi != NULL || gen->gotParams || gen->shape != model->shape)
				JError("cannot use the REFIT option when a custom generating distribution is specified via the GEN_... options");
		}

		option("DATA_X", x = NULL) x = ReadDoubles(fieldStart, fieldLen, NULL, &xLength, 0, 0, identBuffer);
		option("DATA_Y", y = NULL) y = ReadDoubles(fieldStart, fieldLen, NULL, &yLength, 0, 0, identBuffer);
		option("DATA_N", n = NULL) n = ReadDoubles(fieldStart, fieldLen, NULL, &nLength, 0, 0, identBuffer);
		option("DATA_RIGHT", r = NULL) r = ReadDoubles(fieldStart, fieldLen, NULL, &rLength, 0, 0, identBuffer);
		option("DATA_WRONG", w = NULL) w = ReadDoubles(fieldStart, fieldLen, NULL, &wLength, 0, 0, identBuffer);
		option("DATA", d = NULL) d = ReadDoubles(fieldStart, fieldLen, NULL, &dLength, 0, 0, "data matrix");

		option("MATRIX_FORMAT", gDataFormat = unknown_format) {
			ReadString(fieldStart, fieldLen, tempBuffer, (i = kBufferLength, &i));
			switch(MatchString(identBuffer, tempBuffer, FALSE, FALSE, TRUE, 3,
				   "XYN", "XRW", "XRN")) {
				case 1: gDataFormat = xyn; break;
				case 2: gDataFormat = xrw; break;
				case 3: gDataFormat = xrn; break;
				default: JError("Unknown format \"%s\"", tempBuffer);
			}
		}

		option("DO_BOOTSTRAP_T", gDoBootstrapT = FALSE)
			gDoBootstrapT = ReadBoolean(fieldStart, fieldLen, identBuffer);

		option("WRITE_FORMAT", strcpy(out->numericFormat, "%lg")) {
			ReadString(fieldStart, fieldLen, out->numericFormat, (i = mNumericFormatLength + 1, &i));
			if(i > mNumericFormatLength) JError("%s cannot be more than %d characters", identBuffer, mNumericFormatLength);
		}

		if(strncmp(identBuffer, "WRITE_", 6) == 0 && *(s = identBuffer + 6)) {
			if(strcmp(s, "RANDOM_SEED") == 0) chosenOpt = -4;			/* -4: recognized - will be dealt with later */
			else if(strcmp(s, "ADAPTIVE_OUTPUT") == 0) chosenOpt = -4;
			else if(strcmp(s, "ADAPTIVE_TARGET") == 0) chosenOpt = -4;
			else if(strcmp(s, "DATA") == 0) chosenOpt = -4;
			else if(strcmp(s, "IN_REGION") == 0) chosenOpt = -4;
			else if(strcmp(s, "SENS_PARAMS") == 0) chosenOpt = -4;
			else if(strcmp(s, "LDOT") == 0) chosenOpt = -4;
			else if(strcmp(s, "FISHER") == 0) chosenOpt = -4;
			else if(strcmp(s, "Y_SIM") == 0) chosenOpt = -4;
			else if(strcmp(s, "R_SIM") == 0) chosenOpt = -4;
			else if(strcmp(s, "COV") == 0) chosenOpt = -4;
			else if(strncmp(s, "ST", 2) == 0) s += 2, chosenOpt = -3;	/* -3: possibly recognized - check rest of string for one of a limited selection of the endings below */
			else if(strncmp(s, "PA", 2) == 0) s += 2, chosenOpt = -2;	/* -2: possibly recognized - check rest of string for one of the endings below */
			else if(strncmp(s, "TH", 2) == 0) s += 2, chosenOpt = -2;
			else if(strncmp(s, "SL", 2) == 0) s += 2, chosenOpt = -2;
			if(chosenOpt == -2 || chosenOpt == -3) {
				if(*s == 0) chosenOpt = -4; /* whole structure */
				else if(strcmp(s, "_EST") == 0) chosenOpt = -4;
				else if(strcmp(s, "_SIM") == 0) chosenOpt = -4;
				else if(strcmp(s, "_CPE") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_DERIV") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_LFF") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_BC") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_ACC") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_LIMS") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_QUANT") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_T1") == 0) chosenOpt = -4;
				else if(chosenOpt == -2 && strcmp(s, "_T2") == 0) chosenOpt = -4;
				else chosenOpt = -1; /* -1: not recognized after all */
			}
			if(chosenOpt == -4) writeCommandsSpecified = TRUE;
		}


/*		end of options loop */
		if(!started) {
			started = TRUE;
			specified = New(boolean, thisOpt+1);
		}
		else if(!finished && chosenOpt == -1) JError("Unrecognized option \"%s\"", identBuffer);
	}

	Destroy(specified);

	*handleForData = NULL;

	if(externalData && m_mass(externalData) > 0) {
		if(m_getsize(externalData, 2) != 3) JError("data matrix must have three columns");
		nPoints = m_getsize(externalData, 1);
                if(m_mass(externalData) != (int) nPoints * 3) JError("data matrix must be two-dimensional");
		m_first(externalData);
		col1 = m_addr(externalData, 2, 0);
		col2 = m_addr(externalData, 2, 1);
		col3 = m_addr(externalData, 2, 2);
		switch(gDataFormat) {
			case unknown_format:
			case xyn: *handleForData = ConstructDataSet(nPoints, m_getstep(externalData, 1), col1, col2, col3, NULL, NULL, "data matrix"); break;
			case xrw: *handleForData = ConstructDataSet(nPoints, m_getstep(externalData, 1), col1, NULL, NULL, col2, col3, "data matrix"); break;
			case xrn: *handleForData = ConstructDataSet(nPoints, m_getstep(externalData, 1), col1, NULL, col3, col2, NULL, "data matrix"); break;
		}
	}

	if(dLength > 0) {
		if(*handleForData != NULL) JWarning("the data matrix in the preference string will be ignored");
		else {
			if(FindVariableInBatch(prefs, "DATA", &fieldLen, firstOccurrence) != FindVariableInBatch(prefs, "DATA", &fieldLen, lastOccurrence))
				JWarning("one or more data matrices in the preference string will be ignored");
			if(dLength % 3) JError("if data are supplied as text, the matrix should have three columns");
			dLength /= 3;
			switch(gDataFormat) {
				case unknown_format:
				case xyn: *handleForData = ConstructDataSet(dLength, 3, d, d+1, d+2, NULL, NULL, "data matrix"); break;
				case xrw: *handleForData = ConstructDataSet(dLength, 3, d, NULL, NULL, d+1, d+2, "data matrix"); break;
				case xrn: *handleForData = ConstructDataSet(dLength, 3, d, NULL, d+2, d+1, NULL, "data matrix"); break;
			}
		}
		Destroy(d);
	}

	nPoints = (xLength ? xLength : yLength ? yLength : nLength ? nLength : rLength ? rLength : wLength ? wLength : 0);
	if(nPoints > 0) {
		if(*handleForData != NULL) JWarning("data given by #DATA_... fields of preference string will be ignored");
		else {
/*			if(xLength == 0) JError("if data are supplied in preference string, DATA_X must be included");
*/			if((yLength > 0 && yLength != nPoints) || (nLength > 0 && nLength != nPoints)
			|| (rLength > 0 && rLength != nPoints) || (wLength > 0 && wLength != nPoints))
				JError("lengths of DATA fields are mismatched");


			*handleForData = ConstructDataSet(nPoints, 1, x, y, n, r, w, "DATA fields of preference string");
		}
		if(xLength) Destroy(x);
		if(yLength) Destroy(y);
		if(nLength) Destroy(n);
		if(rLength) Destroy(r);
		if(wLength) Destroy(w);
	}

	if(*handleForData == NULL) {
        if(needData) {
            JError("no data supplied!");
            return;
        } else {
			*handleForData = New(DataSet, 1);
			(*handleForData)->nPoints = 0;
			(*handleForData)->x = (*handleForData)->nRight = (*handleForData)->nWrong = NULL;
		}
	}

	gotX = (*handleForData != NULL && (*handleForData)->x != NULL && (*handleForData)->nPoints > 0 && !isnan((*handleForData)->x[0]));
	bcaPossible = (gen->shape == model->shape && gen->psi == NULL && gotX);

	/* assign matrix outputs in reverse order */

	AssignOutput((out->randomSeed = m_new(mNewData, m1D, 1)), prefs, "RANDOM_SEED", NULL, "%.20lg");
	m_val(out->randomSeed) = gen->randomSeed;

	AssignOutput((out->dataExport = m_new(mNoData, m2D, 0, 3)), prefs, "DATA", NULL, out->numericFormat);
	out->dataExportIndex = 0;
	if(out->dataExport->output) {
		out->dataExportIndex = strtoul(out->dataExport->output, &s, 10);
		while(isspace(*s) || *s == ',') s++;
		memmove(out->dataExport->output, s, strlen(s) + 1);
	}

	AssignOutput((gAdaptiveOutput = m_new(mNoData, m2D, gen->nRuns, 0)), prefs, "ADAPTIVE_OUTPUT", NULL, out->numericFormat);
	AssignOutput((gAdaptiveTarget = m_new(mNoData, m2D, 1, 2)), prefs, "ADAPTIVE_TARGET", NULL, out->numericFormat);
	AssignOutput((out->inRegion = m_new(mNoData, m2D, ((out->doParams && bcaPossible) ? gen->nRuns : 0), 1)), prefs, "IN_REGION", NULL, "%lg");
	AssignOutput((out->sensParams = m_new(mNoData, m2D, out->sensNPoints, kNumberOfParams)), prefs, "SENS_PARAMS", NULL, out->numericFormat);
	AssignOutput((out->ldot = m_new(mNoData, m2D, ((out->doParams && bcaPossible) ? gen->nRuns : 0), kNumberOfParams)), prefs, "LDOT", NULL, out->numericFormat);
	AssignOutput((out->pcov = m_new(mNoData, m2D, ((out->doParams && bcaPossible) ? kNumberOfParams : 0), kNumberOfParams)), prefs, "COV", NULL, out->numericFormat);
	AssignOutput((out->fisher = m_new(mNoData, m2D, ((out->doParams && bcaPossible) ? kNumberOfParams : 0), kNumberOfParams)), prefs, "FISHER", NULL, out->numericFormat);
	InitMatrixBundle(&out->slopes, gen, out, out->nCuts, out->doParams, TRUE, bcaPossible, "SL", prefs);
	InitMatrixBundle(&out->thresholds, gen, out, out->nCuts, out->doParams, TRUE, bcaPossible, "TH", prefs);
	InitMatrixBundle(&out->stats, gen, out, kNumberOfStats, out->doStats, FALSE, bcaPossible, "ST", prefs);
	InitMatrixBundle(&out->params, gen, out, kNumberOfParams, out->doParams, TRUE, bcaPossible, "PA", prefs);
	if(gen->nRuns > 0 && (gen->psi != NULL || gen->shape != model->shape)) {
		m_setsize(out->params.est, m2D, 0, kNumberOfParams); /* params will not be comparable */
		m_setsize(out->params.cpe, m2D, 0, kNumberOfParams);
	}
	if(gen->nRuns > 0 && gen->psi) {
		m_setsize(out->thresholds.est, m2D, 0, kNumberOfParams);
		m_setsize(out->thresholds.cpe, m2D, 0, kNumberOfParams);
		m_setsize(out->slopes.est, m2D, 0, kNumberOfParams);
		m_setsize(out->slopes.cpe, m2D, 0, kNumberOfParams);
	}
	AssignOutput((out->ySim = m_new(mNoData, m2D, gen->nRuns, (*handleForData)->nPoints)), prefs, "Y_SIM", NULL, out->numericFormat);
	AssignOutput((out->rSim = m_new(mNoData, m2D, gen->nRuns, (*handleForData)->nPoints)), prefs, "R_SIM", NULL, out->numericFormat);

#ifndef MATLAB_MEX_FILE
	if(!writeCommandsSpecified) {

		if(out->doParams) {
			if(!gen->gotParams) m_setoutput(out->params.est, "-", "w", "PA_EST");
			m_setoutput(out->thresholds.est, "-", "w", "TH_EST");
			m_setoutput(out->slopes.est, "-", "w", "SL_EST");
			if(gen->nRuns) {
				m_setoutput(out->thresholds.cpe, "-", "w", "TH_CPE");
				m_setoutput(out->slopes.cpe, "-", "w", "SL_CPE");
				if(bcaPossible) {
					m_setoutput(out->thresholds.bc, "-", "w", "TH_BC");
					m_setoutput(out->slopes.bc, "-", "w", "SL_BC");
					m_setoutput(out->thresholds.acc, "-", "w", "TH_ACC");
					m_setoutput(out->slopes.acc, "-", "w", "SL_ACC");
					m_setoutput(out->thresholds.lims, "-", "w", "TH_LIMS");
					m_setoutput(out->slopes.lims, "-", "w", "SL_LIMS");
				}
				else {
					m_setoutput(out->thresholds.quant, "-", "w", "TH_QUANT");
					m_setoutput(out->slopes.quant, "-", "w", "SL_QUANT");
				}
			}
		}

		if(out->doStats) {
			m_setoutput(out->stats.est, "-", "w", "ST_EST");
			if(gen->nRuns) m_setoutput(out->stats.cpe, "-", "w", "ST_CPE");
		}
	}
#endif /* MATLAB MEX_FILE */
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
BatchPtr LoadPrefs(char *fileName, char *localString, size_t localLength, boolean disposeable)
{
	BatchPtr dataPrefix, prefs = NULL;
	char dataPrefixString[10] = "#data\n";

	if(fileName && localString) Bug("LoadPrefs(): cannot specify read from file and memory simultaneously");
	if(fileName) prefs = LoadBatchFromFile(fileName, TRUE);
	if(localString) prefs = BatchString(localString, localLength, disposeable);
	if(prefs != NULL && !IsBatchFormat(prefs->buffer)) {
		dataPrefix = BatchString(dataPrefixString, strlen(dataPrefixString), FALSE);
		prefs = ConcatenateBatchStrings(dataPrefix, prefs, TRUE, TRUE);
	}
	return prefs;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
PsychDistribFuncPtr MatchShape(char *buf, char *desc)
{
#define kNumberOfShapes 5
	PsychDistribFuncPtr matched = NULL, possible[kNumberOfShapes] =
		{JCumulativeGaussian, JGumbel, JLogistic, JWeibull, JLinear}; /* if adding or removing, remember to alter kNumberOfShapes */
	unsigned short i, totalLength = 0;
	char *errMsg, tryMatch[32], *tempBuf, *s, joiner[] = "\n\t";

	tempBuf = CopyVals(NULL, buf, strlen(buf)+1, sizeof(char));
	for(s = tempBuf; *s; s++) *s = toupper(*s);
	for(i = 0; i < kNumberOfShapes; i++) {
		strcpy(tryMatch, FunctionName(possible[i]));
		totalLength += strlen(tryMatch) + strlen(joiner);
		for(s = tryMatch; *s; s++) *s = toupper(*s);
		if(strncmp(tempBuf, tryMatch, strlen(tempBuf)) == 0) {matched = possible[i]; break;}
	}
	Destroy(tempBuf);
	if(matched == NULL) {
		errMsg = New(char, totalLength + strlen(buf) + strlen(desc) + 64);
		sprintf(errMsg, "Unknown %s \"%s\" - recognized values are:", desc, buf);
		for(i = 0; i < kNumberOfShapes; i++) sprintf(errMsg + strlen(errMsg), "%s%s", joiner, FunctionName(possible[i]));
		JError("%s", errMsg);
		Destroy(errMsg);
	}
	return matched;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

#endif /* __PREFS_C__ */
