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
#ifndef __PSIGNIFIT_C__
#define __PSIGNIFIT_C__

#include "universalprefix.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "psignifit.h"
#include "adaptiveinterface.h"

#define kMagicBetaLimitParameter1	2.0		/* maximum gradient in guess algorithm = this parameter * 1/minimum x step:  should be > 1.*/
#define kMagicBetaLimitParameter2	0.1		/* minimum gradient in guess algorithm = this parameter / (max x - min x): should be < 0.5 */

#if 0
#include "approx.c"
#else
#define APPROX_1	0
#define APPROX_2	0
#define APPROX_3	0
#endif

#if 0
#include "descent.c"
#else
#define REFINE 0
#endif

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

DataSetPtr gMLMT_data;
ModelPtr gMLMT_model;
boolean gMLMT_paramsConverted;
double gPsychSimplexSizes[kNumberOfParams];

char gErrorContext[128];

/* from fitprefs.c : */
	extern unsigned short gMeshResolution, gMeshIterations;
	extern double gEstimateGamma, gEstimateLambda;

	extern boolean gLogSlopes, gCutPsi;
	extern DataFormat gDataFormat;
	extern boolean gDoBootstrapT;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void AllocateDataSet(DataSetPtr data, short nPoints)
{
	data->nPoints = nPoints;
	data->x = (nPoints ? New(double, nPoints) : NULL);
	data->nRight = (nPoints ? New(double, nPoints) : NULL);
	data->nWrong = (nPoints ? New(double, nPoints) : NULL);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void AllocateMatrixBundle(MatrixBundle *bndl, boolean doBCA)
{
	m_allocate(bndl->sim);
	if(doBCA) {
		m_allocate(bndl->deriv);
		m_allocate(bndl->lff);
		m_allocate(bndl->bc);
		m_allocate(bndl->acc);
		m_allocate(bndl->lims);
	}
	m_allocate(bndl->quant);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void BCATerms(MatrixBundle *bndl, matrix ldot)
{
	matrix ldot_lf;
	double R, biasCount, mean, meanOfCubes, variance, val;
	boolean variation;

	ldot_lf = m_mult(mNewMatrix, ldot, bndl->lff);
	m_allocate(bndl->bc);
	m_allocate(bndl->acc);
	R = (double)m_getsize(ldot_lf, 1);
	if(m_first(ldot_lf) && m_first(bndl->est) && m_first(bndl->sim)) {
		m_first(bndl->bc);
		m_first(bndl->acc);
		do {
			biasCount = 0.0;
			variation = FALSE;
			do {
				if(!variation && fabs(m_val(bndl->sim) - m_val(bndl->est)) > m_val(bndl->est) * 1e-12) variation = TRUE;
				if(m_val(bndl->sim) <= m_val(bndl->est)) biasCount++;
			} while(m_step(bndl->sim, 1, 1));
			m_val(bndl->bc) = (variation ? cg_inv(biasCount / (R + 1.0)) : 0.0);

			mean = meanOfCubes = variance = 0.0;
			do mean += m_val(ldot_lf); while(m_step(ldot_lf, 1, 1));
			mean /= R;
			do {
				val = m_val(ldot_lf);
				meanOfCubes += val * val * val;
				val -= mean;
				variance += val * val;
			} while(m_step(ldot_lf, 1, 1));
			meanOfCubes /= R;
			variance /= R - 1.0;
			m_val(bndl->acc) = (variation ? meanOfCubes / (6.0 * pow(variance, 1.5)) : 0.0);

			m_step(bndl->est, 2, 1);
			m_step(bndl->sim, 2, 1);
			m_step(bndl->bc, 2, 1);
			m_step(bndl->acc, 2, 1);
		} while(m_step(ldot_lf, 2, 1));
	}
	m_free(ldot_lf);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void BootstrapT(ModelPtr model, double *params, DataSetPtr data, OutputPtr out, unsigned short rowIndex)
{
	matrix pfish, pcov, tDeriv, sDeriv;

	pfish = ExpectedFisher(mNewMatrix, model->shape, params, data, model);
	pcov = m_inverse(mNewMatrix, pfish);
	tDeriv = m_new(mNewData, m2D, kNumberOfParams, out->nCuts);
	sDeriv = m_new(mNewData, m2D, kNumberOfParams, out->nCuts);

	Derivs(tDeriv, sDeriv, model, model->shape, params, out->nCuts, out->cuts);
	VarianceEstimates(&out->params, rowIndex, pfish, pcov, NULL);
	VarianceEstimates(&out->thresholds, rowIndex, pfish, pcov, tDeriv);
	VarianceEstimates(&out->slopes, rowIndex, pfish, pcov, sDeriv);

	m_free(sDeriv);
	m_free(tDeriv);
	m_free(pcov);
	m_free(pfish);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double CalculateDeviance(DataSetPtr data, double *expected)
{
	double deviance, ourVal, satVal;
	int i;

	deviance = 0.0;
	for(i = 0; i < data->nPoints; i++) {
		ourVal = expected[i];
		satVal = data->nRight[i] / (data->nRight[i] + data->nWrong[i]);

		ourVal = xlogy_j(data->nRight[i], ourVal) + xlogy_j(data->nWrong[i], 1.0 - ourVal);
		satVal = xlogy_j(data->nRight[i], satVal) + xlogy_j(data->nWrong[i], 1.0 - satVal);
		deviance += 2 * (satVal - ourVal);
	}
	return deviance;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void CheckModel(ModelPtr model, boolean checkFreeParams)
{
	unsigned short i;
	ParamPtr p = model->theta;

	if(model->shape == NULL) Bug("CheckModel() called with model->shape == NULL");
	if(model->tailConstraint.prior) {
		if(model->shape == JWeibull) {
			if(model->xValAtChance != 0.0) JError("cannot apply MAX_TAIL_DRIFT constraint with X_AT_CHANCE = %lg\nF(0) is always 0 for the Weibull function", model->xValAtChance);
			SetPrior(&model->tailConstraint, NULL, 0, NULL);
		}
		else if(!legal_x(model->shape, model->xValAtChance)) JError("X_AT_CHANCE value (%lg) is illegal for the %s function", model->xValAtChance, FunctionName(model->shape));
	}
	for(i = 0; i < kNumberOfParams; i++) {
		if(!p[i].free) SetPrior(&p[i].constraint, NULL, 0, NULL);
		if(!checkFreeParams && p[i].free) continue;
		if((i == ALPHA && !legal_alpha(model->shape, p[i].guess)) || (i == BETA && !legal_beta(model->shape, p[i].guess)))
			JError("%s%s value %s = %lg is illegal for the %s function", gErrorContext, (p[i].free ? "start" : "fixed"), p[i].name, p[i].guess, FunctionName(model->shape));
		if((i == GAMMA || i == LAMBDA) && (p[i].guess < 0.0 || p[i].guess >= 1.0))
/*(*/		JError("%s%s value %s = %lg is illegal: must be in range [0, 1)", gErrorContext, (p[i].free ? "start" : "fixed"), p[i].name, p[i].guess);
/*]*/	if(prior(1.0, &p[i].constraint, p[i].guess) == 0.0)
			JError("%s%s value %s = %lg is disallowed by the user-specified Bayesian constraint", gErrorContext, (p[i].free ? "start" : "fixed"), p[i].name, p[i].guess);
	}
	if((checkFreeParams || (!p[GAMMA].free && !p[LAMBDA].free)) && p[GAMMA].guess + p[LAMBDA].guess >= 1.0)
		JError("%sstart values for %s and %s are illegal: %lg + %lg >= 1.0", gErrorContext, p[GAMMA].name, p[LAMBDA].name, p[GAMMA].guess, p[LAMBDA].guess);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean CheckParams(PsychDistribFuncPtr shape, double *params, char *errFmt, ...)
{
	char contextStr[128] = "", errStr[128] = "";
	double temp;
	va_list ap;
	boolean good = TRUE;

	if(!legal_alpha(shape, (temp = params[ALPHA])))
		{good = FALSE; if(errFmt) sprintf(errStr, "alpha = %lg is illegal for the %s function", temp, FunctionName(shape));}
	else if(!legal_beta(shape, (temp = params[BETA])))
		{good = FALSE; if(errFmt) sprintf(errStr, "beta = %lg is illegal for the %s function", temp, FunctionName(shape));}
	else if((temp = params[GAMMA]) < 0.0 || temp >= 1.0)
/*(*/	{good = FALSE; if(errFmt) sprintf(errStr, "gamma = %lg is outside the permissable range [0, 1)", temp);}
/*]*/
	else if((temp = params[LAMBDA]) < 0.0 || temp >= 1.0)
/*(*/	{good = FALSE; if(errFmt) sprintf(errStr, "lambda = %lg is outside the permissable range [0, 1)", temp);}
/*]*/
	else if((temp = params[GAMMA] + params[LAMBDA]) >= 1.0)
		{good = FALSE; if(errFmt) sprintf(errStr, "illegal value gamma + lambda = %lg (must be < 1)", temp);}

	if(*errStr) {
		va_start(ap, errFmt);
		vsprintf(contextStr, errFmt, ap);
		va_end(ap);
		if(*contextStr) sprintf(contextStr + strlen(contextStr), ": ");
		JError("%s%s", contextStr, errStr);
	}

	return good;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void CollateData(DataSetPtr dest, DataSetPtr src, double * alsoCollate)
/* dest data set should be unallocated (unless same as source, in which case it is automatically disposed and re-allocated)*/
{
	unsigned short newPoints, i;
	DataSet tempDest, tempSrc;
	double lastX;

	if(isnan(src->x[0])) {
		if(dest != src) DuplicateDataSet(dest, src);
		return;
	}

	AllocateDataSet(&tempDest, src->nPoints);
	DuplicateDataSet(&tempSrc, src);
	SortDoubles((alsoCollate ? 4 : 3), tempSrc.nPoints, tempSrc.x, tempSrc.nRight, tempSrc.nWrong, alsoCollate);

	newPoints = 0;
	for(i = 0; i < tempSrc.nPoints; i++) {
		if(newPoints == 0 || lastX != tempSrc.x[i]) {
			lastX = tempDest.x[newPoints] = tempSrc.x[i];
			tempDest.nRight[newPoints] = tempSrc.nRight[i];
			tempDest.nWrong[newPoints] = tempSrc.nWrong[i];
			if(alsoCollate) alsoCollate[newPoints] = alsoCollate[i] * (tempSrc.nRight[i] + tempSrc.nWrong[i]);
			newPoints++;
		}
		else {
			tempDest.nRight[newPoints-1] += tempSrc.nRight[i];
			tempDest.nWrong[newPoints-1] += tempSrc.nWrong[i];
			if(alsoCollate) alsoCollate[newPoints-1] += alsoCollate[i] * (tempSrc.nRight[i] + tempSrc.nWrong[i]);
		}
	}
	tempDest.nPoints = newPoints;
	if(dest == src) DisposeDataSet(dest);
	DuplicateDataSet(dest, &tempDest);
	DisposeDataSet(&tempDest);
	DisposeDataSet(&tempSrc);

	if(alsoCollate) for(i = 0; i < newPoints; i++) alsoCollate[i] /= (dest->nRight[i] + dest->nWrong[i]);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int Core(DataSetPtr data, ModelPtr model, GeneratingInfoPtr gen, OutputPtr out)
{
	int i, genSource;
	boolean initialFitRequired, bootstrap, simFitsRequired;
	boolean functionUsedToGenerate, doBCA, gotX, gotY;
	clock_t t;
	double *predicted = NULL;
	enum {useFittedParams, useUserParams, useUserProbabilities};

	if(gen->nRuns) gBugRef = gen->randomSeed;
	CheckModel(model, FALSE);

	initialFitRequired =  gen->nRuns == 0;
	bootstrap = gen->nRuns > 0 && model->shape == gen->shape && !gen->gotParams && gen->psi == NULL;

	if(gen->psi) {
		if(gen->nPoints != data->nPoints) JError("number of generating values must match number of stimulus values");
		for(i = 0; i < gen->nPoints; i++)
			if(isnan(gen->psi[i]) || gen->psi[i] < 0.0 || gen->psi[i] > 1.0)
				JError("generating values must be between 0 and 1");
	}
	genSource = ((bootstrap || gen->nRuns == 0) ? useFittedParams : (gen->psi == NULL) ? useUserParams : useUserProbabilities);
	if(genSource == useFittedParams) { /* either we are doing a clean bootstrap, or nRuns must be 0, so we can ignore any differently supplied GEN_SHAPE, GEN_PARAMS or GEN_VALUES */
		gen->shape = model->shape;
		gen->gotParams = FALSE;
		if(gen->psi) Destroy(gen->psi);
		gen->psi = NULL;
	}
	if(gen->gotParams) CheckParams(gen->shape, gen->params, "error in generating parameters");

	for(gotY = FALSE, i = 0; i < data->nPoints; i++)
		if(data->nRight[i] > 0.0) {gotY = TRUE; break;}
	gotX = (data->x != NULL && !isnan(data->x[0]));

	initialFitRequired |= bootstrap || (out->doStats && genSource == useFittedParams);
	simFitsRequired = (gen->nRuns > 0 && out->doParams);
	functionUsedToGenerate = (gen->nRuns > 0 && (genSource == useUserParams || genSource == useFittedParams));
	doBCA = (out->doParams && functionUsedToGenerate && model->shape == gen->shape && gotX);

    if(initialFitRequired && !gotY) {
        JError("cannot perform initial fit: no responses were supplied in the data set");
        return 0;
    }
	if((gAdaptPtr == NULL && simFitsRequired) || initialFitRequired)  {
        if(!gotX) {
            JError("cannot perform fits: no stimulus values were supplied in the data set");
            return 0;
        }
		for(i = 0; i < data->nPoints; i++)
            if(!legal_x(model->shape, data->x[i])) {
                JError("x = %lg is illegal for use with the %s fitting function", data->x[i], FunctionName(model->shape));
                return 0;
            }
	}
	if(gAdaptPtr == NULL && functionUsedToGenerate) {
        if(!gotX) {
            JError("cannot generate data sets from parameters: no stimulus values supplied");
            return 0;
        }
		for(i = 0; i < data->nPoints; i++)
            if(!legal_x(gen->shape, data->x[i])) {
                JError("x = %lg is illegal for use with the %s generating function", data->x[i], FunctionName(gen->shape));
                return 0;
            }
	}

/*	If we already have generating parameters and are not performing initial fit,
	it doesn't hurt to output the generating parameters as PA_EST, provided the
	fitting and generating function shapes match:
*/	if(!initialFitRequired && out->params.est->output != NULL && gen->shape == model->shape && gen->gotParams) {
		m_allocate(out->params.est);
		if(m_first(out->params.est))
			for(i = 0; i < kNumberOfParams; i++, m_next(out->params.est))
				m_val(out->params.est) = gen->params[i];
	}

	if(out->verbose && (initialFitRequired || simFitsRequired)) ReportModel(model);

	if(initialFitRequired) {
//		sprintf(gErrorContext, "");
		if(m_mass(out->params.est) == 0) {
			m_setsize(out->params.est, 1, kNumberOfParams);
			m_setsteps(out->params.est, 1, 1);
		}
		if(!m_first(m_allocate(out->params.est))) Bug("Core(): failed to allocate matrix for estimated parameters");
        if (FitPsychometricFunction(data, model, out->params.est->vals, out->verbose) == -1)
            return 0;
		if(!gen->gotParams) {
			if(gen->shape != model->shape) Bug("Core(): generating shape has been specified separately but parameters were not supplied");
			for(i = 0; i < kNumberOfParams; i++)
                if(isnan(gen->params[i] = model->theta[i].fitted)) {
					JError("initial fit failed to converge");
                    return 0;
                }
			gen->gotParams = TRUE;
		}
	}
/*	&& genSource != useUserProbabilities used to be an additional condition in the next line */
	if(out->doParams && gen->gotParams && out->nCuts > 0) {
		if(!m_first(m_allocate(out->thresholds.est))) Bug("Core(): failed to allocate matrix for estimated thresholds");
		if(!m_first(m_allocate(out->slopes.est))) Bug("Core(): failed to allocate matrix for estimated slopes");
                for(i = 0; i < (int)  out->nCuts; i++) ThresholdAndSlope(gen->shape, gen->params, out->cuts[i], m_addr(out->thresholds.est, 2, i), m_addr(out->slopes.est, 2, i), NONE);
	}

	if(out->doStats && gotY) {
		if(genSource == useUserProbabilities) predicted = CopyVals(NULL, gen->psi, data->nPoints, sizeof(double));
		else {
			if(gen->shape == NULL || !gen->gotParams) Bug("Core(): generating shape/params unspecified");
			predicted = ExpectedValues(NULL, data->nPoints, data->x, gen->shape, gen->params, "GEN_PARAMS");
		}
		if(m_first(m_allocate(out->stats.est))) {
			DoStats(predicted, data, NULL,
					m_addr(out->stats.est, 2, 0),
					m_addr(out->stats.est, 2, 1),
					m_addr(out->stats.est, 2, 2),
					NULL, NULL);
		}
		if(out->verbose && genSource == useFittedParams && gen->nRuns == 0)
			fprintf(stderr, "Stats for estimated parameters:\n    D = %lg,  r_pd = %lg, r_kd = %lg\n", out->stats.est->vals[0], out->stats.est->vals[1], out->stats.est->vals[2]);
	}
	if(gen->nRuns) {
		if(out->verbose) {
                        if(genSource == useFittedParams) fprintf(stderr, "running %lu simulations using fitted parameters\n", gen->nRuns);
                        else if(genSource == useUserParams) fprintf(stderr, "running %lu simulations using %s(x; {%lg, %lg, %lg, %lg})\n", gen->nRuns, FunctionName(gen->shape), gen->params[ALPHA], gen->params[BETA], gen->params[GAMMA], gen->params[LAMBDA]);
                        else fprintf(stderr, "running %lu simulations using user-supplied generating values\n", gen->nRuns);
			if(out->stats.est->vals) {
				fprintf(stderr, "Stats for generating distribution:\n    D = %lg,  r_pd = %lg, r_kd = %lg\n", out->stats.est->vals[0], out->stats.est->vals[1], out->stats.est->vals[2]);
				if(!out->refit) fprintf(stderr, "NB: simulated stats will use generating distribution (psi_gen) rather\nthan re-fitted parameters (theta*). Degrees of freedom of the process\nused to obtain psi_gen are therefore not taken into account.\n");
			}
			if(gAdaptPtr) CReportAdaptiveProcedure();
                        fprintf(stderr, "random seed: %lu", gen->randomSeed);
			FlushPrintBuffer(FALSE);
		}
		if(out->ySim->output) m_allocate(out->ySim);
		if(out->rSim->output) m_allocate(out->rSim);
		if(out->doStats) m_allocate(out->stats.sim);
		if(out->doParams) {
			AllocateMatrixBundle(&out->params, doBCA);
			if(doBCA) {
				m_allocate(out->fisher);
				ExpectedFisher(out->fisher, gen->shape, gen->params, data, model);
				m_inverse(out->pcov, out->fisher);
				m_allocate(out->ldot);
//				APPROX_1;
			}
			if(out->nCuts > 0) {
				AllocateMatrixBundle(&out->thresholds, doBCA);
				AllocateMatrixBundle(&out->slopes, doBCA);
				if(doBCA) {
					m_identity(out->params.deriv, kNumberOfParams);
					Derivs(out->thresholds.deriv, out->slopes.deriv, model, gen->shape, gen->params, out->nCuts, out->cuts);
					m_normalize(m_copy(out->params.lff, out->pcov), 1);
					m_normalize(m_mult(out->thresholds.lff, out->pcov, out->thresholds.deriv), 1);
					m_normalize(m_mult(out->slopes.lff, out->pcov, out->slopes.deriv), 1);
//					APPROX_2;
					if(gDoBootstrapT) BootstrapT(model, out->params.est->vals, data, out, 0);
				}
			}
		}
		t = clock();
		MonteCarlo(data, model, gen, out);
		if(out->verbose) fprintf(stderr, "%.2lg seconds.\n", (double)(clock()-t)/(double)CLOCKS_PER_SEC);
	}
	if(out->verbose) FlushPrintBuffer(FALSE);
	if(out->doParams && genSource != useUserProbabilities && gen->shape == model->shape) /* && gotY && gotX) */
		FindSensParams(out->sensParams, out->inRegion, out->params.sim, out->sensNPoints, out->sensCoverage, data, model, gen);
	else {
		m_setsize(out->sensParams, m2D, 0, 0);
		m_setsize(out->inRegion, m2D, 0, 0);
	}
	if(doBCA) {
		BCATerms(&out->params, out->ldot);
		BCATerms(&out->thresholds, out->ldot);
		BCATerms(&out->slopes, out->ldot);
	}
	if(out->nConf > 0) {
		Limits(&out->thresholds, out->conf, out->nConf);
		Limits(&out->slopes, out->conf, out->nConf);
		Limits(&out->params, out->conf, out->nConf);
	}
	CPE(out->params.cpe, out->params.est, out->params.sim);
	CPE(out->stats.cpe, out->stats.est, out->stats.sim);
	CPE(out->thresholds.cpe, out->thresholds.est, out->thresholds.sim);
	CPE(out->slopes.cpe, out->slopes.est, out->slopes.sim);

#ifdef MATLAB_MEX_FILE
	out->params.est->refCon = (initialFitRequired ? 1 : 0);
	out->stats.est->refCon = (out->doStats ? 2 : 0);
	out->params.sim->refCon = ((out->doParams && gen->nRuns > 0) ? 3 : 0);
	out->stats.sim->refCon = ((out->doStats && gen->nRuns > 0) ? 4 : 0);
	out->ldot->refCon = ((out->doParams && gen->nRuns > 0) ? 5 : 0);
#endif

	if(predicted) Destroy(predicted);

    return 1;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
short CountFreeParams(ModelPtr model)
{
	short i, nFreeParams;
	for(nFreeParams = 0, i = 0; i < kNumberOfParams; i++) if(model->theta[i].free) nFreeParams++;
	return nFreeParams;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
matrix CPE(matrix cpe, matrix est, matrix sim)
{
	long nSamples, nCols, count;

	if(est == NULL || est->vals == NULL || m_getsize(est, 1) != 1) return NULL;
	if(sim == NULL || sim->vals == NULL || (nSamples = m_getsize(sim, 1)) == 0) return NULL;
	if((nCols = m_getsize(est, 2)) != m_getsize(sim, 2)) Bug("CPE(): mismatched number of columns");

	if(cpe == NULL) cpe = m_new(mNewData, m2D, 1, nCols);
	if(cpe->vals == NULL) m_allocate(cpe);
	if(m_first(cpe) && m_first(est) && m_first(sim)) do {
		count = 0;
		do {
			if(m_val(sim) <= m_val(est)) count ++;
		} while(m_step(sim, 1, 1));
		m_step(sim, 2, 1);
		m_val(cpe) = (double)count / (double)(nSamples + 1);
	} while(m_next(cpe) && m_next(est));

	return cpe;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void Derivs(matrix tDeriv, matrix sDeriv, ModelPtr model, PsychDistribFuncPtr shape, double *params, unsigned short nCuts, double *cuts)
{
	unsigned short i, j;
	double *tAddr, *sAddr;

	for(j = 0; j < nCuts; j++) {
		m_setpoint(tDeriv, 0, j);
		m_setpoint(sDeriv, 0, j);
		for(i = 0; i < kNumberOfParams; i++) {
			tAddr = m_addr(tDeriv, 1, i);
			sAddr = m_addr(sDeriv, 1, i);
			if(model->theta[i].free)
				ThresholdAndSlope(shape, params, cuts[j], tAddr, sAddr, (ArgIdentifier)i);
			else *tAddr = *sAddr = 0.0;
		}
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double * DevianceResiduals(double *rBuffer, double *expected, DataSetPtr data, double *deviance)
{
	double r, w, y, p, d;
	int i;

	if(rBuffer == NULL) rBuffer = New(double, data->nPoints);
	if(deviance) *deviance = 0.0;

	for(i = 0; i < data->nPoints; i++) {
		r = data->nRight[i];
		w = data->nWrong[i];
		y = r / (r + w);
		p = expected[i];
		d = 2.0 * (xlogy_j(r, y) - xlogy_j(r, p) + xlogy_j(w, 1.0 - y) - xlogy_j(w, 1.0 - p));
		rBuffer[i] = ((p < y) ? sqrt(d) : -sqrt(d));
		if(deviance) *deviance += d;
	}

	return rBuffer;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double DiffLogAllPriors(ModelPtr model, double *p, ArgIdentifier wrt)
{
	boolean doubleDiff;
	ArgIdentifier wrt1, wrt2;
	double t, dt_dp, dt_du, dt_dv;
	double s, ds_dp, ds_du, ds_dv;
	double result;

	doubleDiff = DoubleDiff(wrt, &wrt1, &wrt2);
	result = 0.0;

	if(!model->theta[wrt1].free || !model->theta[wrt2].free) return 0.0;

	if(!CheckParams(model->shape, p, NULL)) return NAN;

	/* NB if any priors are added, removed or changed, the procedure Priors() must also be adjusted */

	/* parameter prior */
	if(model->theta[wrt1].constraint.prior) {
		if(!doubleDiff) result += DiffLogPrior(p[wrt], 1.0, &model->theta[wrt].constraint);
		else if(wrt1 == wrt2) result += Diff2LogPrior(p[wrt1], 1.0, 1.0, 0.0, &model->theta[wrt1].constraint);
	}
	/* "tail drift" prior */
	if(model->tailConstraint.prior && wrt1 != GAMMA && wrt2 != GAMMA && wrt1 != LAMBDA && wrt2 != LAMBDA) {
		t = prob(model->shape, p[ALPHA], p[BETA], model->xValAtChance);
		dt_dp = (*model->shape)(NAN, model->xValAtChance, p[ALPHA], p[BETA], derivative, wrt);
		if(!doubleDiff) result += DiffLogPrior(t, dt_dp, &model->tailConstraint);
		else {
			dt_du = (*model->shape)(NAN, model->xValAtChance, p[ALPHA], p[BETA], derivative, wrt1);
			dt_dv = (*model->shape)(NAN, model->xValAtChance, p[ALPHA], p[BETA], derivative, wrt2);
			result += Diff2LogPrior(t, dt_du, dt_dv, dt_dp, &model->tailConstraint);
		}
	}
	/* priors on real shifts/slopes */
	if(model->shiftConstraint.prior || model->slopeConstraint.prior) {
		ThresholdAndSlope(model->shape, p, 0.5, &t, &s, NONE);
		ThresholdAndSlope(model->shape, p, 0.5, &dt_dp, &ds_dp, wrt);
		if(!doubleDiff) result +=  DiffLogPrior(t, dt_dp, &model->shiftConstraint)
		                         + DiffLogPrior(s, ds_dp, &model->slopeConstraint);
		else {
			ThresholdAndSlope(model->shape, p, 0.5, &dt_du, &ds_du, wrt1);
			ThresholdAndSlope(model->shape, p, 0.5, &dt_dv, &ds_dv, wrt2);
			result +=   Diff2LogPrior(t, dt_du, dt_dv, dt_dp, &model->shiftConstraint)
			          + Diff2LogPrior(s, ds_du, ds_dv, ds_dp, &model->slopeConstraint);
		}
	}
	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double DiffLoglikelihood(PsychDistribFuncPtr shape, double *params, ArgIdentifier wrt, DataSetPtr data, ModelPtr model)
{
	unsigned short i;
	double r, w, psi, dpsi, result;
	boolean doubleDiff;
	ArgIdentifier wrt1, wrt2;
	double dpsi1, dpsi2, temp;

	result = 0.0;
	doubleDiff = DoubleDiff(wrt, &wrt1, &wrt2);

	for(i = 0; i < data->nPoints; i++) {
		r = data->nRight[i];
		w = data->nWrong[i];
		dpsi = DiffPsi(shape, params, data->x[i], &psi, wrt);
		result += dpsi * (r / psi - w / (1.0 - psi));

		if(doubleDiff) {
			dpsi1 = DiffPsi(shape, params, data->x[i], NULL, wrt1);
			dpsi2 = DiffPsi(shape, params, data->x[i], NULL, wrt2);
			temp = 1.0 - psi;
			/* problems occur when psi = 0 or 1 due to rounding errors */
			result -= dpsi1 * dpsi2 * (r / (psi * psi) + w / (temp * temp));
		}
	}
	if(model && model->shape == shape) result += DiffLogAllPriors(model, params, wrt);
	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double DiffPsi(PsychDistribFuncPtr shape, double *params, double x, double *returnPsi, ArgIdentifier wrt)
{
	double f, scaleF;
	ArgIdentifier wrt1, wrt2;

	f = prob(shape, params[ALPHA], params[BETA], x);
	scaleF = 1.0 - params[GAMMA] - params[LAMBDA];
	if(returnPsi) *returnPsi = params[GAMMA] + scaleF * f;

	if(!DoubleDiff(wrt, &wrt1, &wrt2)) {
		switch(wrt) {
			case ALPHA:
			case BETA: return scaleF * (*shape)(NAN, x, params[ALPHA], params[BETA], derivative, wrt);
			case GAMMA: return 1.0 - f;
			case LAMBDA: return -f;
			default: Bug("DiffPsi(): illegal value for argument 'wrt'");
		}
	}
	else {
		if(wrt2 == GAMMA || wrt2 == LAMBDA) /* DoubleDiff() returns the identifiers in order, thus if wrt2 is not GAMMA or LAMBDA, then neither is wrt1 */
			return ((wrt1 == GAMMA || wrt1 == LAMBDA) ? 0.0 : -(*shape)(NAN, x, params[ALPHA], params[BETA], derivative, wrt1));
		else return scaleF * (*shape)(NAN, x, params[ALPHA], params[BETA], derivative, wrt);
	}

	return NAN;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void DisposeDataSet(DataSetPtr data)
{
	if(data == NULL) return;
	data->nPoints = 0;
	if(data->x) Destroy(data->x); data->x = NULL;
	if(data->nRight) Destroy(data->nRight); data->nRight = NULL;
	if(data->nWrong) Destroy(data->nWrong); data->nWrong = NULL;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void DoStats(double *predicted, DataSetPtr data, double *chronIndex,
			 double *returnDeviance, double *returnPRCorr, double *returnKRCorr,
			 double *rSpace, double *kSpace)
{
	double *k, *r;
	boolean dispose_r, dispose_k;
	int i, nPoints;

	if(data == NULL || data->nPoints == 0 || predicted == NULL) return;
	nPoints = data->nPoints;
	dispose_r = (rSpace == NULL);
	dispose_k = (kSpace == NULL);
	r = DevianceResiduals(rSpace, predicted, data, returnDeviance);
	k = (kSpace ? kSpace : New(double, nPoints));
	if(returnPRCorr) *returnPRCorr = CorrelationCoefficient(nPoints, predicted, r);
	if(returnKRCorr) {
                for(i = 0; i < (int) data->nPoints; i++) {
                        k[chronIndex ? (size_t)(chronIndex[i]) : (unsigned) i] =
				((data->nRight[i] > 0.0 && data->nWrong[i] > 0.0) ? r[i] : NAN);
		} /* residuals are now temporarily in k, but they are sorted, with NANs in the cells we're going to miss out */
		for(nPoints = 0, i = 0; i < data->nPoints; i++) {
			if(!isnan(k[i])) {
				r[nPoints] = k[i];
				k[nPoints] = (double)(nPoints + 1);
				nPoints++;
			}
		} /* residuals are now back in r, with the NANs squeezed out, and k contains consecutive positive integers */
		*returnKRCorr = CorrelationCoefficient(nPoints, k, r);
	}

	if(dispose_k) Destroy(k);
	if(dispose_r) Destroy(r);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void DuplicateDataSet(DataSetPtr dest, DataSetPtr src)
/* dest should be unallocated */
{
	short i;

	AllocateDataSet(dest, src->nPoints);
	for(i = 0; i < src->nPoints; i++) {
		dest->x[i] = src->x[i];
		dest->nRight[i] = src->nRight[i];
		dest->nWrong[i] = src->nWrong[i];
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
matrix ExpectedFisher(matrix m, PsychDistribFuncPtr shape, double *params, DataSetPtr data, ModelPtr model)
{
	DataSet expected;
	unsigned short i, j;
	double n, psi, scaleF, d2;
	boolean useModel;
	ArgIdentifier wrt;

	useModel = (model != NULL && model->shape == shape);
	if(m == NULL) m = m_new(mNewData, m2D, kNumberOfParams, kNumberOfParams);
	if(m_getsize(m, 1) != kNumberOfParams || m_getsize(m, 2) != kNumberOfParams || !m_first(m)) Bug("ExpectedFisher(): matrix is not ready or has wrong number of dimensions");
	if(kNumberOfParams == 1) {*m->vals = 1.0; return m;}

	DuplicateDataSet(&expected, data);
	scaleF = 1.0 - params[GAMMA] - params[LAMBDA];
	for(i = 0; i < data->nPoints; i++) {
		psi = params[GAMMA] + scaleF * prob(shape, params[ALPHA], params[BETA], data->x[i]);
		n = data->nRight[i] + data->nWrong[i];
		expected.nWrong[i] = n - (expected.nRight[i] = n * psi);
	}
	for(i = 0; i < kNumberOfParams; i++) {
		for(j = 0; j < kNumberOfParams; j++) {
			if(!useModel || (model->theta[i].free && model->theta[j].free)) {
				wrt = wrt_both(i, j);
				d2 = DiffLoglikelihood(shape, params, wrt, &expected, model);
				m_val(m) = -d2;
			}
			else m_val(m) = ((i == j) ? 1.0 : 0.0);
			m_next(m);
		}
	}
	DisposeDataSet(&expected);
	return m;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double * ExpectedValues(double *expected, unsigned short nPoints, double *x,
				PsychDistribFuncPtr shape, double *params, char * description)
{
	int i;
	double scaleF;

	CheckParams(shape, params, "error in %s", description);
	if(expected == NULL) expected = New(double, nPoints);
	scaleF = 1.0 - params[GAMMA] - params[LAMBDA];
	for(i = 0; i < nPoints; i++) expected[i] = params[GAMMA] + scaleF * prob(shape, params[ALPHA], params[BETA], x[i]);
	return expected;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void ExportDataSet(DataSetPtr data, matrix m, double * chronIndex)
{
	double x, r, w, n, y;
	unsigned short i, index;
	m_setsize(m, 2, data->nPoints, 3);
	m_defaultpacking(m);
	m_allocate(m);
	if(!m_first(m)) return;
	for(i = 0; i < data->nPoints; i++) {
		index = (chronIndex ? (unsigned short)(chronIndex[i]) : i);
		m_setpoint(m, index, 0);
		x = data->x[i];
		r = data->nRight[i];
		w = data->nWrong[i];
		y = r / (n = r + w);
		m_val(m) = x;
		m_step(m, 2, 1);
		m_val(m) = ((gDataFormat == xrn || gDataFormat == xrw) ? r : y);
		m_step(m, 2, 1);
		m_val(m) = ((gDataFormat == xrw) ? w : n);
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void FakeFit(ModelPtr model, double *paramsOut,
				unsigned short nPoints, double *srcX, double *srcPsi,
				PsychDistribFuncPtr srcShape, double *srcParams)
{
	DataSetPtr oldDataSetPtr;
	ModelPtr oldModelPtr;
	boolean oldConversionFlag;

	boolean useFunc;
	DataSet tempData;
	Model tempModel;

	double begin, end, perf;
	unsigned short i;

	if(nPoints < 2) Bug("FakeFit(): at least 2 points required");
	useFunc = (srcShape != NULL && srcParams != NULL);
	if(useFunc && srcPsi != NULL) Bug("FakeFit(): must fit using psi values, or function parameters, not both");
	if(!useFunc && srcPsi == NULL) Bug("FakeFit(): need psi");

	tempModel = *model;
	AllocateDataSet(&tempData, nPoints);
	if(srcX) CopyVals(tempData.x, srcX, nPoints, sizeof(double));
	else {
		if(!useFunc) Bug("FakeFit(): need x");
		begin = inv_prob(srcShape, srcParams[ALPHA], srcParams[BETA], 0.01);
		end = inv_prob(srcShape, srcParams[ALPHA], srcParams[BETA], 0.99);
		get_limits(tempModel.shape, X);
		if(begin < gLegal.min) begin = gLegal.min; if(begin > gLegal.max) begin = gLegal.max;
		if(end < gLegal.min) end = gLegal.min; if(end > gLegal.max) end = gLegal.max;
		get_limits(tempModel.shape, ALPHA);
		if(begin < gLegal.min) begin = gLegal.min; if(begin > gLegal.max) begin = gLegal.max;
		if(end < gLegal.min) end = gLegal.min; if(end > gLegal.max) end = gLegal.max;
		if(begin == end) JError("%s function cannot approximate generating distribution", FunctionName(tempModel.shape));
		for(i = 0; i < nPoints; i++) tempData.x[i] = begin + (end - begin) * (double)i / (double)(nPoints - 1);
	}
	for(i = 0; i < nPoints; i++) {
		perf = (srcPsi ? srcPsi[i] : srcParams[GAMMA] + (1.0 - srcParams[GAMMA] - srcParams[LAMBDA]) * prob(srcShape, srcParams[ALPHA], srcParams[BETA], tempData.x[i]));
		tempData.nRight[i] = floor(0.5 + 1000.0 * perf);
		tempData.nWrong[i] = 1000.0 - tempData.nRight[i];
	}
	get_mlmt_info(&oldDataSetPtr, &oldModelPtr, &oldConversionFlag);
	FitPsychometricFunction(&tempData, &tempModel, paramsOut, FALSE);
	set_mlmt_info(oldDataSetPtr, oldModelPtr, oldConversionFlag);
	DisposeDataSet(&tempData);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void FindSensParams(matrix sensParams, matrix insideMatrix, matrix pSim, unsigned short nPoints, double coverage, DataSetPtr data, ModelPtr model, GeneratingInfoPtr gen)
{
	long i, j, iterations, *indices;
	double *inside, *r, *theta, *sorted = NULL;
	double rMax, binCentre, binHalfWidth, binCentreSpacing, diff;
	double contourVal, contourTol, multiplier, lBound, uBound, growthFactor, newScore;
	double alpha, beta;
	double p[kNumberOfParams], simPoint[kNumberOfParams], scaling[kNumberOfParams];
	matrix slice, scores;
	boolean legalParams;
	DataSet fakeData;
	boolean gotX, gotY;

	if(sensParams == NULL || nPoints == 0) return;
	if(gen->shape != model->shape || gen->psi != NULL || !gen->gotParams || gen->nRuns == 0) return;
        if(!m_first(pSim) || m_getsize(pSim, 1) != (int) gen->nRuns || m_getsize(pSim, 2) != kNumberOfParams) Bug("FindSensParams(): pSim is invalid or has incorrect shape");

	gotX = gotY = FALSE;
	for(i = 0; i < data->nPoints; i++) {
		if(!isnan(data->x[i])) gotX = TRUE;
		if(data->nRight[i] > 0.0) gotY = TRUE;
	}

	if(!gotX || !gotY) {
		if(gotX) {
			fakeData.nPoints = data->nPoints;
			fakeData.x = CopyVals(NULL, data->x, data->nPoints, sizeof(double));
		}
		else {
			fakeData.nPoints = 9;
			fakeData.x = New(double, fakeData.nPoints);
			for(i = 0; i < fakeData.nPoints; i++)
				fakeData.x[i] = inv_prob(gen->shape, gen->params[ALPHA], gen->params[BETA], (double)(i + 1) / (double)(fakeData.nPoints + 1));
		}
		fakeData.nRight = ExpectedValues(NULL, fakeData.nPoints, fakeData.x, gen->shape, gen->params, "generating params for fake data (internal error)");
		fakeData.nWrong = New(double, fakeData.nPoints);
		for(i = 0; i < fakeData.nPoints; i++) {
			fakeData.nRight[i] = floor(0.5 + 1000.0 * fakeData.nRight[i]);
			fakeData.nWrong[i] = 1000.0 - fakeData.nRight[i];
		}
		data = &fakeData;
	}

	m_setsize(insideMatrix, m1D, gen->nRuns); m_first(insideMatrix);
	inside = (insideMatrix ? m_allocate(insideMatrix)->vals : New(double, gen->nRuns));


	m_first(pSim); slice = m_slice(pSim, m1D, gen->nRuns);
	for(i = 0; i < kNumberOfParams; i++, m_moveslice(slice, pSim, 2, 1)) {
		sorted = m_sortvals(sorted, slice);
		scaling[i] = Quantile(0.841, sorted, gen->nRuns) - Quantile(0.159, sorted, gen->nRuns);
		if(scaling[i] == 0.0) scaling[i] = 1.0;
	}
	m_free(slice);

	set_mlmt_info(data, model, FALSE);
	scores = m_new(mNewData, m1D, gen->nRuns);
	r = New(double, gen->nRuns); theta = New(double, gen->nRuns);
        for(i = 0; i < (int) gen->nRuns; i++) {
		for(j = 0; j < kNumberOfParams; j++, m_step(pSim, 2, 1))
			p[j] = m_val(pSim);
		m_val(scores) = mlmt(p);
		alpha = (p[ALPHA] - gen->params[ALPHA]) / scaling[ALPHA];
		beta = (p[BETA] - gen->params[BETA]) / scaling[BETA];
		theta[i] = atan2(beta, alpha) * 180.0 / pi;
		r[i] = alpha*alpha + beta*beta;
		m_step(pSim, 1, 1); m_next(scores);
	}

	m_sortvals(sorted, scores);
	contourVal = Quantile(coverage, sorted, gen->nRuns);
	contourTol = (Quantile(0.841, sorted, gen->nRuns) - Quantile(0.159, sorted, gen->nRuns)) / 100.0;
	Destroy(sorted);

	rMax = 0.0; *(indices = New(long, nPoints)) = 0;
        for(m_first(scores), i = 0; i < (int) gen->nRuns; i++, m_next(scores)) {
		inside[i] = ((m_val(scores) <= contourVal) ? 1.0 : 0.0);
		if(inside[i] && r[i] > rMax) rMax = r[indices[0] = i];
	}
	m_free(scores);


	binCentre = theta[indices[0]];
	binHalfWidth = 0.6 * 180.0 / (double)nPoints; /* when the first multiplier is 1.0, points are not kept apart */
	binCentreSpacing = 360.0 / (double)nPoints;
	for(i = 1; i < nPoints; ) {
		binCentre += binCentreSpacing;
		while(binCentre > 180.0) binCentre -= 360.0;
		while(binCentre < -180.0) binCentre += 360.0;
		rMax = 0.0; indices[i] = -1;
                for(j = 0; j < (int) gen->nRuns; j++) {
			if(inside[j]) {
				diff = theta[j] - binCentre;
				while(diff > 180.0) diff -= 360.0;
				while(diff < -180.0) diff += 360.0;
				if(fabs(diff) <= binHalfWidth && r[j] > rMax) rMax = r[indices[i] = j];
			}
		}
		if(indices[i] == -1) nPoints--; else i++;
	}

	m_setsize(sensParams, m2D, nPoints, kNumberOfParams); m_allocate(sensParams);
	for(m_first(sensParams), i = 0; i < nPoints; i++, m_step(sensParams, 1, 1)) {
		m_setpoint(pSim, indices[i], 0);
		for(j = 0; j < kNumberOfParams; j++, m_step(pSim, 2, 1)) simPoint[j] = m_val(pSim);
		uBound = lBound = 1.0;
		growthFactor = 1.0;
		iterations = 0;
		do {
			uBound *= 1.0 + growthFactor;
			for(j = 0; j < kNumberOfParams; j++) p[j] = (1.0 - uBound) * gen->params[j] + uBound * simPoint[j];
			legalParams = CheckParams(model->shape, p, NULL);
			if(!legalParams) {uBound /= 1.0 + growthFactor; growthFactor *= 0.9;}
		} while(iterations++ < 20 && (!legalParams || mlmt(p) < contourVal));
		iterations = 0;
		do {
			multiplier = 0.5 * (lBound + uBound);
			for(j = 0; j < kNumberOfParams; j++) p[j] = (1.0 - multiplier) * gen->params[j] + multiplier * simPoint[j];

			if(p[GAMMA] < 0.0) p[GAMMA] = 0.0;
			if(p[LAMBDA] < 0.0) p[LAMBDA] = 0.0;
			if(CheckParams(model->shape, p, NULL) == FALSE)
				{CopyVals(p, simPoint, kNumberOfParams, sizeof(double)); break;}

			newScore = mlmt(p);
			if(newScore < contourVal) lBound = multiplier;
			if(newScore > contourVal) uBound = multiplier;
		} while(iterations++ < 20 && fabs(newScore - contourVal) > contourTol);
		for(j = 0; j < kNumberOfParams; j++, m_step(sensParams, 2, 1)) m_val(sensParams) = p[j];
	}

	if(data == &fakeData) DisposeDataSet(&fakeData);
	Destroy(indices); Destroy(theta); Destroy(r);
	if(insideMatrix == NULL) Destroy(inside);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
extern boolean gLambdaEqualsGamma;
int FitCore(double *pIn, double *pOut, boolean *pFree)
{
	int iterations, i;
	boolean oldGammaFree;

	oldGammaFree = pFree[GAMMA];
	if(gLambdaEqualsGamma) pFree[GAMMA] = FALSE;

	for(i = 0; i < kNumberOfParams; i++) pOut[i] = pIn[i];
        iterations = SimplexSearch(kNumberOfParams, pOut, pFree, gPsychSimplexSizes, mlmt); // REFINE;
	if(iterations < 0) for(i = 0; i < kNumberOfParams; i++) pOut[i] = NAN;

	pFree[GAMMA] = oldGammaFree;
	if(gLambdaEqualsGamma) pOut[GAMMA] = pOut[LAMBDA];

	return (iterations < 0) ? -1 : 0;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
short FitPsychometricFunction(DataSetPtr data, ModelPtr model, double *paramsOut, boolean verbose)
{
	double *p, localParams[kNumberOfParams];
	boolean pfree[kNumberOfParams];
	short i, err;
	DataSet tempData;

	p = ((paramsOut == NULL) ? localParams : paramsOut);
	CollateData(&tempData, data, NULL);

	err = 0;

    if (! GuessParams(&tempData, model) ) {
        return  -1;
    }
	if(verbose) {
		fprintf(stderr, "fitting to original data\ninitial: {");
		for(i = 0; i < kNumberOfParams; i++) fprintf(stderr, "%s = %lg%s", model->theta[i].name, model->theta[i].guess, (i == kNumberOfParams - 1) ? "}\n" : ", ");
	}
	CheckModel(model, TRUE);
	for(i = 0; i < kNumberOfParams; i++) {
		p[i] = model->theta[i].guess;
		pfree[i] = model->theta[i].free;
	}
	set_mlmt_info(&tempData, model, model->convertParams);
	if(model->convertParams) TranslateAB(model->shape, p, ab2ts);
	PsychSetSimplexSizes(&tempData, model->shape, p, model->convertParams);
	err = FitCore(p, p, pfree);

	if(err == 0 && model->convertParams) TranslateAB(model->shape, p, ts2ab);
	for(i = 0; i < kNumberOfParams; i++) model->theta[i].fitted = p[i];
	DisposeDataSet(&tempData);
	if(verbose) {
		fprintf(stderr, "  final: {");
		for(i = 0; i < kNumberOfParams; i++) fprintf(stderr, "%s = %lg%s", model->theta[i].name, model->theta[i].fitted, (i == kNumberOfParams - 1) ? "}\n" : ", ");
	}
	return err;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void FixParam(Param theta[], short whichParam, double value)
{
	if(whichParam<0 || whichParam > kNumberOfParams-1)
		Bug("FixParam(): parameter number must be from 0 to %d", kNumberOfParams-1);

	theta[whichParam].guess = value;
	theta[whichParam].free = FALSE;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void FreeParam(Param theta[], short whichParam)
{
	if(whichParam<0 || whichParam > kNumberOfParams-1)
		Bug("FreeParam(): parameter number must be from 0 to %d", kNumberOfParams-1);
	theta[whichParam].free = TRUE;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void GenerateFakeDataSet(DataSetPtr data1, double *expected1, double *nObs1,
						 DataSetPtr data2, double *expected2, double *nObs2)
/*	data set spaces should be pre-allocated, with correct x values */
/*	Can generate two data sets at once using the same random numbers: useful for generating
	collated and uncollated versions of the same data set, which will work so long as the
	x values of both data sets have been sorted in the same order, either increasing or decreasing.
	If only one set is required, pass NULL for data2. If data2 and data1 point to the same data
	set, only the expected1/nObs1 generators are used. */
{
	long trial, boundary1, boundary2, carryOn, pt1, pt2;
	double randNumber;

	if(data1 == data2) data2 = NULL;

	boundary1 = boundary2 = 0;
	pt1 = pt2 = -1;
	carryOn = 1;
	for(trial = 0; carryOn; trial++) {
		carryOn = 0;
		randNumber = UniformRandomDouble();
		while(data1 && trial == boundary1 && ++pt1 < data1->nPoints) boundary1 += nObs1[pt1], data1->nRight[pt1] = data1->nWrong[pt1] = 0.0;
		if(data1 && pt1 < data1->nPoints) carryOn = ((randNumber < expected1[pt1]) ? ++data1->nRight[pt1] : ++data1->nWrong[pt1]);
		while(data2 && trial == boundary2 && ++pt2 < data2->nPoints) boundary2 += nObs2[pt2], data2->nRight[pt2] = data2->nWrong[pt2] = 0.0;
		if(data2 && pt2 < data2->nPoints) carryOn = ((randNumber < expected2[pt2]) ? ++data2->nRight[pt2] : ++data2->nWrong[pt2]);
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void get_mlmt_info(DataSetPtr *data, ModelPtr *model, boolean *treatABasTS)
{
	if(data) *data = gMLMT_data;
	if(model) *model = gMLMT_model;
	if(treatABasTS) *treatABasTS = gMLMT_paramsConverted;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int GuessParams(DataSetPtr data, ModelPtr model)
{
	unsigned short i, pNum;
	Model tempModel;
	double minX, maxX, medianX, minXStep;
	double halfwaySlope, halfwayShift, regressedParams[kNumberOfParams];
	double temp, *temp_yVals, *temp_nObs;
	SearchLimits pSearchLims[kNumberOfParams], shiftSearchLims, slopeSearchLims, *lims, linSlopeSearchLims, logSlopeSearchLims;
	double min[kNumberOfParams], max[kNumberOfParams];
	ParamPtr param;
	double gridScore;

	tempModel = *model;
	temp_yVals = New(double, data->nPoints);
	temp_nObs = New(double, data->nPoints);

	minXStep = 0.0;
	if(data->nPoints < 1) JError("%sdata set contains no points!!", gErrorContext);
	for(i = 0; i < data->nPoints; i++) {
		/* general sanity check on data set */
		if(data->nRight[i] > floor(data->nRight[i]) || data->nWrong[i] > floor(data->nWrong[i]))
			JError("%sdata set contains illegal non-integer numbers of correct or incorrect trials", gErrorContext);
		if(data->nRight[i] < 0.0 || data->nWrong[i] < 0.0)
			JError("%sdata set contains illegal negative numbers of correct or incorrect trials", gErrorContext);
		if(data->nRight[i] + data->nWrong[i] == 0.0)
			JError("%sdata set contains points with 0 observations", gErrorContext);

		/* check data are sorted by increasing x, determine minimum step, calculate temporary values for regression */
		temp_yVals[i] = data->nRight[i] / (temp_nObs[i] = data->nRight[i] + data->nWrong[i]);
		temp = ((i==0) ? 0.0 : data->x[i] - data->x[i-1]);
		if(minXStep == 0.0 || (temp > 0.0 && temp < minXStep)) minXStep = temp;
		if(temp < 0.0) Bug("data set must be sorted before being passed to GuessParams()");
	}
	minX = data->x[0];
	maxX = data->x[data->nPoints - 1];
	medianX = ((data->nPoints % 2) ? data->x[(1 + data->nPoints) / 2 - 1] : 0.5 * (data->x[data->nPoints / 2 - 1] + data->x[data->nPoints / 2]));
	if(!legal_x(tempModel.shape, minX) || !legal_x(tempModel.shape, maxX))
		JError("%sreceived illegal x values for use with %s function", gErrorContext, FunctionName(tempModel.shape));

	pNum = LAMBDA;
	lims = pSearchLims + pNum; param = tempModel.theta + pNum;
	if(param->free) {
		lims->min = lims->max = gEstimateLambda;
		MakeLimitsLegal(lims, &param->constraint, 0.0, 1.0);
		FixParam(tempModel.theta, pNum, lims->min);
	}
	else lims->min = lims->max = param->guess;

	pNum = GAMMA;
	lims = pSearchLims + pNum; param = tempModel.theta + pNum;
	if(param->free) {
		lims->min = lims->max = gEstimateGamma;
		MakeLimitsLegal(lims, &param->constraint, 0.0, 1.0);
		FixParam(tempModel.theta, pNum, lims->min);
	}
	else lims->min = lims->max = param->guess;

	regressedParams[GAMMA] = tempModel.theta[GAMMA].guess;
	regressedParams[LAMBDA] = tempModel.theta[LAMBDA].guess;
    if (! TransformedRegression(data->nPoints, data->x, temp_yVals, temp_nObs,
        tempModel.shape, regressedParams+ALPHA, regressedParams+BETA, regressedParams[GAMMA], regressedParams[LAMBDA]) )
        return 0;
	halfwayShift = inv_prob(tempModel.shape, regressedParams[ALPHA], regressedParams[BETA], 0.5);
	halfwaySlope = slope(tempModel.shape, regressedParams[ALPHA], regressedParams[BETA], 0.5);
	Destroy(temp_yVals); Destroy(temp_nObs);

	if(tempModel.tailConstraint.prior != NULL && ((halfwaySlope > 0.0 && minX < tempModel.xValAtChance) || (halfwaySlope < 0.0 && maxX > tempModel.xValAtChance)))
		JError("%stail drift limitation is inappropriate for the data:\nthere are data the wrong side of x_at_chance, judging from their overall gradient", gErrorContext);

	shiftSearchLims.min = minX - (medianX - minX);
	shiftSearchLims.max = maxX + (maxX - medianX);
	get_limits(tempModel.shape, ALPHA);
	MakeLimitsLegal(&shiftSearchLims, &tempModel.shiftConstraint, gLegal.min, gLegal.max);
	if(halfwayShift < shiftSearchLims.min) halfwayShift = shiftSearchLims.min;
	if(halfwayShift > shiftSearchLims.max) halfwayShift = shiftSearchLims.max;

	if(!legal_gradient(tempModel.shape, halfwaySlope))
		JError("%scannot estimate a legal gradient for the %s function:\napparent %s slope", gErrorContext, FunctionName(tempModel.shape), (halfwaySlope > 0.0) ? "positive" : halfwaySlope < 0.0 ? "negative" : "zero");
	temp = ((halfwaySlope < 0.0) ? -1.0 : 1.0);
	linSlopeSearchLims.min = temp * kMagicBetaLimitParameter2 / (maxX-minX);
	linSlopeSearchLims.max = temp * kMagicBetaLimitParameter1 / minXStep;
	get_limits(tempModel.shape, DFDX);
	MakeLimitsLegal(&linSlopeSearchLims, NULL, gLegal.min, gLegal.max);
	if(gLogSlopes) {
		temp = shiftSearchLims.min * linSlopeSearchLims.min * log(10.0);
		logSlopeSearchLims.min = logSlopeSearchLims.max = temp;
		temp = shiftSearchLims.min * linSlopeSearchLims.max * log(10.0);
		if(temp < logSlopeSearchLims.min) logSlopeSearchLims.min = temp;
		if(temp > logSlopeSearchLims.max) logSlopeSearchLims.max = temp;
		temp = shiftSearchLims.max * linSlopeSearchLims.min * log(10.0);
		if(temp < logSlopeSearchLims.min) logSlopeSearchLims.min = temp;
		if(temp > logSlopeSearchLims.max) logSlopeSearchLims.max = temp;
		temp = shiftSearchLims.max * linSlopeSearchLims.max * log(10.0);
		if(temp < logSlopeSearchLims.min) logSlopeSearchLims.min = temp;
		if(temp > logSlopeSearchLims.max) logSlopeSearchLims.max = temp;
		if(shiftSearchLims.min <= 0.0 && shiftSearchLims.max >= 0.0) {
			temp = 0.0;
			if(temp < logSlopeSearchLims.min) logSlopeSearchLims.min = temp;
			if(temp > logSlopeSearchLims.max) logSlopeSearchLims.max = temp;
		}
		slopeSearchLims = logSlopeSearchLims;
		halfwaySlope *= halfwayShift * log(10.0);
	}
	else slopeSearchLims = linSlopeSearchLims;

	MakeLimitsLegal(&slopeSearchLims, &tempModel.slopeConstraint, -INF, INF);
	if(halfwaySlope < slopeSearchLims.min) halfwaySlope = slopeSearchLims.min;
	if(halfwaySlope > slopeSearchLims.max) halfwaySlope = slopeSearchLims.max;

	if(!isnan(tempModel.fixedSlope)) {
		FixParam(tempModel.theta, BETA, tempModel.fixedSlope);
		slopeSearchLims.min = slopeSearchLims.max = tempModel.fixedSlope;
		tempModel.convertParams = TRUE;
	}
	if(!isnan(tempModel.fixedShift)) {
		FixParam(tempModel.theta, ALPHA, tempModel.fixedShift);
		shiftSearchLims.min = shiftSearchLims.max = tempModel.fixedShift;
		tempModel.convertParams = TRUE;
	}
	if(tempModel.convertParams) {
		pSearchLims[ALPHA] = shiftSearchLims;
		pSearchLims[BETA] = slopeSearchLims;
	}
	else {
		pNum = BETA;
		lims = pSearchLims + pNum; param = tempModel.theta + pNum;
		if(param->free) {
			temp = slopeSearchLims.min; if(gLogSlopes) temp /= (halfwayShift * log(10.0));
			if(temp < linSlopeSearchLims.min) temp = linSlopeSearchLims.min;
			if(temp > linSlopeSearchLims.max) temp = linSlopeSearchLims.max;
			lims->min = get_beta(tempModel.shape, halfwayShift, temp, 0.5);

			temp = slopeSearchLims.max; if(gLogSlopes) temp /= (halfwayShift * log(10.0));
			if(temp < linSlopeSearchLims.min) temp = linSlopeSearchLims.min;
			if(temp > linSlopeSearchLims.max) temp = linSlopeSearchLims.max;
			lims->max = get_beta(tempModel.shape, halfwayShift, temp, 0.5);

			if(lims->min > lims->max) temp = lims->min, lims->min = lims->max, lims->max = temp;
			get_limits(tempModel.shape, BETA);
			MakeLimitsLegal(lims, &param->constraint, gLegal.min, gLegal.max);
			if(lims->min == lims->max) FixParam(tempModel.theta, pNum, lims->min);
		}
		else lims->min = lims->max = param->guess;


		pNum = ALPHA;
		lims = pSearchLims + pNum; param = tempModel.theta + pNum;
		if(param->free) {
			*lims = shiftSearchLims;
			temp = halfwaySlope; if(gLogSlopes) temp /= (shiftSearchLims.min * log(10.0));
			if(temp < linSlopeSearchLims.min) temp = linSlopeSearchLims.min;
			if(temp > linSlopeSearchLims.max) temp = linSlopeSearchLims.max;
			temp = get_alpha(tempModel.shape, shiftSearchLims.min, temp, 0.5);
			if(temp < lims->min) lims->min = temp;

			temp = halfwaySlope; if(gLogSlopes) temp /= (shiftSearchLims.max * log(10.0));
			if(temp < linSlopeSearchLims.min) temp = linSlopeSearchLims.min;
			if(temp > linSlopeSearchLims.max) temp = linSlopeSearchLims.max;
			temp = get_alpha(tempModel.shape, shiftSearchLims.max, temp, 0.5);
			if(temp > lims->max) lims->max = temp;

			get_limits(tempModel.shape, ALPHA);
			MakeLimitsLegal(lims, &param->constraint, gLegal.min, gLegal.max);
			if(lims->min == lims->max) FixParam(tempModel.theta, pNum, lims->min);
		}
		else lims->min = lims->max = param->guess;
	}

	for(i = 0; i < kNumberOfParams; i++) {
		min[i] = pSearchLims[i].min;
		max[i] = pSearchLims[i].max;
	}

	set_mlmt_info(data, &tempModel, tempModel.convertParams);
	MagicMesh(&tempModel, gMeshResolution, gMeshIterations, min, max, mlmt);

	for(i = 0; i < kNumberOfParams; i++) min[i] = tempModel.theta[i].fitted;

	if(isinf(gridScore = mlmt(min))) JError("guess procedure failed to find an approximate parameter set:\npossibly the user-supplied Bayesian priors constrain the fit\ntoo tightly for these data, or the priors may be mutually exclusive");

	if(tempModel.convertParams) TranslateAB(tempModel.shape, min, ts2ab);
	for(i = 0; i < kNumberOfParams; i++) {
		model->theta[i].guess = min[i];
		model->theta[i].fitted = NAN;
	}

    return 1;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void InitParam(ModelPtr model, short paramNumber, char * paramName)
{
	if(paramNumber<0 || paramNumber > kNumberOfParams-1)
		Bug("InitParam(): illegal parameter index %hd - must be from 0 to %hd", paramNumber, kNumberOfParams-1);
	strncpy(model->theta[paramNumber].name, paramName, kMaxParamNameLength);
	model->theta[paramNumber].free = TRUE;
	model->theta[paramNumber].constraint.prior = NULL;
	model->theta[paramNumber].guess = NAN;
	model->theta[paramNumber].fitted = NAN;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void Limits(MatrixBundle *bndl, double *conf, unsigned short nConf)
{
	unsigned short i, j, nVars, nSamples;
	double *tempSpace, bcaConf, bc, acc, z;
	matrix slice;
	boolean doBCA, doQuantiles;

	if(nConf == 0 || bndl->sim == NULL || bndl->sim->vals == NULL) return;
	nSamples = m_getsize(bndl->sim, 1);
	nVars = m_getsize(bndl->sim, 2);
	if(bndl->quant == NULL) bndl->quant = m_new(mNewData, m2D, nConf, nVars);
	if(bndl->lims == NULL) bndl->lims = m_new(mNewData, m2D, nConf, nVars);
	if(bndl->quant->vals == NULL) m_allocate(bndl->quant);
	if(bndl->lims->vals == NULL) m_allocate(bndl->lims);
	doQuantiles = m_first(bndl->quant);
	doBCA = (m_first(bndl->lims) && m_first(bndl->bc) && m_first(bndl->acc));
	if(!m_first(bndl->sim) || (!doQuantiles && !doBCA)) return;

	if(doQuantiles && (m_getsize(bndl->quant, 1) != nConf || m_getsize(bndl->quant, 2) != nVars)) Bug("Limits(): output matrix \"quant\" is the wrong shape");
	if(doBCA && (m_getsize(bndl->lims, 1) != nConf || m_getsize(bndl->lims, 2) != nVars)) Bug("Limits(): output matrix \"lims\" is the wrong shape");
	if(doBCA && (m_mass(bndl->bc) != nVars || m_mass(bndl->acc) != nVars)) Bug("Limits(): matrices \"bc\" and/or \"acc\" are the wrong size");

	tempSpace = New(double, nSamples);
	slice = m_slice(bndl->sim, 1, nSamples);
	for(j = 0; j < nVars; j++) {

		m_sortvals(tempSpace, slice);
		m_moveslice(slice, bndl->sim, 2, 1);

		bc = (doBCA ? m_val(bndl->bc) : NAN);
		acc = (doBCA ? m_val(bndl->acc) : NAN);

		for(i = 0; i < nConf; i++) {

			z = cg_inv(conf[i]);
			bcaConf =  cg(bc + (bc + z) / (1.0 - acc * (bc + z)));

			if(doQuantiles) {
				m_val(bndl->quant) = Quantile(conf[i], tempSpace, nSamples);
				m_next(bndl->quant);
			}
			if(doBCA) {
				m_val(bndl->lims) = Quantile(bcaConf, tempSpace, nSamples);
				m_next(bndl->lims);
			}
		}

		if(doBCA) {m_next(bndl->bc); m_next(bndl->acc);}
	}
	m_free(slice);
	Destroy(tempSpace);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void MagicMesh(ModelPtr model, unsigned short nSteps, unsigned short nIterations,
				double *min, double *max, double (*function)(double * params))
{
	unsigned short nFreeParams, totalSteps, i, bestPoint = 0;
	short loop, iteration;
	double p[kNumberOfParams];
	boolean pfree[kNumberOfParams];
	double absMin[kNumberOfParams];
	double absMax[kNumberOfParams];
	double min_err = 0, try_err, first_err = 0, newHalfWidth;
	double factor;
	boolean shrink, variation = 0;

#define PEEK_MESH		0

#if defined MATLAB_MEX_FILE && PEEK_MESH
int mexEvalf(char * fmt, ...);
#define PEEK_MESH_1		mexEvalf("INF = inf; a = {}; b = {}; m = {};\n");
#define PEEK_MESH_2		mexEvalf("a = [a {zeros(%d * ones(1, %d))}]; b = [b a(%d)]; m = [m a(%d)];\n", nSteps, nFreeParams, loop+1, loop+1);
#define PEEK_MESH_3		mexEvalf("a{%d}(%d) = %.20lg; b{%d}(%d) = %.20lg; m{%d}(%d) = %.20lg;\n", loop+1, i+1, p[ALPHA], loop+1, i+1, p[BETA], loop+1, i+1, try_err);
#else
#define PEEK_MESH_1		(void)0;
#define PEEK_MESH_2		(void)0;
#define PEEK_MESH_3		(void)0;
#endif

	nFreeParams = 0;
	totalSteps = 1;
	for(i = 0; i < kNumberOfParams; i++) {
		if((pfree[i] = model->theta[i].free) == TRUE) {
			if(max[i] <= min[i] || isinf(min[i]) || isnan(min[i]) || isinf(max[i]) || isnan(max[i]))
				Bug("MagicMesh() received illegal parameter limits [%lg, %lg] for %s", min[i], max[i], model->theta[i].name);
			totalSteps *= nSteps;
		}
		else p[i] = min[i] = max[i] = model->theta[i].guess; /* if fixed through model */
		if(min[i] == max[i]) {pfree[i] = FALSE; p[i] = min[i];} /* if fixed because min==max */

		if(pfree[i]) nFreeParams++;
		absMin[i] = min[i]; absMax[i] = max[i];
	}
	CheckModel(model, FALSE);

	factor = 1.0 / (double)(nSteps - 1);

	PEEK_MESH_1

	for(loop = 0, iteration = 0;
		iteration < nIterations && loop < nIterations * 2 * nSteps;
		iteration++, loop++) {

		PEEK_MESH_2

		for(i = 0; i < totalSteps; i++) {
			MagicMeshPoint(i, nSteps, p, pfree, min, max);
			try_err = (*function)(p);
			if(i == 0 || try_err < min_err) {
				min_err = try_err;
				bestPoint = i;
			}
			if(i == 0) {first_err = try_err; variation = FALSE;}
			else if(!variation) variation = (try_err != first_err);

			PEEK_MESH_3
		}
		if(variation) MagicMeshPoint(bestPoint, nSteps, p, pfree, min, max);
		else for(i = 0; i < kNumberOfParams; i++) p[i] = (min[i] + max[i]) / 2.0;

		shrink = TRUE;
		for(i = 0; i < kNumberOfParams; i++) {
			if(!pfree[i]) continue;
			else if((p[i] == min[i] && p[i] > absMin[i])) {
				shrink = FALSE;
				if((min[i] -= (max[i] - min[i])) < absMin[i]) min[i] = absMin[i];
			}
			else if((p[i] == max[i] && p[i] < absMax[i])) {
				shrink = FALSE;
				if((max[i] += (max[i] - min[i])) > absMax[i]) max[i] = absMax[i];
			}
		}
		if(!shrink) {
			iteration--;
			continue;
		}
		for(i = 0; i < kNumberOfParams; i++) {
			if(!pfree[i]) continue;
			newHalfWidth = (max[i] - min[i]) * factor;
			if((min[i] = p[i] - newHalfWidth) < absMin[i]) min[i] = absMin[i];
			if((max[i] = p[i] + newHalfWidth) > absMax[i]) max[i] = absMax[i];
		}

	}

	for(i = 0; i < kNumberOfParams; i++) model->theta[i].fitted = p[i];

}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void MagicMeshPoint(unsigned short stepNumber, unsigned short stepsPerDimension,
					double * p, boolean *pfree, double * min, double * max)
{
	unsigned short i;

	for(i = 0; i < kNumberOfParams; i++) {
		if(!pfree[i]) continue;
		p[i] = min[i] + (double)(stepNumber%stepsPerDimension) * (max[i] - min[i])/(double)(stepsPerDimension-1);
		stepNumber /= stepsPerDimension;
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double MakeLegal(PsychDistribFuncPtr shape, ArgIdentifier wrt, double val)
{
	get_limits(shape, wrt);
	if(val < gLegal.min) val = gLegal.min;
	if(val > gLegal.max) val = gLegal.max;
	return val;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void MakeLimitsLegal(SearchLimitsPtr lims, ConstraintPtr con, double absoluteMin, double absoluteMax)
{
	double temp, wMin, wMax;

	if(lims->min > lims->max) {
		temp = lims->min;
		lims->min = lims->max;
		lims->max = temp;
	}
	if(con != NULL && con->prior != NULL) {
		wMin = GetWorkingMin(con);
		wMax = GetWorkingMax(con);
		temp = 0.02 * (wMax - wMin);
		if(!isinf(temp) && !isnan(temp)) {wMin += temp; wMax -= temp;}

		/* if upper and lower limits are the same (parameter fixed),
		   make sure the fixed value is somewhere in the range allowed by constraints */
		if(lims->min == lims->max && (lims->min < wMin || lims->min > wMax)) {
			if(isinf(wMin)) lims->min = lims->max = wMax;
			else if(isinf(wMax)) lims->min = lims->max = wMin;
			else lims->min = lims->max = (wMin + wMax) / 2.0;
		}
		/* make sure the search limits are within the constraint limits */
		if(lims->min < wMin) lims->min = wMin;
		if(lims->min > wMax) lims->min = wMax;
		if(lims->max < wMin) lims->max = wMin;
		if(lims->max > wMax) lims->max = wMax;
	}
	if(lims->min < absoluteMin) lims->min = absoluteMin;
	if(lims->min > absoluteMax) lims->min = absoluteMax;
	if(lims->max < absoluteMin) lims->max = absoluteMin;
	if(lims->max > absoluteMax) lims->max = absoluteMax;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
extern boolean gLambdaEqualsGamma;
double mlmt(double * pIn)
/*	mlmt = - ( log(bayesian prior) + SUM_i(nRight_i*log(p_i) + nWrong_i*log(1-p_i)) )  */
{
	double scale, predicted, result, p[kNumberOfParams];
	double gamma;
	short i;

	/* Start with -log(bayesian prior) */
	if((result = Priors(gMLMT_model, pIn, p, gMLMT_paramsConverted)) == 0.0) return INF;
	result = - log_j(result);

	gamma = (gLambdaEqualsGamma ? p[LAMBDA] : p[GAMMA]);
	scale = 1.0 - gamma - p[LAMBDA];
	for(i=0;i<gMLMT_data->nPoints;i++) {
/*		Subtract (nRight*log(p) + nWrong * log(1-p)) for each data point 	*/
		predicted = gamma + scale * prob(gMLMT_model->shape, p[ALPHA], p[BETA], gMLMT_data->x[i]);
		result -= xlogy_j(gMLMT_data->nRight[i], predicted) + xlogy_j(gMLMT_data->nWrong[i], 1.0 - predicted);
	}

	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void MonteCarlo(DataSetPtr originalData, ModelPtr model, GeneratingInfoPtr gen, OutputPtr out)
{
	unsigned int i;
	double p[kNumberOfParams], simGuess[kNumberOfParams];
	boolean pfree[kNumberOfParams];
	short err = 0;
	DataSet dat1 = NULL_DATA_SET, dat2 = NULL_DATA_SET;
	DataSetPtr collated = NULL, uncollated = NULL;
	double *chronIndex = NULL, *tempPsi = NULL;
	double *predictedU = NULL, *nObsU = NULL;
	double *predictedC = NULL, *nObsC = NULL;
	double *predictedR = NULL, *rSpace = NULL, *kSpace = NULL;
	int (*fitCoreFcn)(double *pIn, double *pOut, boolean *pFree);

	for(i = 0; i < kNumberOfParams; i++) pfree[i] = model->theta[i].free;

	if(gAdaptPtr) {
		fitCoreFcn = CAdaptiveFitCore;
		m_setsize(out->stats.sim, 2, 0, 0);
		m_setsize(out->ySim, 2, 0, 0);
		m_setsize(out->rSim, 2, 0, 0);
		CSetAdaptiveGeneratingFunction(gen->shape, gen->params);
		CSetUpAdaptiveOutput(gen->nRuns);
/*		data set pointers point to separate sets, in
		which all pointers are NULL to begin with.
		chronIndex, predictedU, predictedC, nObsU, nObsC,
		predictedR, rSpace, kSpace all remain NULL pointers
*/		uncollated = &dat1; collated = &dat2;
		gMLMT_data = NULL;
	}
	else {
		fitCoreFcn = FitCore;
/*		set up to generate fake data sets	*/
		chronIndex = New(double, originalData->nPoints);
		for(i = 0; i < originalData->nPoints; i++) chronIndex[i] = (double)i;
		tempPsi = (gen->psi ? CopyVals(NULL, gen->psi, uncollated->nPoints, sizeof(double)) : NULL);
		DuplicateDataSet((uncollated = &dat1), originalData);
		if(isnan(uncollated->x[0]) && tempPsi != NULL) SortDoubles(4, uncollated->nPoints, tempPsi, uncollated->nRight, uncollated->nWrong, chronIndex);
		else SortDoubles((tempPsi == NULL ? 4 : 5), uncollated->nPoints, uncollated->x, uncollated->nRight, uncollated->nWrong, chronIndex, tempPsi);
		MonteCarloGenerators(uncollated, gen->shape, gen->params, tempPsi, &predictedU, &nObsU);
		CollateData((collated = &dat2), uncollated, tempPsi);
		MonteCarloGenerators(collated, gen->shape, gen->params, tempPsi, &predictedC, &nObsC);
		if(tempPsi) {Destroy(tempPsi); tempPsi = NULL;}
		if(collated->nPoints == uncollated->nPoints) collated = uncollated;
/* 		(by making the pointers equal, we make GenerateFakeDataSet run slightly faster) */

/*		set up for p:r correlation	*/
		if(m_first(out->stats.sim)) {
			predictedR = (out->refit ? New(double, uncollated->nPoints) : NULL);
			rSpace = New(double, uncollated->nPoints);
			kSpace = New(double, uncollated->nPoints);
		}
	}

/*	initialize Simplex start-point based on generating params */
	sprintf(gErrorContext, "failed to approximate generating distribution with the specified model:\n");
	if(gen->psi == NULL && out->doParams) {
		for(i = 0; i < kNumberOfParams; i++)
			simGuess[i] = (model->theta[i].free ? gen->params[i] : model->theta[i].guess);
		if(gen->shape != model->shape) {
			TranslateAB(gen->shape, simGuess, ab2ts);
			if(legal_x(model->shape, simGuess[ALPHA]) && legal_gradient(model->shape, simGuess[BETA]))
				TranslateAB(model->shape, simGuess, ts2ab);
/* 				(if shift and slope are legal in new shape, translate them directly...) */
			else {
				TranslateAB(gen->shape, simGuess, ts2ab);
				FakeFit(model, simGuess, 20, NULL, NULL, gen->shape, simGuess);
/* 				(...otherwise translate them back using old shape, and do a "fake fit" with 20 points) */
			}
		}
	}
	else if(out->doParams) {
		if(collated->nPoints == 0 || predictedC == NULL)
			Bug("data set must exist for \"guess\" fit to GEN_VALUES");
		FakeFit(model, simGuess, collated->nPoints, collated->x, predictedC, NULL, NULL);
/* 		(approximate the sampled generating distribution as well as possible using a "fake fit") */
	}
//	sprintf(gErrorContext, "");
/* 	***	The next section was inserted because, with lambda_gen close to 0,
		lapses will still occur in some of the simulated sets, but the
		simplex search appears to get stuck in a local minimum "channel"
		near to lambda=0 when started at that value. This is a crude fix. */
/*	*** (Probably solved with the introduction of "PsychSetSimplexSizes"
	    However it can't hurt to keep this in---19/3/01)*/
#define kMinBoundOffset 	0.01
	if(model->theta[LAMBDA].free && simGuess[LAMBDA] < kMinBoundOffset &&
	   prior(1.0, &model->theta[LAMBDA].constraint, kMinBoundOffset) > 0.0)
			simGuess[LAMBDA] = kMinBoundOffset;
/*	***	The same applies to low values of gamma in subjective paradigms. */
	if(model->theta[GAMMA].free && simGuess[GAMMA] < kMinBoundOffset &&
	   prior(1.0, &model->theta[GAMMA].constraint, kMinBoundOffset) > 0.0)
			simGuess[GAMMA] = kMinBoundOffset;
/*	*** */

/*	convert guess parameters to threshold/slope format if thresholds/slopes are fixed */
	if(model->convertParams) TranslateAB(model->shape, simGuess, ab2ts);


/*	run */
	PsychSetSimplexSizes(collated, model->shape, simGuess, model->convertParams);
	set_mlmt_info(collated, model, model->convertParams);
	InitRandomSeed(gen->randomSeed);

	for(gen->run = 1; gen->run <= gen->nRuns; gen->run++) {

		if(gAdaptPtr) CDoAdaptive(uncollated, collated);
		else {
			/* DATA */
			GenerateFakeDataSet(uncollated, predictedU, nObsU, collated, predictedC, nObsC);

			/* Y_SIM, R_SIM */
			if(m_setpoint(out->ySim, gen->run - 1, 0)) {
				for(i = 0; i < uncollated->nPoints; i++)
					if(m_setpos(out->ySim, 2, (long)(chronIndex[i]))) m_val(out->ySim) = uncollated->nRight[i] / (uncollated->nRight[i] + uncollated->nWrong[i]);
			}
			if(m_setpoint(out->rSim, gen->run - 1, 0)) {
				for(i = 0; i < uncollated->nPoints; i++)
					if(m_setpos(out->rSim, 2, (long)(chronIndex[i]))) m_val(out->rSim) = uncollated->nRight[i];
			}
		}

		if(gen->run == out->dataExportIndex) ExportDataSet(uncollated, out->dataExport, chronIndex);
		/*LDOT woz ere*/

		if(m_setpoint(out->params.sim, gen->run - 1, 0)) {

			/* FIT */
			err = (*fitCoreFcn)(simGuess, p, pfree);
			if(err == 0 && model->convertParams) TranslateAB(model->shape, p, ts2ab);
			for(i = 0; i < kNumberOfParams; i++) {
				m_val(out->params.sim) = p[i];
				m_step(out->params.sim, 2, 1);
			}

			/* THRESHOLDS & SLOPES */
			if((m_setpoint(out->thresholds.sim, gen->run - 1, 0) | m_setpoint(out->slopes.sim, gen->run - 1, 0)) != 0)
				for(i = 0; i < out->nCuts; i++) ThresholdAndSlope(model->shape, p, out->cuts[i], m_addr(out->thresholds.sim, 2, i), m_addr(out->slopes.sim, 2, i), NONE);

			if(gDoBootstrapT && gen->shape == model->shape) BootstrapT(model, p, uncollated, out, gen->run);

			if(predictedR) /* i.e. if refitting, a new prediction must be made on each run for statistical testing purposes */
				ExpectedValues(predictedR, uncollated->nPoints, uncollated->x, model->shape, p, "fitted values");
		}

		/* STATS */
		if(m_setpoint(out->stats.sim, gen->run - 1, 0)) {
			if(err) { do m_val(out->stats.sim) = NAN; while(m_step(out->stats.sim, 2, 1)); }
			else DoStats((predictedR ? predictedR : predictedU), uncollated, chronIndex, m_addr(out->stats.sim, 2, 0), m_addr(out->stats.sim, 2, 1), m_addr(out->stats.sim, 2, 2), rSpace, kSpace);
		}

		/* LDOT */
		if(m_setpoint(out->ldot, gen->run - 1, 0)) {
			if(gen->psi) Bug("MonteCarlo(): should not be calculating LDOT if gen_psi was supplied");
			for(i = 0; i < kNumberOfParams; i++) {
				m_val(out->ldot) = (model->theta[i].free ? DiffLoglikelihood(gen->shape, gen->params, (ArgIdentifier)i, collated, model) : 0);
				m_step(out->ldot, 2, 1);
			}
//			APPROX_3;
		}
	}

	if(gAdaptPtr) CAdaptiveCleanup();

	if(kSpace) Destroy(kSpace);
	if(rSpace) Destroy(rSpace);
	if(predictedR) Destroy(predictedR);
	if(nObsC) Destroy(nObsC);
	if(predictedC) Destroy(predictedC);
	DisposeDataSet(&dat2);
	if(nObsU) Destroy(nObsU);
	if(predictedU) Destroy(predictedU);
	DisposeDataSet(&dat1);
	if(chronIndex) Destroy(chronIndex);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void MonteCarloGenerators(DataSetPtr data, PsychDistribFuncPtr shape, double *params, double *psi,
						double **predicted, double **nObs)
{
	unsigned short i;
	double alpha, beta, gamma, scaleF;

	alpha = params[ALPHA];
	beta = params[BETA];
	gamma = params[GAMMA];
	scaleF = 1.0 - gamma - params[LAMBDA];

	*predicted = New(double, data->nPoints);
	*nObs = New(double, data->nPoints);
	for(i = 0; i < data->nPoints; i++) {
		(*predicted)[i] = (psi ? psi[i] : gamma + scaleF * prob(shape, alpha, beta, data->x[i]));
		(*nObs)[i] = (unsigned short)(0.5 + data->nRight[i] + data->nWrong[i]);
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double Priors(ModelPtr model, double *pIn, double *pOut, boolean paramsConverted)
{
	int i;
	double result, shift = 0, scale, slope = 0;
	double localParams[kNumberOfParams], *p;

	p = ((pOut == NULL) ? localParams : pOut);
	CopyVals(p, pIn, kNumberOfParams, sizeof(double));
	if(paramsConverted) {
		shift = p[ALPHA];
		slope = p[BETA];
		TranslateAB(model->shape, p, ts2ab);
	}

	if(!CheckParams(model->shape, p, NULL)) return 0.0;
	scale = 1.0 - p[GAMMA] - p[LAMBDA];

	/* NB if any priors are added, removed or changed, the procedure DiffLogAllPriors() must also be adjusted */

	result = 1.0;
	/* parameter priors */
	for(i = 0; i < kNumberOfParams; i++)
		if((result = prior(result, &model->theta[i].constraint, p[i])) == 0.0) return 0.0;

	/* "tail drift" prior */
	if((result = prior(result, &model->tailConstraint, prob(model->shape, p[ALPHA], p[BETA], model->xValAtChance))) == 0.0) return 0.0;

	/* priors on real shifts/slopes */
	if(model->shiftConstraint.prior || (gLogSlopes && model->slopeConstraint.prior))
		if(!paramsConverted) shift = inv_prob(model->shape, p[ALPHA], p[BETA], 0.5);
	if(model->shiftConstraint.prior) {
		if(!legal_alpha(model->shape, shift)) return 0.0;
		if((result = prior(result, &model->shiftConstraint, shift)) == 0.0) return 0.0;
	}
	/* gCutPsi is disregarded for shift calculation, above - we're interested in the half-way point no matter what */
	/* However, it is taken into account in slope below, so that the prior can be entered in familiar units to those who use performance thresholds */
	if(model->slopeConstraint.prior) {
		if(!paramsConverted) {
			slope = diff_prob(model->shape, p[ALPHA], p[BETA], shift);
			if(!legal_gradient(model->shape, slope)) return 0.0;
			if(gCutPsi) slope *= scale;
			if(gLogSlopes) slope *= shift * log_j(10.0);
		}
		if((result = prior(result, &model->slopeConstraint, slope)) == 0.0) return 0.0;
	}

	return result;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void PsychSetSimplexSizes(DataSetPtr data, PsychDistribFuncPtr shape, double * guess, boolean useTSUnits)
{
	double p[kNumberOfParams];
	double th1, th2, sl1, sl2;
	double lgr = 0.2, slrf = 5.0, frac = 0.02;
	double minX, maxX;
	unsigned short i;
	boolean bad;

	CopyVals(p, guess, kNumberOfParams, sizeof(double));
	if(!useTSUnits) TranslateAB(shape, p, ab2ts);
	bad = isinf(p[0]) || isnan(p[0]) || !legal_alpha(shape, p[0]) ||
	      isinf(p[1]) || isnan(p[1]) || !legal_gradient(shape, p[1]);
	if(bad) {
		minX = INF; maxX = -INF;
		for(i = 0; i < data->nPoints; i++) {
			if(data->x[i] < minX) minX = data->x[i];
			if(data->x[i] > maxX) maxX = data->x[i];
		}
		p[0] = (maxX + minX) / 2.0;
		p[1] = 3.0 * ((p[1] < 0.0) ? -1.0 : 1.0) / (maxX - minX);
	}
	th1 = p[0] - frac * 0.5 / p[1];
	th2 = p[0] + frac * 0.5 / p[1];
	th1 = MakeLegal(shape, ALPHA, th1);
	th2 = MakeLegal(shape, ALPHA, th2);

	sl1 = p[1] + frac * (p[1] * slrf - p[1]);
	sl2 = p[1] + frac * (p[1] / slrf - p[1]);
	sl1 = MakeLegal(shape, DFDX, sl1);
	sl2 = MakeLegal(shape, DFDX, sl2);

	bad = (th1 == th2) || isinf(th1) || isnan(th1) || isinf(th2) || isnan(th2)
	   || (sl1 == sl2) || isinf(sl1) || isnan(sl1) || isinf(sl2) || isnan(sl2);
	if(bad) JError("failed to obtain a legal estimate of parameter variability to initialize the simplex search\ndata may be too poorly sampled, or have an illegal gradient for the chosen function shape");

	if(!useTSUnits) {
		th1 = get_alpha(shape, th1, p[1], 0.5);
		th2 = get_alpha(shape, th2, p[1], 0.5);
		sl1 = get_beta(shape, p[0], sl1, 0.5);
		sl2 = get_beta(shape, p[0], sl2, 0.5);
	}
	gPsychSimplexSizes[ALPHA] = th2 - th1;
	if(isnan(gPsychSimplexSizes[ALPHA]) || isinf(gPsychSimplexSizes[ALPHA]))
		gPsychSimplexSizes[ALPHA] = 2.0 * (p[0] - th1);
	if(isnan(gPsychSimplexSizes[ALPHA]) || isinf(gPsychSimplexSizes[ALPHA]))
		gPsychSimplexSizes[ALPHA] = 2.0 * (th2 - p[0]);
	if(isnan(gPsychSimplexSizes[ALPHA]) || isinf(gPsychSimplexSizes[ALPHA]))
		JError("the fitting engine could not determine a suitable scale for searching for ALPHA");

	gPsychSimplexSizes[BETA] = sl2 - sl1;
	if(isnan(gPsychSimplexSizes[BETA]) || isinf(gPsychSimplexSizes[BETA]))
		gPsychSimplexSizes[BETA] = 2.0 * (p[1] - sl1);
	if(isnan(gPsychSimplexSizes[BETA]) || isinf(gPsychSimplexSizes[BETA]))
		gPsychSimplexSizes[BETA] = 2.0 * (sl2 - p[1]);
	if(isnan(gPsychSimplexSizes[BETA]) || isinf(gPsychSimplexSizes[BETA]))
		JError("the fitting engine could not determine a suitable scale for searching for BETA");

	gPsychSimplexSizes[GAMMA] = lgr * frac;
	gPsychSimplexSizes[LAMBDA] = lgr * frac;
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
double Quantile(double conf, double *orderedVals, long nVals)
{
	double index, floorWeight;
	int floorIndex, ceilIndex;

	while(isnan(orderedVals[nVals-1])) nVals--;
	index = conf * (double)(nVals + 1) - 1.0;
	if(index < 0.0 || index > (double)(nVals - 1.0)) return NAN;

	floorIndex = ceilIndex = (int)ceil(index);
	if(floorIndex) floorIndex--;
	floorWeight = (double)ceilIndex - index;

	return floorWeight * orderedVals[floorIndex] + (1.0 - floorWeight) * orderedVals[ceilIndex];
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void ReportDataSet(DataSetPtr data)
{
	short i;
	for(i=0;i<data->nPoints;i++){
		switch(gDataFormat) {
			case unknown_format:
			case xyn: fprintf(stderr, "%3.2lf\t%5.3lf\t%3lg\n", data->x[i], data->nRight[i] / (data->nRight[i] + data->nWrong[i]), data->nRight[i] + data->nWrong[i]); break;
			case xrw: fprintf(stderr, "%3.2lf\t\t%3lg\t%3lg\n", data->x[i], data->nRight[i], data->nWrong[i]); break;
			case xrn: fprintf(stderr, "%3.2lf\t\t%3lg\t%3lg\n", data->x[i], data->nRight[i], data->nRight[i] + data->nWrong[i]); break;
		}
	}
	fprintf(stderr, "\n");
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void ReportModel(ModelPtr model)
{
	unsigned short i;
	char shiftString[36], slopeString[36], tailLevelString[36], *str;
	char *gammaName;

	gammaName = (gLambdaEqualsGamma ? model->theta[LAMBDA].name : model->theta[GAMMA].name);

	sprintf(shiftString, "x at F(x)==0.5");
	sprintf(slopeString, "d%s/d%s at F(x)==0.5", (gCutPsi ? "Psi" : "F"), (gLogSlopes ? "(log10 x)" : "x"));
	sprintf(tailLevelString, "F(%lg)", model->xValAtChance);

	fprintf(stderr, "psi(x) = %s + (1 - %s - %s) * F(x, %s, %s)\nusing %s function for F ", gammaName, gammaName, model->theta[LAMBDA].name, model->theta[ALPHA].name, model->theta[BETA].name, FunctionName(model->shape));
	if(gLambdaEqualsGamma) fprintf(stderr, "\n(note that upper and lower asymptote offsets are forced to be equal)\n");
	else if(model->nIntervals == 1) fprintf(stderr, "and assuming single-interval design\n");
	else {
		fprintf(stderr, "and assuming %huAFC design", model->nIntervals);
		if(model->theta[GAMMA].free) fprintf(stderr, "\n(note, however, that %s has been explicitly allowed to vary)\n", gammaName);
		else if(fabs(model->theta[GAMMA].guess - (1.0 / (double)model->nIntervals)) > 0.000001)
			fprintf(stderr, "\n(note, however, that %s has been explicitly fixed at %lg)\n", gammaName, model->theta[GAMMA].guess);
		else fprintf(stderr, " (%s = %lg)\n", gammaName, model->theta[GAMMA].guess);
	}
	for(i = 0; i < kNumberOfParams; i++) {
		if(i == GAMMA && gLambdaEqualsGamma) continue;
		str = ((i == ALPHA && model->convertParams) ? shiftString : (i == BETA && model->convertParams) ? slopeString : model->theta[i].name);
		if(!model->theta[i].free && !(i == 2 && model->nIntervals > 1))
			fprintf(stderr, "%s is fixed at %lg\n", str, model->theta[i].guess);
		else if(model->theta[i].free && ReportPrior(model->theta[i].name, &model->theta[i].constraint)) fprintf(stderr, "\n");
	}
	if(ReportPrior(tailLevelString, &model->tailConstraint)) fprintf(stderr, " (no signal present)\n");
	if(ReportPrior(shiftString, &model->shiftConstraint)) fprintf(stderr, "\n");
	if(ReportPrior(slopeString, &model->slopeConstraint)) fprintf(stderr, "\n");
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int SelfTest(void)
{
	ModelPtr model = NULL;
	GeneratingInfoPtr gen = NULL;
	OutputPtr out = NULL;
	DataSetPtr data;
	double flatdata[8*3] = {1.0,  4.0,  6.0,  1.6,  3.5,  1.8,  3.0,  2.5, \
	                       20.0, 40.0, 40.0, 23.0, 52.0, 35.0, 49.0, 40.0, \
	                       20.0, 00.0, 00.0, 17.0,  8.0, 25.0, 11.0, 20.0};
	char prefString[] = "#shape Weibull\n#n_intervals 2\n#verbose false\n#runs 0\n#write_pa_est 0\n";
	Batch prefs = {NULL, 0, 0, FALSE};
	double percentError, targetParams[kNumberOfParams] = {3.06836, 4.00961, 0.5, 3.09818e-07};
	boolean floatOK, accurate = TRUE;
	int i;

	prefs.buffer = prefString; prefs.length = strlen(prefString);
	fprintf(stderr, "psignifit engine self test (engine version = %s)\n\n(1) ", PSIGNIFIT_VERSION);
	floatOK = (boolean)TestFloatingPointBehaviour();
	InitPrefs(&prefs, &model, &data, &gen, &out, m_new(flatdata, m2D, 8, 3));

	fprintf(stderr, "\n(2) Fitting Weibull function to standard 2AFC data...\n");
	FitPsychometricFunction(data, model, NULL, out->verbose);
	fprintf(stderr, "      initial guess : {");
	for(i = 0; i < kNumberOfParams; i++) fprintf(stderr, "%.1s = %lg%s", model->theta[i].name, model->theta[i].guess, (i == kNumberOfParams - 1) ? "}\n" : ", ");
	fprintf(stderr, "  fitted parameters : {");
	for(i = 0; i < kNumberOfParams; i++) fprintf(stderr, "%.1s = %lg%s", model->theta[i].name, model->theta[i].fitted, (i == kNumberOfParams - 1) ? "}\n" : ", ");
	fprintf(stderr, "          should be : {");
	for(i = 0; i < kNumberOfParams; i++) fprintf(stderr, "%.1s = %lg%s", model->theta[i].name, targetParams[i], (i == kNumberOfParams - 1) ? "}\n" : ", ");
	fprintf(stderr, "\n");
	for(i = 0; i < kNumberOfParams; i++) {
		percentError = 100.0 * fabs(model->theta[i].fitted - targetParams[i]) / targetParams[i];
		if(percentError > 0.1) accurate = FALSE, fprintf(stderr, "Warning: %s is %lg%% out\n", model->theta[i].name, percentError);
	}

 	if(!floatOK) fprintf(stderr, "Warning: IEEE standards not fully supported.\nFloating-point results from this compiled version are likely to be\ninaccurate and unpredictable.\n");
	if(accurate) {
		if(floatOK) fprintf(stderr, "*** success! ***\n");
		else fprintf(stderr, "However, this particular fit was successful.\n");
	}
	else if(floatOK) fprintf(stderr, "Sorry, the psignifit engine has not been properly debugged for your platform.\nResults may be unreliable.\n");

	m_clear();
	DisposeDataSet(data);
	Destroy(data);
	Destroy(model);
	Destroy(gen);
    if(out->conf) Destroy(out->conf);
	if(out->cuts) Destroy(out->cuts);
    Destroy(out);
    ReportBlocks();

	return ((floatOK && accurate) ? EXIT_SUCCESS : EXIT_FAILURE);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void set_mlmt_info(DataSetPtr data, ModelPtr model, boolean treatABasTS)
	{gMLMT_data = data; gMLMT_model = model; gMLMT_paramsConverted = treatABasTS;}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void ThresholdAndSlope(PsychDistribFuncPtr shape, double * params, double cut, double *thPtr, double *slPtr, ArgIdentifier wrt)
{
	ArgIdentifier wrt1, wrt2;
	double f, t, s, tOut = 0, sOut = 0, dt_du, dt_dv, ds_du, ds_dv;

	/*
		N.B. the gCutPsi option was disabled 19/10/99 because of the unnecessary complications it caused here
		If it is used, all slope values must be multiplied by (1-gamma-lambda), and all threshold and slopes
		must be calculated using f = (cut - gamma)/(1-gamma-lambda) as the argument. When differentiating wrt
		gamma or lambda, this means that 8 more formulae would be required in each psychometric function:
		dt/df, d2t/df2, d2t/dadf, d2t/dbdf, and similarly for s(f; {a, b}). As it is, using f = cut, 10
		formulae are required for differentiation: dt/da, dt/db, d2t/da2, d2t/db2, d2t/dadb and similarly for
		s(f; {a, b}).
	*/

	f = cut;
	s = slope(shape, params[ALPHA], params[BETA], f);
	t = inv_prob(shape, params[ALPHA], params[BETA], f);

	if(!DoubleDiff(wrt, &wrt1, &wrt2)) {
		switch(wrt) {
			case NONE:
				tOut = t;
				sOut = s;
				if(gLogSlopes) sOut *= t * log(10.0);
				break;
			case ALPHA: case BETA:
				tOut = (*shape)(f, NAN, params[ALPHA], params[BETA], threshold_derivative, wrt);
				sOut = (*shape)(f, NAN, params[ALPHA], params[BETA], slope_derivative, wrt);
				if(gLogSlopes) sOut = log(10.0) * (sOut * t + tOut * s);
				break;
			case GAMMA: case LAMBDA:
				tOut = sOut = 0.0;
				break;
		}
	}
	else {
		if(wrt1 == GAMMA || wrt2 == GAMMA || wrt1 == LAMBDA || wrt2 == LAMBDA) tOut = sOut = 0.0;
		else {
			tOut = (*shape)(f, NAN, params[ALPHA], params[BETA], threshold_derivative, wrt);
			sOut = (*shape)(f, NAN, params[ALPHA], params[BETA], slope_derivative, wrt);

			if(gLogSlopes) {
				dt_du = (*shape)(f, NAN, params[ALPHA], params[BETA], threshold_derivative, wrt1);
				dt_dv = (*shape)(f, NAN, params[ALPHA], params[BETA], threshold_derivative, wrt2);
				ds_du = (*shape)(f, NAN, params[ALPHA], params[BETA], slope_derivative, wrt1);
				ds_dv = (*shape)(f, NAN, params[ALPHA], params[BETA], slope_derivative, wrt2);

				sOut = log(10.0) * (sOut * t + tOut * s + dt_du * ds_dv + dt_dv * ds_du);
			}
		}
	}

	if(thPtr) *thPtr = tOut;
	if(slPtr) *slPtr = sOut;

}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
int TransformedRegression(unsigned short nPoints, double *x, double *y, double *weights,
		PsychDistribFuncPtr shape, double *alpha, double *beta, double gamma, double lambda)
{
	unsigned short i, nValidPoints;
	double *xT, *fT, *wT;
	double scale, f, mTran, cTran, mLin, cLin, shift, slope;
	boolean valid, gotHi, gotLo;
	unsigned short min = 0, max = 0;

	scale = 1.0 - gamma - lambda;
	xT = New(double, nPoints);
	fT = New(double, nPoints);
	wT = New(double, nPoints);
	gotHi = gotLo = FALSE;
	for(nValidPoints = 0, i = 0; i < nPoints; i++) {
		f = (y[i] - gamma) / scale;

		if(x[i] < x[min]) min = i; if(x[i] > x[max]) max = i;

		if(f <= 0.0 || f >= 1.0) continue;
		if(f <= 0.4) gotLo = TRUE;
		if(f >= 0.6) gotHi = TRUE;
		xT[nValidPoints] = rtx(shape, x[i]);
		fT[nValidPoints] = rtf(shape, f);
		wT[nValidPoints] = weights[i];
		nValidPoints++;
	}
	valid = (nValidPoints >= 3 && gotHi && gotLo);
	if(valid) {
		WeightedLinearRegression(nValidPoints, xT, fT, wT, &mTran, &cTran);
		shift = *alpha = rtcm_a(shape, cTran, mTran);
		slope = *beta = rtcm_b(shape, cTran, mTran);
		if(!legal_alpha(shape, *alpha)) valid = FALSE;
	}
	WeightedLinearRegression(nPoints, x, y, weights, &mLin, &cLin);
	mLin /= scale; cLin = (cLin - gamma) / scale;
	if(!valid || (nValidPoints < nPoints && mTran/mLin <= 0.0)) {
/*	if insufficient points, or if the transformed-fitted slope from a reduced data set has different
	sign from the linear regression on the whole, use the linear regression, arbitrarily raising
	the gradient by a factor of 5
*/		shift = (0.5 - cLin) / mLin;
		if(!legal_x(shape, shift)) shift = median(nPoints, x);

		slope = 5.0 * mLin;
		get_limits(shape, DFDX);
		if(slope <= gLegal.min) slope = gLegal.min + EPS;
		if(slope >= gLegal.max) slope = gLegal.max - EPS;

		*alpha = get_alpha(shape, shift, slope, 0.5);
		*beta = get_beta(shape, shift, slope, 0.5);
		if(isnan(*alpha) || isinf(*alpha)) *alpha = shift;
		/* and finally if beta is non-real, man, what /can/ you do?? */
        if(isnan(*beta) || isinf(*beta)) {
            JError("transformed regression failed");
            return 0;
        }
	}
	Destroy(xT); Destroy(fT); Destroy(wT);
    if(!legal_beta(shape, *beta)) {
		JError("%scannot estimate a legal slope parameter for the %s function:\napparent %s slope", gErrorContext, FunctionName(shape), (slope > 0.0) ? "positive" : slope < 0.0 ? "negative" : "zero");
        return 0;
    }
    return 1;

}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void TranslateAB(PsychDistribFuncPtr shape, double *p, Translation t)
{
	double th, sl, cut = 0.5;
	switch(t) {
		case ts2ab:
			th = p[ALPHA]; sl = p[BETA];
			if(gLogSlopes) sl /= th * log(10.0);
			p[ALPHA] = get_alpha(shape, th, sl, cut);
			p[BETA] = get_beta(shape, th, sl, cut);
			break;
		case ab2ts:
			ThresholdAndSlope(shape, p, cut, &th, &sl, NONE);
/*			The function ThresholdAndSlope takes care of the gLogSlope preference */
			p[ALPHA] = th; p[BETA] = sl;
			break;
		default:
			Bug("unknown translation argument to TranslateAB()");
	}
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
void VarianceEstimates(MatrixBundle *bndl, unsigned short rowIndex, matrix pfish, matrix pcov, matrix deriv)
{
	matrix lff, v, *t;
	long oldPos[mMaxDims], nRows, nCols, i, j, repeat;

/*	if deriv is NULL, let's assume it's the identity matrix (params.deriv) */
	if(deriv) lff = m_normalize(m_mult(mNewMatrix, pcov, deriv), 1);
	else lff = m_normalize(m_copy(mNewMatrix, pcov), 1);

	nRows = m_getsize(bndl->sim, 1);
	nCols = m_getsize(bndl->sim, 2);

	v = m_new(mNewData, m2D, 1, nCols);
	for(repeat = 0; repeat < 2; repeat++) {
		if(repeat == 0) {
			t = &bndl->t1;
			if(deriv) m_hessian(v, deriv, pcov);
			else m_diag(v, pcov);
		}
		else {
			t = &bndl->t2;
			m_hessian(v, lff, pfish);
			if(m_first(v)) do m_val(v) = 1.0 / m_val(v); while(m_next(v));
		}
		if(rowIndex == 0) {
			if((*t)->vals == NULL) m_allocate(*t);
			m_first(*t);
			m_first(v);

			for(j = 0; j < nCols; j++) {
				for(i = 0; i < nRows; i++) {
					m_val(*t) = m_val(v);
					m_next(*t);
				}
				m_next(v);
			}
		}
		else {
			m_getpoint(bndl->sim, oldPos);
			m_setpoint(bndl->sim, rowIndex - 1, 0);
			m_setpoint(*t, rowIndex - 1, 0);
			m_first(bndl->est); m_first(v);
			for(j = 0; j < nCols; j++) {
				m_val(*t) = m_val(bndl->est) + (m_val(bndl->sim) - m_val(bndl->est)) * sqrt(m_val(*t) / m_val(v));
				/* the studentized difference (boot-est) is ADDED to the estimate in this case, not
				subtracted. This means it is backwards. MATLAB routines subsequently perform the
				reflection 2 * est - boot, which is analogous to the way basic bootstrap limits
				are obtained from the raw bootstrap percentile distribution. It is not recommended
				that the bootstrap-t be used: its implementation here is undocumented and admittedly
				a tad bizarre. */
				m_step(bndl->sim, 2, 1); m_step(*t, 2, 1);
				m_next(bndl->est); m_next(v);
			}
			m_setpoint(bndl->sim, oldPos);
		}
	}
	m_free(v);
	m_free(lff);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

#endif /* __PSIGNIFIT_C__ */
