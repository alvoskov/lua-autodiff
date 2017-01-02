/*
 * mlsmat.c  MATLAB-style vectors for Lua 5.3. This file is designed
 * as dynamic library (module) for Lua containing implementation of
 * several classes:
 *   RealVector -- Vector of doubles
 *   IndexRange -- Index ranges for RealVector
 * It also initializes empty DualNVector table (that is reseved
 * for dual numbers implemented in mlslib.lua)
 * 
 * This module also can be linked statically
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
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "mlsmat.h"

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

/*========== IndexRange class ==========*/
/*
 * obj = IndexRange.new()
 * obj = IndexRange.new(a, b)
 * obj = IndexRange.new(a, step, b);
 */
static int indexrange_new(lua_State *L)
{
	int nargin = lua_gettop(L), a, b, step;
	/* Check and get input arguments */
	if (nargin == 2) { /* Range with default step 1 */
		a = luaL_checkinteger(L, 1);
		b = luaL_checkinteger(L, 2);
		step = 1;
	} else if (nargin == 3) { /* Range with user defined step */
		a = luaL_checkinteger(L, 1);
		step = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		if (step == 0) {
			luaL_error(L, "Step cannot be equal to zero");
		}
	} else if (nargin == 0) { /* All elements range */
		a = 1;
		b = -1;
		step = 1;
	} else {
		luaL_error(L, "Invalid number of arguments");
	}
	if (a == 0) {
		luaL_error(L, "Invalid value of a");
	}
	if (b == 0) {
		luaL_error(L, "Invalid value of b");
	}
	/* Create data structure */
	IndexRange *inds = (IndexRange *) lua_newuserdata(L, sizeof(IndexRange));
	luaL_getmetatable(L, "MLSMat::IndexRange");
	lua_setmetatable(L, -2);
	inds->a = a;
	inds->b = b;
	inds->step = step;
	return 1;
}

static int indexrange_tostring(lua_State *L)
{
	static char buf[256];
	IndexRange *inds = (IndexRange *) luaL_checkudata(L, 1, "MLSMat::IndexRange");
	sprintf(buf, "<IndexRange: %d:%d:%d>", inds->a,  inds->step, inds->b);
	lua_pushstring(L, buf);
	return 1;
}


static const struct luaL_Reg indexrange_funcs[] = {
	{"new", indexrange_new},
	{"__tostring", indexrange_tostring},
	{NULL, NULL}
};


/*========== RealVector class ==========*/
static RealVector *c_realvector_create(lua_State *L, int len)
{
	/* Create class example (initialize properties) */
	lua_newtable(L);
	/* a) array memory */
	lua_pushstring(L, "data");
	RealVector *vec = (RealVector *) lua_newuserdata(L, sizeof(RealVector));
	vec->len = len;
	vec->data = calloc(len + 1, sizeof(double)); /* +1 -- to provide 1-based indices */
	/* b) set metatable */
	luaL_getmetatable(L, "MLSMat::RealVector");
	lua_setmetatable(L, -2);
	return vec;
}

static int realvector_rand(lua_State *L)
{
	if (lua_gettop(L) != 1) {
		luaL_error(L, "Invalid number of input arguments");
	}
	if (lua_isinteger(L, 1)) {
		/* Create array of U[0;1] of required size */
		int len = luaL_checkinteger(L, 1);
		luaL_argcheck(L, len >= 0, 1, "Invalid size");
		RealVector *vec = c_realvector_create(L, len);
		double *out = vec->data + 1;
		for (int i = 1; i <= vec->len; i++) {
			*out++ = (double) rand() / RAND_MAX;
		}
	} else {
		luaL_error(L, "Input argument must be an integer value");
	}
	return 1;
}

static int realvector_randn(lua_State *L)
{
	if (lua_gettop(L) != 1) {
		luaL_error(L, "Invalid number of input arguments");
	}
	if (lua_isinteger(L, 1)) {
		/* Create array of N(0;1) of required size */
		int len = luaL_checkinteger(L, 1);
		luaL_argcheck(L, len >= 0, 1, "Invalid size");
		RealVector *vec = c_realvector_create(L, len);
		double *out = vec->data + 1;
		for (int i = 1; i <= vec->len; i++) {
			double u1 = (double) rand() / RAND_MAX;
			double u2 = (double) rand() / RAND_MAX;			
			*out++ = sqrt(-2*log(u1)) * cos(2*M_PI*u2);
		}
	} else {
		luaL_error(L, "Input argument must be an integer value");
	}
	return 1;
}


