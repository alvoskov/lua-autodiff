#ifndef __MLSMAT_H
#define __MLSMAT_H

typedef struct {
	int a;
	int b;
	int step;
} IndexRange;

typedef struct {
	int len;
	double *data;
} RealVector;

int __declspec(dllexport) luaopen_mlsmat(lua_State* L);

#endif