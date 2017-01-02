--
-- func.lua  A simple example of input file for ex_levmar.exe
-- Data set for fitting is taken from:
-- Graybill F.A. and Iyer H.K. Regression analysis. Concepts and
-- applications. 1994. Duxbury Pr. ISBN 0534198694
-- http://www.stat.colostate.edu/regression_book/chapter9.pdf
--
-- (C) 2016-2017 Alexey Voskov (alvoskov@gmail.com)
-- License: MIT (X11) license

local X, Y = nil, nil -- Data set for curve fitting
return {
	-- Initialization function
	initfunc = function(env)
		Y = env.Vec{2.86, 2.64, 1.57, 1.24, 0.45, 1.02, 0.65, 0.18, 0.15, 0.01, 0.04, 0.36}
		X = env.Vec{0.0,  0.0,  1.0,  1.0,  2.0,  2.0,  3.0,  3.0,  4.0,  4.0,  5.0,  5.0}
		return env.Vec{0.2, 0.21, 0.22} -- Initial approximation
	end,
	-- Residuals calculation function
	resfunc = function(b)
		return b[1] + b[2]*(-b[3]*X):exp() - Y -- Residuals
	end
}
