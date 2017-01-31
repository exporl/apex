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
#ifndef __PSIGNIFIT_H__
#define __PSIGNIFIT_H__

#include "mathheader.h"
#include "batchfiles.h"
#include "matrices.h"
#include "psychometric.h"
#include "priors.h"

#define PSIGNIFIT_VERSION		"2.5.6"

#define kNumberOfParams			4
#define kNumberOfStats			3
#define kMaxParamNameLength		9

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned short nPoints;
	double * x;
	double * nRight;
	double * nWrong;
} DataSet;
typedef DataSet * DataSetPtr;
#define NULL_DATA_SET		{0, NULL, NULL, NULL}

typedef struct {
	char name[kMaxParamNameLength+1];
	boolean free;
	Constraint constraint;
	double guess;
	double fitted;
/* do not include pointers without checking a=b assignments in code */
} Param;
typedef Param * ParamPtr;

typedef struct {
	Param						theta[kNumberOfParams];
	PsychDistribFuncPtr			shape;
	unsigned short				nIntervals;
	Constraint					tailConstraint;
	Constraint					shiftConstraint;
	Constraint					slopeConstraint;
	double						xValAtChance;
	boolean						convertParams;
	double						fixedShift;
	double						fixedSlope;
/* do not include pointers without checking a=b assignments in code */
} Model;
typedef Model * ModelPtr;

typedef struct {
	PsychDistribFuncPtr	shape;
	double params[kNumberOfParams];
	boolean gotParams;
	long randomSeed;
	unsigned long run;
	unsigned long nRuns;
	double * psi;
	unsigned short nPoints;
} GeneratingInfo;
typedef GeneratingInfo * GeneratingInfoPtr;

typedef struct{
	matrix est;
	matrix sim;
	matrix cpe;
	matrix deriv;
	matrix lff;
	matrix bc;
	matrix acc;
	matrix lims;
	matrix quant;
	
	matrix t1;
	matrix t2;
} MatrixBundle;

typedef struct {
	MatrixBundle params;
	MatrixBundle stats;
	MatrixBundle thresholds;
	MatrixBundle slopes;
	matrix ySim;
	matrix rSim;
	matrix ldot;
	matrix randomSeed;
	matrix fisher;
	matrix pcov;

	matrix sensParams;
	matrix inRegion;
	
	matrix dataExport;
	unsigned int dataExportIndex;
	
	boolean logSlopes;
	boolean cutPsi;

	double *cuts;
	unsigned int nCuts;

	double *conf;
	unsigned int nConf;

	boolean doParams;
	boolean doStats;
	boolean refit;
	double sensCoverage;
	unsigned short sensNPoints;
	boolean verbose;
	char numericFormat[mNumericFormatLength + 1];
} Output;
typedef Output * OutputPtr;

typedef enum {unknown_format, xyn, xrw, xrn} DataFormat;

typedef enum {ts2ab, ab2ts} Translation;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

void AllocateDataSet(DataSetPtr theSet, short nPoints);
void AllocateMatrixBundle(MatrixBundle *bndl, boolean doBCA);
void AssignOutput(matrix m, BatchPtr batch, char *ident, char *extn, char *writeFormat);
void BCATerms(MatrixBundle *bndl, matrix ldot);
void BootstrapT(ModelPtr model, double *params, DataSetPtr data, OutputPtr out, unsigned short rowIndex);
double CalculateDeviance(DataSetPtr data, double *expected);
void CheckModel(ModelPtr model, boolean checkFreeParams);
boolean CheckParams(PsychDistribFuncPtr shape, double *params, char *errFmt, ...);
void CleanUp(DataSetPtr data, ModelPtr model, GeneratingInfoPtr gen, OutputPtr out);
void CollateData(DataSetPtr dest, DataSetPtr src, double * alsoCollate);
DataSetPtr ConstructDataSet(int nPoints, int rowSkip, double *x, double *y, double *n, double *r, double *w, char *sourceDescription);
int Core(DataSetPtr data, ModelPtr model, GeneratingInfoPtr gen, OutputPtr out);
short CountFreeParams(ModelPtr model);
matrix CPE(matrix cpe, matrix est, matrix sim);
void Derivs(matrix tDeriv, matrix sDeriv, ModelPtr model, PsychDistribFuncPtr shape, double *params, unsigned short nCuts, double *cuts);
double * DevianceResiduals(double *rBuffer, double *expected, DataSetPtr data, double *deviance);
double DiffLogAllPriors(ModelPtr model, double *p, ArgIdentifier wrt);
double DiffLoglikelihood(PsychDistribFuncPtr shape, double *params, ArgIdentifier wrt, DataSetPtr data, ModelPtr model);
double DiffPsi(PsychDistribFuncPtr shape, double *params, double x, double *returnPsi, ArgIdentifier wrt);
void DisposeDataSet(DataSetPtr theSet);
void DoStats(double *predicted, DataSetPtr data, double *chronIndex,
			 double *returnDeviance, double *returnPRCorr, double *returnKRCorr,
			 double *rSpace, double *kSpace);
