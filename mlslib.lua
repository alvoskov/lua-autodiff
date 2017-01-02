--
-- Implementation of DualNVector class: n-dimensional dual number vector for Lua 5.3
-- It actively uses RealVector class implemented as C library
--
-- (C) 2016-2017 Alexey Voskov (alvoskov@gmail.com)
-- License: MIT (X11) license
--

-- A universal loader for binary parts: both for static and dynamic linking
-- Two modes of loading are possible:
-- * Dynamic linking - mlsmat.dll (RealVector class) is loaded by require("mlsmat")
-- * Static linking - mlsmat.c (RealVector class) is loaded by cwrapper.c (or user)
--   and taken from __MLSMat global variable
local m = nil
if __MLSMat == nil then -- Dynamic linking
	m = require("mlsmat")
else -- Static linking (preloaded module)
	m = __MLSMat
end
-- Load math library (built-in in Lua)
local math = require("math")

-- Prohibit creation of global variables
-- (to improve reliablity of code)
setmetatable(_G, {__newindex = function (_,_)
	error('No creation of globals is allowed');
end })

-- Checks if the value is an array index
function m.isindex(ind)
	if type(ind) == "number" and math.floor(ind) == ind and ind >= 1 then
		return true
	else
		return false
	end
end

-- DualNVector.new  Creates a dual number either from
-- scratch or from user-defined RealVector variables
-- Usage:
--   obj = DualNVector.new(size, nvars)
--   obj = DualNVector.new(real, imag1, imag2, ...)
function m.DualNVector.new (...)
	local arg = {...}
	if #arg < 2 then
		error('Invalid number of input arguments')
	end
	local obj = {real = {}, imag = {}}
	-- Different variants of vector creation
	if #arg == 2 and m.isindex(arg[1]) and m.isindex(arg[2]) then
		-- Create empty vector
		local size, nvars = arg[1], arg[2]
		obj.real = m.Vec(size)
		for i = 1, nvars do
			obj.imag[i] = m.Vec(size)
		end
	else
		-- Create vector from RealVector vectors
		-- a) check arguments types and sizes
		local veclen = #(arg[1])
		for i = 1, #arg do
			if getmetatable(arg[i]) ~= m.RealVector then
				error(('Argument %d must be a RealVector'):format(i))
			end
			if #(arg[i]) ~= veclen then
				error(('Argument %d size is not consistent'):format(i))
			end
		end
		-- b) save arguments
		obj.real = arg[1]:copy()
		for i = 1, #arg - 1 do
			obj.imag[i] = arg[i + 1]:copy()
		end
	end
	setmetatable(obj, m.DualNVector)
	return obj
end

