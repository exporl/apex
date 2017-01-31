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
#include "universalprefix.h"

#include <stdlib.h>
#include <string.h>

#include "batchfiles.h"
#include "mathheader.h"
#include "matlabtools.h"
#include "matrices.h"
#include "psignifit.h"

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

#ifdef MATLAB_MEX_FILE
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */

boolean LegalArg1(mxArray * arg);
boolean LegalArg2(mxArray * arg);

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean LegalArg1(mxArray * arg) { return (boolean)(!mxIsSparse(arg) && !mxIsComplex(arg) && mxIsDouble(arg) && mxGetM(arg) > 0 && mxGetN(arg) == 3); }
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
boolean LegalArg2(mxArray * arg) { return (boolean)(!mxIsSparse(arg) && !mxIsComplex(arg) && mxIsChar(arg) && mxGetM(arg) == 1 && mxGetN(arg) > 0); }
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
MATLAB_GATEWAY(mexFunction)
{
	mxArray * dataMatrix = NULL, * prefsMatrix = NULL;
	DataSetPtr data;
	ModelPtr model;
	GeneratingInfoPtr gen;
	OutputPtr out;
	BatchPtr prefs;
	size_t n;
	char * prefsBuffer;
	int argsOut;
	boolean doSelfTest = FALSE;
	matrix externalData;
	
	TabulaRasa();
	m_init();

	if(nargin < 1 || nargin > 2) JError("Number of input arguments should be 1 or 2");  
	if(nargout > 5) nargout = 5;	

	mexInitOutputList(nargout, &argsOut, _argout);
	if(nargin == 1 && mxIsChar(argin(1)) && mxGetNumberOfElements(argin(1)) == 2) {
		mxGetString(argin(1), (prefsBuffer = New(char, 10)), 10);
		doSelfTest = (strcmp(prefsBuffer, "-t") == 0);
		Destroy(prefsBuffer);
		if(doSelfTest) {SelfTest(); return;}
	} 
		
	if(LegalArg1(argin(1))) dataMatrix = argin(1);
	if(LegalArg2(argin(1))) prefsMatrix = argin(1);
	if(nargin >= 2) {
		if(dataMatrix==NULL && LegalArg1(argin(2))) dataMatrix = argin(2);
		if(prefsMatrix==NULL && LegalArg2(argin(2))) prefsMatrix = argin(2);
		if(dataMatrix==NULL) JError("data must be a full, real, 3-column matrix of doubles");
		if(prefsMatrix==NULL) JError("prefs must be a string matrix with one row");
	}
	if(dataMatrix == NULL && prefsMatrix == NULL)
		JError("data should be a 3-column double-precision matrix, prefs should be a one-line string");

	if(prefsMatrix == NULL) prefs = 0;
	else {
		prefsBuffer = New(char, (n = mxGetNumberOfElements(prefsMatrix)) + 1);
		mxGetString(prefsMatrix, prefsBuffer, n + 1);
		prefs = BatchString(prefsBuffer, n, TRUE);
	}

	externalData = ((dataMatrix == NULL) ? NULL : m_new(mxGetPr(dataMatrix), 2, mxGetM(dataMatrix), mxGetN(dataMatrix)));
	InitPrefs(prefs, &model, &data, &gen, &out, externalData);
	DisposeBatch(prefs);
	m_free(externalData);
	Core(data, model, gen, out);
	CleanUp(data, model, gen, out);
	MEX_END
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#else
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#include <unistd.h>
int main(int argc, char *argv[])
{
	boolean doSelfTest = FALSE;
	ModelPtr model = NULL;
	DataSetPtr data = NULL;
	GeneratingInfoPtr gen = NULL;
	OutputPtr out = NULL;
	BatchPtr input = NULL;
	int i;
		
	TabulaRasa();	
	m_init();
	if(argv[0]) {
		for(i = 1; i < argc; i++) {
			if(argc == 2 && strcmp(argv[i], "-t") == 0) doSelfTest = TRUE;
			else input = ConcatenateBatchStrings(input, LoadPrefs(argv[i], NULL, 0, 0), TRUE, TRUE);
		}
		for(gExecName = argv[0], i = 0; argv[0] && argv[0][i]; i++) if(argv[0][i] == '/') gExecName = argv[0] + i + 1;
	}
	else {
		gExecName = NULL;
		input = LoadPrefs("prefs", NULL, 0, 0);
	}
	if(!doSelfTest && (input == NULL || !isatty(0))) input = ConcatenateBatchStrings(input, LoadPrefs("stdin", NULL, 0, 0), TRUE, TRUE);	
	if(input != NULL && strncmp(input->buffer, "#data\n-t", strlen("#data\n-t")) == 0) doSelfTest = TRUE;

	if(doSelfTest) {DisposeBatch(input); return SelfTest();}

	InitPrefs(input, &model, &data, &gen, &out, NULL);
	DisposeBatch(input);
	Core(data, model, gen, out);	
	CleanUp(data, model, gen, out);	
	return (EXIT_SUCCESS);
}
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    */
#endif /* MATLAB_MEX_FILE */
