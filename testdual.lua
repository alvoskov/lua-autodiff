--
-- Tests for DualNVector class (implementation of dual numbers)
-- (C) 2016-2017 Alexey Voskov
-- License: MIT (X11) License 
--

local d = require("mlslib")

local function create_x23grid()
	local x2, x3 = {}, {}, {}
	local x2i = 0
	while x2i <= 1.0 do
		local x3i = 0
		while x3i <= 1 - x2i do
			x2[#x2 + 1], x3[#x3 + 1] = x2i, x3i
			x3i = x3i + 1 / 64
		end
		x2i = x2i + 1 / 64
	end
	x2, x3 = d.vec(x2), d.vec(x3)
	local x1 = 1 - x2 - x3

	for i = 1, #x2 do
		if x2[i] == 0 then x2[i] = 1e-15 end
		if x3[i] == 0 then x3[i] = 1e-15 end
		if x2[i] == 1 then x2[i] = 1 - 1e-15 end
		if x3[i] == 1 then x3[i] = 1 - 1e-15 end
	end
	return x1, x2, x3
end

local function test_basic()
	-- Test with logarithms
	print('===== Some basic tests (Gid based)');
	local x1, x2, x3 = create_x23grid()
	local x2d = d.DualNVector.var(x2, 1, 2)
	local x3d = d.DualNVector.var(x3, 2, 2)
	local x1d = 1 - x2d - x3d

	local R, T = 8.314, 298.15

	local Gd = R*T*(x1d*x1d:log() + x2d*x2d:log() + x3d*x3d:log());
	local lna1_d = Gd.real - x2d.real*Gd.imag[1] - x3d.real*Gd.imag[2];
	local lna2_d = Gd.real + (1 - x2d.real)*Gd.imag[1] - x3d.real*Gd.imag[2];
	local lna3_d = Gd.real - x2d.real*Gd.imag[1] + (1 - x3d.real)*Gd.imag[2];

	local G = R*T*(x1*x1:log() + x2*x2:log() + x3*x3:log());
	print(string.format('dG:    %g', (G - Gd.real):abs():max()))
	print(string.format('dlna1: %g', (lna1_d - R*T*x1:log()):abs():max()))
	print(string.format('dlna2: %g', (lna2_d - R*T*x2:log()):abs():max()))
	print(string.format('dlna3: %g', (lna3_d - R*T*x3:log()):abs():max()))
	print('')
end


local function test_exp()
	print('===== exp and sqrt functions test')
	local x = (0.5*d.RealVector.randn(1000)):abs();
	local y = (1 + 50*d.RealVector.rand(1000)):abs();

	local xd = d.DualNVector.var(x, 1, 2)
	local yd = d.DualNVector.var(y, 2, 2)

	local f = (x ^ 2.5 + y:sqrt()):exp();
	local dfdx = f * 2.5 * x ^ 1.5;
	local dfdy = f * 0.5 / y:sqrt();

	local fd = (xd ^ 2.5 + yd:sqrt()):exp();

	print(string.format('dF:      %g', (f - fd.real):abs():max()))
	print(string.format('d(dFdX): %g', (dfdx - fd.imag[1]):abs():max()))
	print(string.format('d(dFdY): %g', (dfdy - fd.imag[2]):abs():max()))
	print('');
end


local function test_div()
	print('===== rdivide operator test');
	local x = 1 + 100*d.RealVector.rand(8000);
	local y = 1 + 100*d.RealVector.rand(8000);
	local z = 1 + 100*d.RealVector.rand(8000);

	local xd = d.DualNVector.var(x, 1, 3)
	local yd = d.DualNVector.var(y, 2, 3)
	local zd = d.DualNVector.var(z, 3, 3)

	local fd = zd / (xd + (xd*yd):log())

	local f = z / (x + (x*y):log());
	local dfdx = -z*(1 + 1 / x) / (x + (x*y):log())^2;
	local dfdy = -z/y/(x + (x*y):log())^2;
	local dfdz = 1/(x + (x*y):log());

	print(string.format('dF:      %g', (f - fd.real):abs():max()))
	print(string.format('d(dFdX): %g', (dfdx - fd.imag[1]):abs():max()))
	print(string.format('d(dFdY): %g', (dfdy - fd.imag[2]):abs():max()))
	print(string.format('d(dFdZ): %g', (dfdz - fd.imag[3]):abs():max()))
	print('')
end

local function test_power()
	print('===== power operator test')
	local x = d.RealVector.linspace(0.01, 3, 250)
	local y = d.RealVector.linspace(0.1, 2.9, 250);
	local f = x ^ y;
	local dfdx = y*x^(y-1);
	local dfdy = x ^ y * x:log();
	
	local xd = d.DVar(x, 1, 2);
	local yd = d.DVar(y, 2, 2);
	local df = xd ^ yd;

	print('Test 1: d(x^y)/dx partial derivative');
	print(string.format('  dF:      %g', (f - df.real):abs():max()))
	print(string.format('  d(dFdX): %g', (dfdx - df.imag[1]):abs():max()))

	print('Test 2: d(x^y)/dy partial derivative');
	print(string.format('  dF:      %g', (f - df.real):abs():max()))
	print(string.format('  d(dFdY): %g', (dfdy - df.imag[2]):abs():max()))

	print('Test 3: d(y^y)/dy partial derivative');
	f = y ^ y;
	df = yd ^ yd;
	dfdy = y^y * (y:log() + 1);
	print(string.format('  dF:      %g', (f - df.real):abs():max()))
	print(string.format('  d(dFdY): %g', (dfdy - df.imag[2]):abs():max()))

	print('Test 4: d(a^n)/dx (x = 0) partial derivatives and values');
	local n = d.Vec{-4.55, -2, 0, 2, 4.55}
	local a = d.Vec{0,      0, 0, 0, 0}
	local ad = d.DVar(a, 1, 1)

	f = a ^ n;
	local dfda = n * a ^ (n-1)
	local fd = ad ^ n

	local numOfEq = 0;
	for i = 1, #fd do
		if fd.real[i] == f[i] then
			numOfEq = numOfEq + 1;
		end
		if (fd.imag[1][i] == dfda[i]) or (fd.imag[1][i] ~= fd.imag[1][i] and dfda[i] ~= dfda[i]) then
			numOfEq = numOfEq + 1;
		end
	end
	if numOfEq ~= 10 then
		print('  Test: FAILURE');
		print(f)
		print(dfda)
		print(fd)
	else
		print('  Test: OK');
	end
	print('')
end


test_basic()
test_exp()
test_div()
test_power()


print('------------')
local x = d.Vec{1,2,3,4,5}
local y = d.DConst(d.Vec{3,4,5,6,7}, 1, 1)
local rr = x + y
print(rr)