static int realvector_new(lua_State *L)
{
	/* Check input argument (one integer that sets size of array) */
	if (lua_gettop(L) != 1) {
		luaL_error(L, "Invalid number of input arguments");
	}
	if (lua_isinteger(L, 1)) {		
		/* Create zero-filled array of required size */
		int len = luaL_checkinteger(L, 1);
		luaL_argcheck(L, len >= 0, 1, "Invalid size");
		(void) c_realvector_create(L, len);
	} else if (lua_istable(L, 1)) {
		/* Create vector from an array */
		int len = 0;
		/* a) check all data and define vector size */
		lua_pushnil(L);
		while (lua_next(L, 1) != 0) {
			if (!lua_isinteger(L, -2)) {
				lua_pushstring(L, "Input table contains not integer key");
				lua_error(L);
			}
			int ind = lua_tointeger(L, -2);
			if (ind < 1) {
				lua_pushstring(L, "Invalid value of integer key");
				lua_error(L);
			} else if (ind > len) {
				len = ind;
			}
			if (!lua_isnumber(L, -1)) {
				lua_pushstring(L, "Input table contains not numeric value");
				lua_error(L);
			}
			lua_pop(L, 1); /* Remove value from stack */
		}
		if (len == 0) {
			lua_pushstring(L, "Vector cannot be empty");			
		}
		/* b) create matrix and fill it */
		RealVector *vec = c_realvector_create(L, len);
		lua_pushnil(L);
		while (lua_next(L, 1) != 0) {
			int ind = lua_tointeger(L, -2);
			double val = lua_tonumber(L, -1);
			vec->data[ind] = val;
			lua_pop(L, 1); /* Remove value from stack */
		}
	} else {
		luaL_error(L, "Input argument must be either integer or table");
	}
	return 1;
}

static int realvector_gc(lua_State *L)
{
	RealVector *vec = luaL_checkudata(L, -1, "MLSMat::RealVector");
	free(vec->data);
	return 0;
}

typedef struct {
	/* Raw data */
	int flags;
	union {
		double val;
		RealVector *vec;
	} arg1;
	union {
		double val;
		RealVector *vec;
	} arg2;
	/* Some additional information */
	double val; /* Scalar value (if applicable, see flags) */
	RealVector *vec; /* Vector value (if applicable, see flags) */
	RealVector *resvec; /* Resulting vector */
} BinOpArgInfo;

/*
 * Checks types of arguments and returns BinOpArgInfo struct with the next
 * possible values in flags field:
 *   1 -- 1st argument is double, 2nd argument is RealVector
 *   2 -- 1st argument is RealVector, 2nd argument is double
 *   3 -- 1st and 2nd arguments are RealVectors
 */
static BinOpArgInfo realvector_binop_arginfo(lua_State *L)
{
	BinOpArgInfo res;
	res.flags = 0;
	/* Testing 1st argument */
	if ((res.arg1.vec = (RealVector *) luaL_testudata(L, 1, "MLSMat::RealVector")) != NULL) {
		res.vec = res.arg1.vec;
		res.flags |= 2;
	} else {
		res.arg1.val = luaL_checknumber(L, 1);
		res.val = res.arg1.val;
	}
	/* Testing 2nd argument */
	if ((res.arg2.vec = (RealVector *) luaL_testudata(L, 2, "MLSMat::RealVector")) != NULL) {
		res.vec = res.arg2.vec;
		res.flags |= 1;
	} else {
		if (lua_type(L, 2) == LUA_TNUMBER) {
			/* 2nd argument is a number */
			res.arg2.val = luaL_checknumber(L, 2);
			res.val = res.arg2.val;
		} else if (lua_type(L, 2) == LUA_TTABLE) {
			/* 2nd argument is a class: readdress to its metatable */
			/* Get information about the function */
			lua_Debug ar;
			lua_getstack(L, 0, &ar);
			lua_getinfo(L, "n", &ar);
			const char *fname = ar.name;
			/* Get binary operation method (from metatable of 2nd argument) */
			if (lua_getmetatable(L, 2) == 0) {
				luaL_error(L, "bad argument #2 to %s: table must have a metatable", fname);
			}
			lua_getfield(L, 3, fname);
			if (lua_isnil(L, 4)) {
				luaL_error(L, "bad argument #2 to %s: cannot readdress", fname);
			}
			/* Call metamethod */
			lua_pushvalue(L, 1);
			lua_pushvalue(L, 2);
			lua_remove(L, 1);
			lua_remove(L, 1);
			lua_remove(L, 1);
			lua_call(L, 2, 1);
			res.flags |= 128;
			return res; /* To prevent preallocations of RealVector */
		} else {
			luaL_error(L, "bad argument #2");
		}
	}
	/* Check sizes of arguments */
	if (res.flags == 3) {
		int v1len = res.arg1.vec->len, v2len = res.arg2.vec->len;
		if (v1len != 1 && v2len == 1) { /* V2 is scalar */
			res.vec = res.arg1.vec;
			res.val = res.arg2.vec->data[1];
			res.flags = 2;
		} else if (v1len == 1 && v2len != 1) { /* V1 is scalar */
			res.val = res.arg1.vec->data[1];
			res.vec = res.arg2.vec;
			res.flags = 1;
		} else if (v1len != v2len) { /* Both are vectors: check sizes */
			luaL_error(L, "RealVector sizes are mismatching");
		} 
	}
	/* Preallocate output vector */
	if (res.flags == 3) {
		res.resvec = c_realvector_create(L, res.arg1.vec->len);
	} else {
		res.resvec = c_realvector_create(L, res.vec->len);
	}
	/* Return info about arguments types */
	return res;
}

