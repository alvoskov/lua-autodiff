# lua-autodiff
Implements vectorized automatic differentiation for Lua. It is designed
mainly for usage for the least squares method with user-defined functions
and allows to avoid both manual Jacobian programming and numeric
differentation.

The next libraries are required:
* Lua 5.2 or higher (as scripting language)
* levmar 2.6 or higher (Levenberg-Marquardt method implementation)

Files:
* COPYING - File containing license
* cwrapper.c - Interface for calling Lua functions with automatic differentation
  from C or C++ (hides all Lua internals)
* cwrapper.h - Interface API functions declaration (see above for details)
* ex_levmar.c - Example of using Lua scripts with automatic differentation
  for Levenberg-Marquardt method implementation
* func.lua - Simple function example for ex_levmar.c and cwrapper.c
* funcs.lua - Several functions examples for ex_levmar.c and cwrapper.c
* Makefile - Make file for GNU Make (mainly for GCC, MinGW etc.)
* makescript.lua - Conversion of mlslib.lua into C file (for static linking)
* mlsmat.c - RealVector Lua class implementation
* mlsmat.h - RealVector Lua class implementation (RealVector C structure declaration)
* mlslib.lua - DualNVector Lua class implementation
* test.lua - tests for RealVector class
* testdual.lua - tests for DualNVector class

Currently the compilation is fully tested only under MinGW.
