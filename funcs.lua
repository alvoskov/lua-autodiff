--
-- func.lua  An example of input file for ex_levmar.exe. It contains
-- several data sets that are selected by if..then..elseif..else operator.
-- This example demonstates that the user can actively use Lua language.
--
-- Data set for fitting is taken from:
-- Graybill F.A. and Iyer H.K. Regression analysis. Concepts and
-- applications. 1994. Duxbury Pr. ISBN 0534198694
-- http://www.stat.colostate.edu/regression_book/chapter9.pdf
--
-- (C) 2016-2017 Alexey Voskov (alvoskov@gmail.com)
-- License: MIT (X11) license

-- Internal variables
local X, Y = nil, nil -- Data set for curve fitting
local env = nil -- mlslib.lua module
local m = {} -- Module table (that must be returned by this script)

local dataSet = 1
if dataSet == 0 then
	-- Initialization function
	function m:initfunc()
		env = self
		Y = env.Vec{2.86, 2.64, 1.57, 1.24, 0.45, 1.02, 0.65, 0.18, 0.15, 0.01, 0.04, 0.36}
		X = env.Vec{0.0,  0.0,  1.0,  1.0,  2.0,  2.0,  3.0,  3.0,  4.0,  4.0,  5.0,  5.0}
		return env.Vec{0.2, 0.21, 0.22} -- Initial approximation
	end
	-- Residuals calculation function (not that b is DualNVector, i.e. dual number vector)
	function m.resfunc(b)
		local F = b[1] + b[2]*(-b[3]*X):exp() - Y -- Residuals
		return F
	end
elseif dataSet == 1 then
	function m:initfunc()
		env = self
		Y = env.Vec{1.51, 1.49, 1.47, 1.43, 1.35, 1.19, 0.96, 0.85, 0.65, 0.64, 0.58, 0.56, 0.52, 0.53, 0.49, 0.50}
		X = env.Vec{0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20}
		return env.Vec{12, 10, 12} -- Initial approximation
	end
	-- Residuals calculation function (not that b is DualNVector, i.e. dual number vector)
	function m.resfunc(b)
		local F = b[1]*(1 - (-(-b[2]-b[3]*X):exp()):exp()) - Y -- Residuals
		return F
	end
	-- Results:
	-- beta: 1.9483, -1.2699, 14.3631
	-- sbeta: 0.472,   0.681,   2.704
elseif dataSet == 2 then
	function m:initfunc()
		env = self
		Y = env.Vec{0.02,  0.06,  0.10,  0.18,  0.35,  0.56,  0.78,  0.86,  0.94,  0.99}
		X = env.Vec{0.000, 0.005, 0.010, 0.015, 0.020, 0.025, 0.030, 0.035, 0.040, 0.045}
		return env.Vec{-20, 40} -- Initial approximation
	end
	-- Residuals calculation function (not that b is DualNVector, i.e. dual number vector)
	function m.resfunc(b)
		local F = 1 / (1 + (-b[1] - b[2]*X):exp()) - Y -- Residuals
		return F
	end
	-- Results:
	-- beta: -4.0261, 171.6644
	-- sbeta: 0.129, 5.299
elseif dataSet == 3 then
	function m:initfunc()
		env = self
		Y = env.Vec{1.8,  2.9,  6.0,  8.8,  6.6,  3.8,  2.9,  1.5,  1.1,  0.5,  1.1,  0.2}
		X = env.Vec{0.5,  1.0,  1.5,  2.0,  2.5,  3.0,  3.5,  4.0,  4.5,  5.0,  5.5,  6.0}
		return env.Vec{2, -2, 2} -- Initial approximation
	end
	-- Residuals calculation function (not that b is DualNVector, i.e. dual number vector)
	function m.resfunc(b)
		local F = 1 / (b[1] + b[2]*X + b[3]*X^2) - Y -- Residuals
		return F
	end
	-- Results:
	-- beta: 0.8183, -0.6776, 0.1633
	-- sbeta: 0.0653, 0.0616, 0.0145
else
	error(string.format('Unknown data set %d', dataSet));
end

-- Return table with functions
return m