#define REALVECTOR_BINOP_BODY(op) \
{ \
	BinOpArgInfo ai = realvector_binop_arginfo(L); \
	if (ai.flags > 32) { \
		return 1; \
	} \
	double *in1 = ai.vec->data + 1, *in2, *out = ai.resvec->data + 1; \
	if (ai.flags == 2) { \
		for (int i = 1; i <= ai.vec->len; i++) \
			*out++ = *in1++ op ai.val; \
	} else if (ai.flags == 1) { \
		for (int i = 1; i <= ai.vec->len; i++) \
			*out++ = ai.val op *in1++; \
	} else if (ai.flags == 3) { \
		in1 = ai.arg1.vec->data + 1; in2 = ai.arg2.vec->data + 1; \
		for (int i = 1; i <= ai.vec->len; i++) \
			*out++ = *in1++ op *in2++; \
	} \
	return 1; \
}

static int realvector_add(lua_State *L)
REALVECTOR_BINOP_BODY(+)

static int realvector_sub(lua_State *L)
REALVECTOR_BINOP_BODY(-)

static int realvector_mul(lua_State *L)
REALVECTOR_BINOP_BODY(*)

static int realvector_div(lua_State *L)
REALVECTOR_BINOP_BODY(/)

static int realvector_pow(lua_State *L)
{
	BinOpArgInfo ai = realvector_binop_arginfo(L);
	if (ai.flags > 32) {
		return 1;
	}
	double *in1 = ai.vec->data + 1, *in2, *out = ai.resvec->data + 1;
	if (ai.flags == 2) {
		for (int i = 1; i <= ai.vec->len; i++)
			*out++ = pow(*in1++, ai.val);
	} else if (ai.flags == 1) {
		for (int i = 1; i <= ai.vec->len; i++)
			*out++ = pow(ai.val, *in1++);
	} else if (ai.flags == 3) {
		in1 = ai.arg1.vec->data + 1; in2 = ai.arg2.vec->data + 1;
		for (int i = 1; i <= ai.vec->len; i++)
			*out++ = pow(*in1++, *in2++);
	}
	return 1;
}

#define REALVECTOR_UNOP_BODY(op) \
{ \
	RealVector *vec = (RealVector *) luaL_testudata(L, -1, "MLSMat::RealVector"); \
	int len = vec->len; double *in = vec->data + 1, *out = (c_realvector_create(L, len))->data + 1; \
	for (int i = 1; i <= len; i++) *out++ = op(*in++); \
	return 1; \
}


static int realvector_unm(lua_State *L)
REALVECTOR_UNOP_BODY(-)

static int realvector_abs(lua_State *L)
REALVECTOR_UNOP_BODY(fabs)

static int realvector_exp(lua_State *L)
REALVECTOR_UNOP_BODY(exp)

static int realvector_log(lua_State *L)
REALVECTOR_UNOP_BODY(log)

static int realvector_sqrt(lua_State *L)
REALVECTOR_UNOP_BODY(sqrt)


