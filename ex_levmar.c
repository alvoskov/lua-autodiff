/*
 * ex_levmar.c An example of joint usage of levmar library and
 * dual numbers library (mlsmat.c and mlslib.lua) for nonlinear
 * regression.
 *
 * Two binary variants of this program are linked:
 * - ex_levmar.exe -- dynamic linking with Lua and dual number library
 * - ex_levmar_static.exe -- static linking with Lua and dual number library
 * Note: levmar is linked statically in all cases.
 *
 * (C) 2016-2017 Alexey Voskov (alvoskov@gmail.com)
 * License: MIT (X11) license
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#include "cwrapper.h"
#include "levmar.h"

/* Stub for levmar (residuals) */
void func(double *param, double *res, int m, int n, void *adata)
{
	LuaFunc *F = (LuaFunc *) adata;
	F->userFlags = 1;
	if (LuaFunc_Eval(F, param)) {
		if (LuaFunc_GetValue(F, res, NULL) == 0) {
			printf("func: %s\n", LuaFunc_GetErrMsg(F));
		}
	} else {
		printf("func :%s\n", LuaFunc_GetErrMsg(F));
	}
}

/* Stub for levmar (Jacobian) */
void jacf(double *param, double *J, int m, int n, void *adata)
{
	LuaFunc *F = (LuaFunc *) adata;
	if (F->userFlags || LuaFunc_Eval(F, param)) {
		if (LuaFunc_GetValue(F, NULL, J) == 0) {
			printf("jacf: %s\n", LuaFunc_GetErrMsg(F));
		}
		F->userFlags = 0;
	} else {
		printf("jacf :%s\n", LuaFunc_GetErrMsg(F));
	}
}

/* Program entry point */
int main(int argc, const char *argv[])
{
	double *beta, *covar;
	LuaFunc LF;
	if (argc != 2) {
		printf("Levenberg-Marquardt method for user-defined functions written in Lua.\n");
		printf("(C) 2016-2017 Alexey Voskov (alvoskov@gmail.com)\n");
		printf("Usage:\n");
		printf("  ex_levmar func.lua");
		return 0;
	}
	/* Load user-defined function */
	const char *filename = argv[1];
	if (LuaFunc_Init(&LF, filename) == 0) {
		printf("Error during initialization: %s\n", LuaFunc_GetErrMsg(&LF));
		return 1;
	}
	/* Type initial approximation */
	printf("Initial approxmiation: ");
	for (int i = 0; i < LF.nparams; i++) {
		printf("%10g", LF.beta0[i]);
	}
	printf("\n");
	int m = LF.nparams;
	beta = (double *) calloc(m, sizeof(double));
	covar = (double *) calloc(m * m, sizeof(double));
	/* Try to evaluate it */
	if (LuaFunc_Eval(&LF, LF.beta0) == 0) {
		printf("Error during function evaluation: %s\n", LuaFunc_GetErrMsg(&LF));
		return 1;
	}
	int n = LuaFunc_GetValueLength(&LF);
	if (n == -1) {
		printf("Error during getting number of points: %s\n", LuaFunc_GetErrMsg(&LF));
		return 1;		
	}
	/* Run optimizer (from levmar library) */
	memcpy(beta, LF.beta0, LF.nparams * sizeof(double));
	double opts[LM_OPTS_SZ] = {LM_INIT_MU, 1e-15, 1e-15, 1e-20, LM_DIFF_DELTA};
	double info[LM_INFO_SZ];
	dlevmar_der(
		func, jacf, beta, NULL, m, n,
		500, // Itmax
		opts, info, NULL, covar,
		&LF);
	/* Type the result */
	printf("Result:\n");
	printf("%10s %10s\n", "beta", "s(beta)");
	for (int i = 0; i < LF.nparams; i++) {
		printf("%10g %10g\n", beta[i], sqrt((covar[i*m + i])));
	}
	
	/* Close all structs and exit */
	LuaFunc_Close(&LF);
	free(beta);
	free(covar);
	return 0;
}
