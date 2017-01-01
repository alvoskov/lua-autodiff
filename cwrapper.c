/*
 * MATLAB-style matrices for LUA 5.3
 *
 * (C) 2016-2017 Alexey Voskov
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "mlsmat.h"
#include "cwrapper.h"

#ifdef STATIC_LINK
#include "mlslib_lua.c"
#endif


/*
 * Initializes lua function. The resulting LUA stack is
 * 1: mlslib module
 * 2: user-defined function module (table with initfunc and resfunc fields)
 * 3: DualNVector.new method (constructor)
 * 4: Vec function (RealVector.new method alias)
 */
int LuaFunc_Init(LuaFunc *F, const char *filename)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	F->LuaState = (void *) L;
	F->userFlags = 0;
	char *errmsg = F->errMsg;
#ifdef STATIC_LINK
	/* Load mlslib and mlslib libraries that are embedded into file */
	/* It is designed for static linking with Lua */
	luaopen_mlsmat(L);
	lua_setglobal(L, "__MLSMat");
	if (luaL_loadbuffer(L, mlslib_lua_data, mlslib_lua_len, "mlslib") != 0 || lua_pcall(L, 0, 1, 0) != 0) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "Cannot load libraries (%s)", lua_tostring(L, -1));
		return 0;
	}
#else
	/* Load mlslib.lua and mlsmat.dll libraries from files */
	/* It is designed for dynamic linking with Lua */
	if (luaL_dofile(L, "mlslib.lua") != 0) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "Cannot load libraries (%s)", lua_tostring(L, -1));
		return 0;
	}
#endif
	if (lua_gettop(L) != 1 || lua_type(L, 1) != LUA_TTABLE) {
		printf("%d %d\n", lua_gettop(L), lua_type(L, 1));
		snprintf(errmsg, LUAFUNC_BUFSIZE, "Libraries are corrupted");
		return 0;
	}
	/* Load user-defined script */
	if (luaL_dofile(L, filename) != 0) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "Failed to load user-defined script %s (%s)", filename, lua_tostring(L, -1));
		return 0;
	}
	/* Check script output */
	/* a) data type */
	if (lua_gettop(L) != 2 || lua_type(L, 2) != LUA_TTABLE) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "User script must return a table");
		return 0;
	}
	/* b) fields */
	lua_getfield(L, -1, "initfunc");
	if (lua_isnil(L, -1) || !lua_isfunction(L, -1)) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "initfunc field is absent or doesn't contain function");
		return 0;
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "resfunc");
	if (lua_isnil(L, -1) || !lua_isfunction(L, -1)) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "resfunc field is absent or doesn't contain function");
		return 0;
	}
	lua_pop(L, 1);
	/* Initialize user script */
	lua_getfield(L, -1, "initfunc");
	lua_pushvalue(L, 1); /* Module with dual numbers */
	if (lua_pcall(L, 1, 0, 0) != 0) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "User script initialization failed (%s)", lua_tostring(L, -1));
		return 0;
	}
	/* Get constructor for dual numbers */
	lua_getfield(L, 1, "DualNVector");
	if (lua_isnil(L, -1)) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "DualNVector class is absent");
		return 0;
	}
	lua_getfield(L, -1, "new");
	if (lua_isnil(L, -1)) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "DualNVector.new method is absent");
		return 0;
	}	
	lua_remove(L, -2); /* Remove DualNVector table from stack */
	/* Get constructor for real numbers */
	lua_getfield(L, 1, "Vec");
	if (lua_isnil(L, -1)) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "Vec function is absent");
		return 0;
	}
	return 1;
}

/*
 * Evaluates Lua function. The resulting Lua stack is:
 * 1-4: initLuaFunc output
 * 5: DualNVector result
 * 
 * Returns 1 in the case of success or 0 in the case of error
 */