static int realvector_totable(lua_State *L)
{
	if (lua_gettop(L) != 1) {
		luaL_error(L, "Invalid number of arguments");
	}
	RealVector *vec = luaL_checkudata(L, -1, "MLSMat::RealVector");
	lua_newtable(L);
	for (int i = 1; i <= vec->len; i++) {
		lua_pushinteger(L, i);
		lua_pushnumber(L, vec->data[i]);
		lua_settable(L, -3);
	}
	return 1;
}

static int realvector_tostring(lua_State *L)
{
	char buf[64];
	RealVector *vec = (RealVector *) luaL_checkudata(L, -1, "MLSMat::RealVector");
	char *result = (char *) calloc(32 + vec->len * 20, sizeof(char));
	
	result[0] = 0;
	sprintf(buf, "RealVector: %d elements\n  ", vec->len); strcat(result, buf);
	for (int i = 0; i < vec->len; i++) {
		if (i % 5 == 0 && i > 0) {
			strcat(result, "\n  ");
		}
		sprintf(buf, "%12.5g ", vec->data[i+1]); strcat(result, buf);
	}
	strcat(result, "\n");
	lua_pushstring(L, result);
	free(result);
	return 1;
}

static int realvector_getvalue(lua_State *L)
{	/* Get data and check array index */
	IndexRange *inds_ptr;
	RealVector *vec = (RealVector *) luaL_checkudata(L, -2, "MLSMat::RealVector");
	if (lua_isinteger(L, -1)) {
		/* Variant 1: integer index */
		int ind = luaL_checkinteger(L, -1);
		luaL_argcheck(L, 1 <= ind && ind <= vec->len, 2, "Index is out of boundaries");
		/* Return value */	
		lua_pushnumber(L, vec->data[ind]);
	} else if (lua_isstring(L, -1)) {
		/* Variant 2: string index, return method from metatable */
		luaL_getmetatable(L, "MLSMat::RealVector");
		lua_pushvalue(L, -2);
		lua_gettable(L, -2);
	} else if ((inds_ptr = (IndexRange *) luaL_testudata(L, -1, "MLSMat::IndexRange")) != NULL) {
		/* Variant 3: user-defined range */
		/* a) Render range limits */
		IndexRange inds = *inds_ptr;
		if (inds.a < 0) inds.a = vec->len + inds.a + 1;
		if (inds.b < 0) inds.b = vec->len + inds.b + 1;
		if (inds.a < 1 || inds.b < 1) luaL_error(L, "bad arguments #1 to '__index': invalid IndexRange range");
		/* b) Find number of elements and copy data */
		int reslen = 0, ibegin, iend;
		if (inds.a < inds.b) {
			ibegin = inds.a; iend = inds.b;
		} else {
			ibegin = inds.b; iend = inds.a;
		}
		if ((inds.a < inds.b && inds.step > 0) || (inds.a > inds.b) && inds.step < 0) {
			for (int i = inds.a; ibegin <= i && i <= iend; i += inds.step) {
				reslen++;
			}
		}
		RealVector *resvec = (RealVector *) c_realvector_create(L, reslen);
		double *in = vec->data, *out = resvec->data + 1;
		for (int i = inds.a; ibegin <= i && i <= iend; i += inds.step) {
			*out++ = in[i];
		}
	} else {
		luaL_error(L, "bad argument #1 to '__index' (number, string or IndexRange expected)");
	}
	return 1;
}

static int realvector_setvalue(lua_State *L)
{	/* Get data and check array index */
	RealVector *vec = (RealVector *) luaL_checkudata(L, -3, "MLSMat::RealVector");
	int ind = luaL_checkinteger(L, -2);
	luaL_argcheck(L, 1 <= ind && ind <= vec->len, 2, "Index is out of boundaries");
	double value = luaL_checknumber(L, -1);
	/* Set value */
	vec->data[ind] = value;
	return 0;
}

static int realvector_length(lua_State *L)
{
	RealVector *vec = (RealVector *) luaL_checkudata(L, 1, "MLSMat::RealVector");
	lua_pushinteger(L, vec->len);
	return 1;	
}

static int realvector_concat(lua_State *L)
{
	RealVector *vec1 = (RealVector *) luaL_checkudata(L, 1, "MLSMat::RealVector");
	RealVector *vec2 = (RealVector *) luaL_checkudata(L, 2, "MLSMat::RealVector");
	RealVector *resvec = (RealVector *) c_realvector_create(L, vec1->len + vec2->len);

	double *out = resvec->data + 1, *in1 = vec1->data + 1, *in2 = vec2->data + 1;
	for (int i = 1; i <= vec1->len; i++) {
		*out++ = *in1++;
	}
	for (int i = 1; i <= vec2->len; i++) {
		*out++ = *in2++;
	}
	return 1;
}

