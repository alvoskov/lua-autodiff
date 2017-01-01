local X, Y, env = nil, nil, nil

local m = {}

function m:initfunc()
	env = self
	Y = env.Vec{2.86, 2.64, 1.57, 1.24, 0.45, 1.02, 0.65, 0.18, 0.15, 0.01, 0.04, 0.36}
	X = env.Vec{0.0,  0.0,  1.0,  1.0,  2.0,  2.0,  3.0,  3.0,  4.0,  4.0,  5.0,  5.0}
end

function m.resfunc(b)
	local F = b[1] + b[2]*(-b[3]*X):exp() - Y -- Residuals
	return F
	
end

return m