-- DualNVector.const  Creates a dual number containing const
-- Usage:
--   obj = DualNVector.const(value, nvars)
-- Inputs:
--   value -- RealVector or number -- value of constant
--   nvars -- number -- number of variables (for differentiation)
-- Output:
--   obj -- DualNVector class example
function m.DualNVector.const(value, nvars)
	-- Check input arguments
	if not m.isindex(nvars) then
		error('Invalid nvars value')
	end
	-- Fill real (with value) and imaginary (with zeros) parts
	local obj = nil
	if type(value) == "number" then
		-- Create scalar constant
		obj = m.DualNVector.new(1, nvars)
		obj.real[1] = value
	elseif getmetatable(value) == m.RealVector then
		-- Create vectorized constant from RealVector
		obj = m.DualNVector.new(#value, nvars)
		obj.real = value:copy()
	elseif type(value) == "table" then
		-- Create vectorized constant from table
		obj = m.DualNVector.new(#value, nvars)
		obj.real = m.RealVector.new(value)
	else
		error('value must be either number or RealVector')
	end
	return obj
end

-- DualNVector.var  Creates a dual number containing a variable
-- suitable for automatic differentiation
-- Usage:
--   obj = DualNVector.var(value, varind, nvars)
function m.DualNVector.var(value, varind, nvars)
	-- Check input arguments
	if not m.isindex(nvars) then
		error('Invalid nvars value')
	end
	-- Check input arguments
	if not m.isindex(varind) or varind > nvars then
		error('Invalid varind value')
	end
	-- Fill real part
	local obj = m.DualNVector.const(value, nvars)
	-- Fill imaginary part and return
	obj.imag[varind] = obj.imag[varind] + 1
	return obj
end

-- DualNVector.copy  Creates a full copy of a class example
-- Usage:
--   objcopy = obj:copy()
function m.DualNVector:copy()
	local obj = {}
	obj.real, obj.imag = self.real:copy(), {}
	for i = 1, #self.imag do
		obj.imag[i] = self.imag[i]:copy()
	end
	setmetatable(obj, m.DualNVector)
	return obj
end

-- Generic implementation of binary operations
-- with all required checks
-- Usage:
--   r = DualNVector.binop(o1, o2, func, dfunc)
-- Inputs:
--   o1, o2 -- input arguments
--   func(real1, real2) -- function for calculating real part
--   func(real1, imag1, real2, imag2) -- function for calculating imag part
function m.DualNVector.binop(o1, o2, func, dfunc)
	-- Check arguments consistency
	local nvars = nil
	if getmetatable(o1) ~= m.DualNVector then
		nvars = #(o2.imag)
		o1 = m.DualNVector.const(o1, nvars)
	end
	if getmetatable(o2) ~= m.DualNVector then
		nvars = #(o1.imag)
		o2, nvars = m.DualNVector.const(o2, nvars)
	end
	if #(o1.imag) ~= #(o2.imag) then
		error('Numbers of variables are not consistent')
	end
	-- Create the resulting object
	local r = {real = func(o1.real, o2.real), imag = {}}
	for i = 1, #(o1.imag) do
		r.imag[i] = dfunc(o1.real, o1.imag[i],  o2.real, o2.imag[i])
	end
	setmetatable(r, m.DualNVector)
	return r
end

-- __add (+) operator implementation
function m.DualNVector.__add_func(o1r, o2r)
	return o1r + o2r
end
function m.DualNVector.__add_dfunc(o1r, o1i, o2r, o2i)
	return o1i + o2i
end
function m.DualNVector.__add(o1, o2)
	return m.DualNVector.binop(o1, o2,
		m.DualNVector.__add_func,
		m.DualNVector.__add_dfunc)
end

-- __sub (-) operator implementation
function m.DualNVector.__sub_func(o1r, o2r)
	return o1r - o2r
end
function m.DualNVector.__sub_dfunc(o1r, o1i, o2r, o2i)
	return o1i - o2i
end
function m.DualNVector.__sub(o1, o2)
	return m.DualNVector.binop(o1, o2,
		m.DualNVector.__sub_func,
		m.DualNVector.__sub_dfunc)
end

-- __mul (*) operator implementation
function m.DualNVector.__mul_func(o1r, o2r)
	return o1r * o2r
end
function m.DualNVector.__mul_dfunc(o1r, o1i, o2r, o2i)
	return o1i * o2r + o1r * o2i
end
function m.DualNVector.__mul(o1, o2)
	return m.DualNVector.binop(o1, o2,
		m.DualNVector.__mul_func,
		m.DualNVector.__mul_dfunc)
end

-- __div (/) operator implemenation
function m.DualNVector.__div_func(o1r, o2r)
	return o1r / o2r
end
function m.DualNVector.__div_dfunc(o1r, o1i, o2r, o2i)
	return (o1i * o2r - o1r * o2i) / o2r ^ 2
end
function m.DualNVector.__div(o1, o2)
	return m.DualNVector.binop(o1, o2,
		m.DualNVector.__div_func,
		m.DualNVector.__div_dfunc)
end

-- __pow (^) operator implementation
function m.DualNVector.__pow_func(o1r, o2r)
	return o1r ^ o2r
end
function m.DualNVector.__pow_dfunc(o1r, o1i, o2r, o2i)
	local res = nil;
	if #o2i == 1 and o2i[1] == 0 then
		res = o1r ^ (o2r - 1) * (o2r*o1i);
	else
		res = o1r ^ (o2r - 1) * o2r*o1i + o1r ^ o2r * (o2i*(o1r:log()));
		for i = 1, #o2i do
			if o2i[i] == 0 and o1r[i] <= 0 then
				res[i] = o1r[i] ^ (o2r[i] - 1) * (o2r[i]*o1i[i]);
			end
		end
	end
	
	return res
end
function m.DualNVector.__pow(o1, o2)
	return m.DualNVector.binop(o1, o2,
		m.DualNVector.__pow_func,
		m.DualNVector.__pow_dfunc)
end

-- log function implementation
function m.DualNVector:log()
	local r = {real = self.real:log(), imag = {}}
	for i = 1, #(self.imag) do
		r.imag[i] = self.imag[i] / self.real
	end
	setmetatable(r, m.DualNVector)
	return r
end

-- exp function implementation
function m.DualNVector:exp()
	local r = {real = self.real:exp(), imag = {}}
	for i = 1, #(self.imag) do
		r.imag[i] = self.imag[i] * r.real
	end
	setmetatable(r, m.DualNVector)
	return r
end

-- sqrt function implementation
function m.DualNVector:sqrt()
	local r = {real = self.real:sqrt(), imag = {}}
	for i = 1, #(self.imag) do
                r.imag[i] = self.imag[i] / (2 * r.real)
	end
	setmetatable(r, m.DualNVector)
	return r
end

-- unary minus function implementation
function m.DualNVector:__unm()
	local r = {real = -self.real, imag = {}}
	for i = 1, #(self.imag) do
                r.imag[i] = -self.imag[i]
	end
	setmetatable(r, m.DualNVector)
	return r
end

-- Returns number of elements (dual numbers) in the vector
function m.DualNVector:__len()
	return #self.real
end

-- DualNVector:tostring  Metamethod that converts DualNVector object
-- to string containing all its values (including real and imaginary part)
function m.DualNVector:__tostring()
	local str = ("DualNVector: %d elements (%d variables)\n"):format(#(self.real), #(self.imag))
	str = str .. "Real part:\n" .. tostring(self.real);
	for i = 1, #(self.imag) do
		str = str .. ("Imaginary part (variable %d):\n"):format(i);
		str = str .. tostring(self.imag[i])
	end
	
	return str
end

-- Returns subvector using user-defined index (see RealVector indexing modes)
function m.DualNVector:__index(ind)
	if type(ind) == "number" or type(ind) == "table" then
		local r = {imag = {}}
		-- Real part
		r.real = self.real[ind]
		if type(r.real) == "number" then
			r.real = m.RealVector.new({r.real})
		end
		-- Imaginary part
		if type(real) == "number" then
			for i = 1, #self.imag do 
				r.imag[i] = self.imag[i][ind]
			end
		else
			for i = 1, #self.imag do 
				r.imag[i] = m.RealVector.new({self.imag[i][ind]})
			end
		end
		-- Set required metatables
		setmetatable(r, m.DualNVector)
		return r
	else
		return m.DualNVector[ind]
	end
end

---- Aliases for some methods
function m.DConst(value, nvars)
	return m.DualNVector.const(value, nvars)
end

function m.DVar(value, varind, nvars)
	return m.DualNVector.var(value, varind, nvars)
end

-- Return module table
return m