static int realvector_copy(lua_State *L)
{
	RealVector *vec = (RealVector *) luaL_checkudata(L, 1, "MLSMat::RealVector");
	RealVector *resvec = (RealVector *) c_realvector_create(L, vec->len);
	memcpy(resvec->data + 1, vec->data + 1, vec->len * sizeof(double));
	return 1;
}

static int realvector_max(lua_State *L)
{
	RealVector *vec = (RealVector *) luaL_checkudata(L, 1, "MLSMat::RealVector");
	if (vec->len == 0) {
		lua_pushnil(L);
		return 1;
	}
	double maxval = vec->data[1];
	for (int i = 2; i <= vec->len; i++) {
		if (vec->data[i] > maxval) {
			maxval = vec->data[i];
		}
	}
	lua_pushnumber(L, maxval);
	return 1;
}

static int realvector_min(lua_State *L)
{
	RealVector *vec = (RealVector *) luaL_checkudata(L, 1, "MLSMat::RealVector");
	if (vec->len == 0) {
		lua_pushnil(L);
		return 1;
	}
	double minval = vec->data[1];
	for (int i = 2; i <= vec->len; i++) {
		if (vec->data[i] < minval) {
			minval = vec->data[i];
		}
	}
	lua_pushnumber(L, minval);
	return 1;
}

static int realvector_linspace(lua_State *L)
{
	/* Check inputs */
	if (lua_gettop(L) != 3) {
		luaL_error(L, "Invalid number of arguments");
	}
	double a = luaL_checknumber(L, 1);
	double b = luaL_checknumber(L, 2);
	double n = luaL_checkinteger(L, 3);
	if (n < 0) {
		luaL_error(L, "Invalid value of n");
	}
	if (n == 0) {
		n = 1;
	}
	/* Create output vector */
	RealVector *vec = c_realvector_create(L, n);
	for (int i = 1; i <= n; i++) {
		vec->data[i] = a + (i - 1) * (b - a) / (n - 1);
	}
	return 1;
}

static const struct luaL_Reg realvector_funcs[] = {
	{"new", realvector_new},
	{"rand", realvector_rand},
	{"randn", realvector_randn},
	{"__add", realvector_add},
	{"__sub", realvector_sub},
	{"__mul", realvector_mul},
	{"__div", realvector_div},
	{"__pow", realvector_pow},
	{"__unm", realvector_unm},
	{"__len", realvector_length},
	{"__concat", realvector_concat},
	{"abs", realvector_abs},
	{"exp", realvector_exp},
	{"log", realvector_log},
	{"sqrt", realvector_sqrt},
	{"totable", realvector_totable},
	{"copy", realvector_copy},
	{"max", realvector_max},
	{"min", realvector_min},
	{"linspace", realvector_linspace},
	{"__tostring", realvector_tostring},
	{"__index", realvector_getvalue},
	{"__newindex", realvector_setvalue},
	{"__gc", realvector_gc},
	{NULL, NULL}
};

int __declspec(dllexport) luaopen_mlsmat(lua_State* L)
{
	static int initialized = 0;
	if (!initialized) {
		initialized = 1;
	} else {
		return 0;
	}

	srand(time(NULL));
	lua_newtable(L);

	lua_pushstring(L, "RealVector");
	luaL_newmetatable(L, "MLSMat::RealVector");
	luaL_setfuncs(L, realvector_funcs, 0);
	lua_settable(L, -3);

	lua_pushstring(L, "IndexRange");
	luaL_newmetatable(L, "MLSMat::IndexRange");
	luaL_setfuncs(L, indexrange_funcs, 0);
	lua_settable(L, -3);

	lua_pushstring(L, "DualNVector");
	luaL_newmetatable(L, "MLSMat::DualNVector");
	lua_settable(L, -3);
	/* Short aliases for constructors */
	lua_pushstring(L, "Vec");
	lua_pushcfunction(L, realvector_new);
	lua_settable(L, -3);

	lua_pushstring(L, "Rng");
	lua_pushcfunction(L, indexrange_new);
	lua_settable(L, -3);

	return 1;
}
