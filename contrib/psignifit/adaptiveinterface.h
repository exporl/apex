#ifndef __ADAPTIVEINTERFACE_H__
#define __ADAPTIVEINTERFACE_H__


extern void * gAdaptPtr;
extern matrix gAdaptiveOutput, gAdaptiveTarget;

void CAdaptiveCleanup(void);
int CAdaptiveFitCore(double *pIn, double *pOut, boolean *pFree);
void CDoAdaptive(DataSetPtr uncollated, DataSetPtr collated);
void CReportAdaptiveProcedure(void);
void *CSetUpAdaptiveProcedure(char *method, unsigned short nParams, double *params, double *lims);
void CSetAdaptiveGeneratingFunction(PsychDistribFuncPtr shape,  double *params);
void CSetUpAdaptiveOutput(unsigned short nRuns);

#define ADAPTIVE_ENABLED

#endif /* __ADAPTIVEINTERFACE_H__ */
