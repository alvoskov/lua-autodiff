--
-- func_cpfit.lua  An example of input file for ex_levmar.exe
-- It shows the possiblity of complex operations such as file loading,
-- debug output, different kinds of derivatives.
--
-- (C) 2016-2017 Alexey Voskov (alvoskov@gmail.com)
-- License: MIT (X11) license


local env, Texp, Cpexp = nil, nil, nil -- Data set for curve fitting
local CONST_R = 8.3144598 -- Universal gas constant


local function TypeDerivatives(dn)
for i = 1, #(dn.real) do
	for j = 1, #(dn.imag) do
		io.write(string.format("%10.4g ", dn.imag[j][i]))
	end
	io.write("\n");
end
end

-- This function uses automatic differentiation (dual numbers)
-- Note that there are some issues with rounding errors and the
-- not evident workaround (see processing of NaNs in the function)
local function resAutoDiff(b)
	local nparams = #b
	local Cp = 0
	print(b.real)
 	for i = 1, nparams // 2 do
		local alpha, theta = b[i], b[nparams // 2 + i]
		local x = theta / Texp
		local ex = x:exp()
		local Cpterm = 3*alpha*ex*x^2 / (ex - 1) ^ 2
		local ii = x:gt(300)
		Cpterm[ii] = 3*alpha*x[ii]^2 / ex[ii]
		-- Prevents situations like "0*Inf gives NaN"
		-- (the occur at low T due to e(x)-->Inf)
		-- NOTE: Removing this code will break the optimization!
		for i = 1, #(Cpterm.imag) do
			local jj = Cpterm.imag[i]:isnan()
			Cpterm.imag[i][jj] = 0
		end
		-- Add term to the sum
		Cp = Cp + Cpterm
	end
	Cp = Cp * CONST_R
	return Cp - Cpexp
end

-- This function uses analytical differentiation (i.e. derivatives
-- that are written in explicit form inside the code)
local function resAnalytDiff(bDual)
	local nparams = #bDual
	local Cp = 0 -- Heat capacity (sum of terms)
	local b = bDual.real -- No dual numbers are required for analytical derivatives
 	for i = 1, nparams // 2 do
		-- Prepare intermediate part
		local alpha, theta = b[i], b[nparams // 2 + i]
		local x = theta / Texp
		local ex = x:exp()
		local Cpterm_real, ii = env.Vec(#Texp), x:gt(300)
		local x_ii, ex_ii, x_nii, ex_nii = x[ii], ex[ii], x[~ii], ex[~ii]
		-- Calculate function value
		Cpterm_real[~ii] = 3*alpha*ex_nii*x_nii^2 / (ex_nii - 1) ^ 2
		Cpterm_real[ii] = 3*alpha*x_ii^2 / ex_ii
		-- Initialize arrays for derivatives
		local Cpterm_imag = {}
		for j = 1, nparams do
			Cpterm_imag[j] = env.Vec(#Texp)
		end
		-- Calculate derivatives
		-- a) alpha
		Cpterm_imag[i][~ii] = 3*ex_nii*x_nii^2 / (ex_nii - 1) ^ 2
		Cpterm_imag[i][ii] = 3*x_ii^2 / ex_ii
		-- b) theta
		Cpterm_imag[i + nparams // 2][~ii] =
			3*alpha / (Texp[~ii] * (ex_nii - 1) ^ 2) * x_nii * ex_nii * ( 
			(2 + x_nii) - 2*x_nii*ex_nii / (ex_nii - 1) )
		Cpterm_imag[i + nparams // 2][ii] =
			3*alpha*x_ii / (Texp[ii] * ex_ii) * (2 - x_ii)
		-- Create dual number used calculated derivatives
		local Cpterm = env.DualNVector.new(Cpterm_real, table.unpack(Cpterm_imag))
		-- Add term
		Cp = Cp + Cpterm
	end
	Cp = Cp * CONST_R
	return Cp - Cpexp
end

-- Heat capacity function without any derivatives
local function Cpfunc(b)
	local nparams = #b
	local Cp = 0 -- Heat capacity (sum of terms)
 	for i = 1, nparams // 2 do
		-- Prepare intermediate part
		local alpha, theta = b[i], b[nparams // 2 + i]
		local x = theta / Texp
		local ex = x:exp()
		local Cpterm, ii = env.Vec(#Texp), x:gt(300)
		-- Calculate function value
		Cpterm[~ii] = 3*alpha*ex[~ii]*x[~ii] / (ex[~ii] - 1) ^ 2
		Cpterm[ii] = 3*alpha*x[ii]^2 / ex[ii]
		-- Add term
		Cp = Cp + Cpterm		
	end
	Cp = Cp * CONST_R
	return Cp - Cpexp
end


-- Create and return the table that must be returned by the module
return {
	-- Initialization function
	initfunc = function(newEnv)
		local Xary, Yary = {}, {}
		env = newEnv
		for line in io.lines('ScF3.dat') do
			local T, Cp = line:match("([^%s]+)%s([^%s]+)")
			Xary[#Xary + 1] = tonumber(T)
			Yary[#Yary + 1] = tonumber(Cp)
		end
		Texp, Cpexp = env.Vec(Xary), env.Vec(Yary)
--
--		for i = 1, #Texp do print(Texp[i], ',', Cpexp[i], ',') end
		return env.Vec{0.1, 0.1, 1.0, 1.0} -- Init.approx: {alphav, thetav};
--		return env.Vec{1.49265, 2.21149, 149.893, 488.283} -- Init.approx: {alphav, thetav}
	end,

	-- Residuals calculation function
	resfunc = resAutoDiff
--	resfunc = resAnalytDiff
}
