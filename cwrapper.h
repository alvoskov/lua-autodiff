/*
 * cwrapper.c  A header file for interface between Lua DualNVector and
 * RealVector classes and C subroutines that hides all Lua structures
 * from the end user.
 *
 * (C) 2016-2017 Alexey Voskov (alvoskov@gmail.com)
 * License: MIT (X11) license
 */
#ifndef __CWRAPPER_H
#define __CWRAPPER_H
#define LUAFUNC_BUFSIZE 512

/* Structure for saving Lua state, error messages, initial approximations etc.*/
typedef struct {
	void *LuaState;	/* Pointer to lua_State structure */
	int userFlags; /* Reseved for arbitrary usage by a user */
	char errMsg[LUAFUNC_BUFSIZE]; /* Buffer */
	double *beta0; /* Initial approximation */
	double nparams; /* Number of parameters*/
} LuaFunc;

#ifdef __cplusplus
#define FEXTERN extern "C"
#else
#define FEXTERN extern
#endif

/* API for user */
int FEXTERN LuaFunc_Init(LuaFunc *F, const char *filename);
int FEXTERN LuaFunc_Eval(LuaFunc *F, double *b);
int FEXTERN LuaFunc_GetValueLength(LuaFunc *F);
int FEXTERN LuaFunc_GetValue(LuaFunc *F, double *res, double *J);
void FEXTERN LuaFunc_Close(LuaFunc *F);
const char FEXTERN *LuaFunc_GetErrMsg(LuaFunc *F);
double FEXTERN *LuaFunc_GetBeta0(LuaFunc *F);
int FEXTERN LuaFunc_GetNParams(LuaFunc *F);
#endif

