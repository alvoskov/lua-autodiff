/*
 * An example of joint usage of levmar library and dual numbers
 * for nonlinear regression
 *
 * (C) 2016-2017 Alexey Voskov
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#include "cwrapper.h"
#include "levmar.h"

void func(double *param, double *res, int m, int n, void *adata)
{
	LuaFunc *F = (LuaFunc *) adata;
	F->userFlags = 1;
	if (LuaFunc_Eval(F, param, m)) {
		LuaFunc_GetValue(F, res, NULL);
	} else {
		printf("FUNC ERROR");
	}
}

void jacf(double *param, double *J, int m, int n, void *adata)
{
	LuaFunc *F = (LuaFunc *) adata;
	if (F->userFlags || LuaFunc_Eval(F, param, m)) {
		LuaFunc_GetValue(F, NULL, J);
		F->userFlags = 0;
	} else {
		printf("JACF ERROR");
	}
}

int main()
{
	double b0[] = {0.1, 0.1, 0.1};
	double b[3], covar[3*3];
	LuaFunc LF;
	int m = 3;
	/* Load user-defined function */
	if (LuaFunc_Init(&LF, "func.lua") == 0) {
		printf("%s\n", LuaFunc_GetErrMsg(&LF));
		return 1;
	}
	/* Try to evaluate it */
	if (LuaFunc_Eval(&LF, b0, m) == 0) {
		printf("%s\n", LuaFunc_GetErrMsg(&LF));
		return 1;
	}
	int n = LuaFunc_GetValueLength(&LF);
	if (n == -1) {
		printf("%s\n", LuaFunc_GetErrMsg(&LF));
		return 1;		
	}
	/* Run optimizer */
	double opts[LM_OPTS_SZ] = {LM_INIT_MU, 1e-15, 1e-15, 1e-20, LM_DIFF_DELTA};
	double info[LM_INFO_SZ];
	dlevmar_der(
		func, jacf, b0, NULL, m, n,
		500, // Itmax
		opts, info, NULL, covar,
		&LF);
	/* Type the result */
	for (int i = 0; i < m; i++) {
		printf("%f ", b0[i]);
	}
	printf("\n");
	/* Close all structs and exit */	
	LuaFunc_Close(&LF);
	return 0;
}