void DuplicateDataSet(DataSetPtr dest, DataSetPtr src);
matrix ExpectedFisher(matrix m, PsychDistribFuncPtr shape, double *params, DataSetPtr data, ModelPtr model);
double * ExpectedValues(double *expected, unsigned short nPoints, double *x,
				PsychDistribFuncPtr shape, double *params, char * description);
void ExportDataSet(DataSetPtr data, matrix m, double * chronIndex);
void FakeFit(ModelPtr model, double *paramsOut,
				unsigned short nPoints, double *srcX, double *srcPsi,
				PsychDistribFuncPtr srcShape, double *srcParams);
void FindSensParams(matrix sensParams, matrix insideMatrix, matrix pSim,
					unsigned short nPoints, double coverage,
					DataSetPtr data, ModelPtr model, GeneratingInfoPtr gen);
int FitCore(double *pIn, double *pOut, boolean *pFree);
short FitPsychometricFunction(DataSetPtr data, ModelPtr model, double *paramsOut, boolean verbose);
void FixParam(Param theta[], short whichParam, double value);
void FreeParam(Param theta[], short whichParam);
void GenerateFakeDataSet(DataSetPtr data1, double *expected1, double *nObs1,
						 DataSetPtr data2, double *expected2, double *nObs2);
void get_mlmt_info(DataSetPtr *data, ModelPtr *model, boolean *treatABasTS);
int GuessParams(DataSetPtr data, ModelPtr model);
void InitMatrixBundle(MatrixBundle *bndl, GeneratingInfoPtr gen, OutputPtr out,
					  long nCols, boolean valid, boolean doLimits, boolean bcaPossible,
					  char *identStem, BatchPtr batch);
void InitParam(ModelPtr model, short paramNumber, char * paramName);
void InitPrefs(BatchPtr prefs, ModelPtr * handleForModel,
							   DataSetPtr * handleForData,
							   GeneratingInfoPtr * handleForGeneratingInfo,
							   OutputPtr * handleForOutput,
							   matrix externalData);
void Limits(MatrixBundle *bndl, double *conf, unsigned short nConf);
BatchPtr LoadPrefs(char *fileName, char *localString, size_t localLength, boolean disposeable);
void MagicMesh(ModelPtr model, unsigned short nSteps, unsigned short nIterations,
				double *min, double *max, double (*function)(double * params));
void MagicMeshPoint(unsigned short stepNumber, unsigned short stepsPerDimension,
					double * p, boolean *pfree, double * min, double * max);
double MakeLegal(PsychDistribFuncPtr shape, ArgIdentifier wrt, double val);
void MakeLimitsLegal(SearchLimitsPtr lims, ConstraintPtr con, double absoluteMin, double absoluteMax);
PsychDistribFuncPtr MatchShape(char *buf, char *desc);
double mlmt(double * p);
void MonteCarlo(DataSetPtr originalData, ModelPtr model, GeneratingInfoPtr gen, OutputPtr out);
void MonteCarloGenerators(DataSetPtr data, PsychDistribFuncPtr shape, double *params, double *psi,
						double **predicted, double **nObs);
double Priors(ModelPtr model, double *pIn, double *pOut, boolean paramsConverted);
void PsychSetSimplexSizes(DataSetPtr data, PsychDistribFuncPtr shape, double * guess, boolean useTSUnits);
double Quantile(double conf, double *orderedVals, long nVals);
void ReportDataSet(DataSetPtr data);
void ReportModel(ModelPtr model);
int SelfTest(void);
void set_mlmt_info(DataSetPtr data, ModelPtr model, boolean treatABasTS);
void ThresholdAndSlope(PsychDistribFuncPtr shape, double * params, double cut, double *thPtr, double *slPtr, ArgIdentifier wrt);
int TransformedRegression(unsigned short nPoints, double *x, double *y, double *weights,
		PsychDistribFuncPtr shape, double *alpha, double *beta, double gamma, double lambda);
void TranslateAB(PsychDistribFuncPtr shape, double *p, Translation t);
void VarianceEstimates(MatrixBundle *bndl, unsigned short rowIndex, matrix pfish, matrix pcov, matrix deriv);


extern DataFormat gDataFormat;

#ifdef __cplusplus
}       // extern "C"
#endif

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

#endif /* __PSIGNIFIT_H__*/


