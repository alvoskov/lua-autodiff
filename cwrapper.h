/*
 * MATLAB-style matrices for LUA 5.3
 * http://pgl.yoyo.org/luai/i/lua_settable
 *
 * (C) 2016-2017 Alexey Voskov
 */

#ifndef __CWRAPPER_H
#define __CWRAPPER_H
#define LUAFUNC_BUFSIZE 512

typedef struct {
	void *LuaState;	
	int userFlags;
	char errMsg[LUAFUNC_BUFSIZE];
} LuaFunc;

int LuaFunc_Init(LuaFunc *F, const char *filename);
int LuaFunc_Eval(LuaFunc *F, double *b, int m);
int LuaFunc_GetValueLength(LuaFunc *F);
int LuaFunc_GetValue(LuaFunc *F, double *res, double *J);
void LuaFunc_Close(LuaFunc *F);
const char *LuaFunc_GetErrMsg(LuaFunc *F);

#endif

