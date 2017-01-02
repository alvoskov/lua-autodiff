INCLUDE = -IC:/C_PROG/LUA/src
LIBS = -L. -llua53 -llevmar
LIBS_LUASTAT = -L. -llua -llevmar
LIBS_PATH = -L.
KEYS = -O2 -std=c99
CC = gcc
luamat: libladif.dll mlsmat.dll ex_levmar.exe ex_levmar_static.exe
libladif.dll: cwrapper_static.o mlsmat.o
	$(CC) -shared cwrapper_static.o mlsmat.o libladif.def -Wl,--exclude-all-symbols -o libladif.dll $(LIBS_LUASTAT) $(LIBS_PATH)
ex_levmar.exe: ex_levmar.o cwrapper.o
	$(CC) ex_levmar.o cwrapper.o -o ex_levmar.exe $(LIBS) $(LIBS_PATH)
ex_levmar_static.exe: ex_levmar.o cwrapper_static.o mlsmat.o
	$(CC) ex_levmar.o cwrapper_static.o mlsmat.o -o ex_levmar_static.exe $(LIBS_LUASTAT) $(LIBS_PATH)
mlslib_lua.c: mlslib.lua makescript.lua
	lua makescript.lua
ex_levmar.o: ex_levmar.c cwrapper.h
	$(CC) ex_levmar.c -fPIC -c -o ex_levmar.o $(INCLUDE) $(KEYS)
cwrapper_static.o: cwrapper.c mlsmat.h cwrapper.h
	$(CC) cwrapper.c -fPIC -c -o cwrapper_static.o -DSTATIC_LINK $(INCLUDE) $(KEYS)
cwrapper.o: cwrapper.c mlsmat.h cwrapper.h
	$(CC) cwrapper.c -fPIC -c -o cwrapper.o $(INCLUDE) $(KEYS)
mlsmat.dll: mlsmat.o
	$(CC) -shared mlsmat.o -o mlsmat.dll $(LIBS_PATH) $(LIBS)
mlsmat.o: mlsmat.c mlsmat.h
	$(CC) mlsmat.c -fPIC -c -o mlsmat.o $(INCLUDE) $(KEYS)