int LuaFunc_Eval(LuaFunc *F, double *b, int m)
{
	lua_State *L = (lua_State *) F->LuaState;
	char *errmsg = F->errMsg;
	/* Get lua residuals function from the table */
	lua_getfield(L, 2, "resfunc");
	/* Construct beta vector */
	lua_pushvalue(L, 3); /* DualVectorN.new copy */
	/* a) real part */
	lua_pushvalue(L, 4); /* Vec copy */
	lua_newtable(L);
	for (int i = 0; i < m; i++) {
		lua_pushnumber(L, i + 1); /* Key */
		lua_pushnumber(L, b[i]); /* Value */
		lua_settable(L, -3);
	}
	if (lua_pcall(L, 1, 1, 0) != 0) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "%s", lua_tostring(L, -1));
		return 0;
	}
	/* b) imaginary parts */
	for (int i = 1; i <= m; i++) {
		lua_pushvalue(L, 4); /* Vec copy */
		lua_newtable(L);
		for (int j = 1; j <= m; j++) {
			lua_pushnumber(L, j); /* Key */
			lua_pushnumber(L, (i == j) ? 1.0 : 0.0); /* Value */
			lua_settable(L, -3);
		}
		if (lua_pcall(L, 1, 1, 0) != 0) {
			snprintf(errmsg, LUAFUNC_BUFSIZE, "%s", lua_tostring(L, -1));
			return 0;
		}
	}
	/* c) DualVector.new constructor call */
	if (lua_pcall(L, m + 1, 1, 0) != 0) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "%s", lua_tostring(L, -1));
		return 0;
	}
	/* Call resfunc */
	if (lua_pcall(L, 1, 1, 0) != 0) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "%s", lua_tostring(L, -1));
		return 0;
	}
	/* Check the type */
	if (lua_type(L, -1) != LUA_TTABLE) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "resfunc must return a table\n");
		return 0;
	}
	lua_getfield(L, -1, "__len");
	if (lua_isnil(L, -1)) {
		snprintf(errmsg, LUAFUNC_BUFSIZE, "ERROR: __len method not found\n");
		return 0;
	}
	lua_pop(L, 1);
	return 1;
}

int LuaFunc_GetValueLength(LuaFunc *F)
{
	lua_State *L = (lua_State *) F->LuaState;
	/* Call __len method */
	lua_getfield(L, -1, "__len");
	if (lua_isnil(L, -1)) {
		printf("ERROR: __len method not found\n");
		return -1;
	}
	lua_pushvalue(L, -2); /* self */
	if (lua_pcall(L, 1, 1, 0) != 0) {
		printf("%s\n", lua_tostring(L, -1));
		return -1;
	}
	/* And convert it to number */
	int len = (int) lua_tonumber(L, -1);
	lua_pop(L, 1);
	return len;
}

/*
 * res: residuals
 * J: [dF(x1)/dB1...dF(x1)/dBm, ..., dF(xn)/dB1...dF(xn)/dBm]
 */
int LuaFunc_GetValue(LuaFunc *F, double *res, double *J)
{
	lua_State *L = (lua_State *) F->LuaState;
	/* Get length (number of elements) */
	int n = LuaFunc_GetValueLength(F);
	if (n == -1) {
		return -1;
	}
	/* Prepare real part (values) */
	if (res != NULL) {
		lua_getfield(L, -1, "real");
		RealVector *rv = (RealVector *) luaL_testudata(L, -1, "MLSMat::RealVector");
		if (rv == NULL) {
			return -1;
		}
		for (int i = 0; i < n; i++) {
			res[i] = rv->data[i + 1];
		}
		lua_pop(L, 1);
	}
	/* Prepare imaginary part (derivatives) */
	if (J != NULL) {
		lua_getfield(L, -1, "imag");
		if (lua_type(L, -1) != LUA_TTABLE) {
			return -1;
		}
		int m = lua_rawlen(L, -1);
		for (int j = 0; j < m; j++) {
			/* Get vector with imaginary part (i.e. dBj derivatives) */
			lua_pushnumber(L, j + 1);
			lua_gettable(L, -2);
			RealVector *iv = (RealVector *) luaL_testudata(L, -1, "MLSMat::RealVector");
			if (iv == NULL) {
				return -1;
			}
			/* Copy the vector */
			for (int i = 0; i < n; i++) {
				J[m*i + j] = iv->data[i + 1];
			}
			/* Restore the stack */
			lua_pop(L, 1);
		}
	}
	return 0;
}

void LuaFunc_Close(LuaFunc *F)
{
	lua_close((lua_State *) F->LuaState);
}

const char *LuaFunc_GetErrMsg(LuaFunc *F)
{
	return F->errMsg;
}

