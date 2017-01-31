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
#ifndef __ADAPTIVESTUBS_C__
#define __ADAPTIVESTUBS_C__

#define Q_UNUSED(x) (void)x;

#include "universalprefix.h"

#include "psignifit.h"
#include "adaptiveinterface.h"

void * gAdaptPtr;
matrix gAdaptiveOutput, gAdaptiveTarget;

void NoAdaptive(void);
void NoAdaptive(void) {JError("Adaptive procedures not implemented in this release");}

void CAdaptiveCleanup(void)
	{NoAdaptive();}
int CAdaptiveFitCore(double *pIn, double *pOut, boolean *pFree)
        {Q_UNUSED(pIn); Q_UNUSED(pOut); Q_UNUSED(pFree);
         NoAdaptive(); return -1;}
void CDoAdaptive(DataSetPtr uncollated, DataSetPtr collated)
        {Q_UNUSED(uncollated);Q_UNUSED(collated); NoAdaptive();}
void CReportAdaptiveProcedure(void)
	{NoAdaptive();}
void CSetAdaptiveGeneratingFunction(PsychDistribFuncPtr shape, double *params)
        {Q_UNUSED(shape); Q_UNUSED(params); NoAdaptive();}
void CSetUpAdaptiveOutput(unsigned short nRuns)
        {Q_UNUSED(nRuns); NoAdaptive();}
void *CSetUpAdaptiveProcedure(char *method, unsigned short nParams, double *params, double *lims)
{
        Q_UNUSED(nParams);
	if(method != NULL || params != NULL || lims != NULL) NoAdaptive();
	return gAdaptPtr = NULL;
}
#endif /* __ADAPTIVESTUBS_C__ */
