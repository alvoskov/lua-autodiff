t = require('mlsmat')
vec = t.vec


a = -(t.vec{1,2,3,4,5} - t.vec{5,6,7,-8,-9}):abs():sqrt()
a[1] = 100;
a[2] = 1.23456788e20;
a[3] = 1.23456788e-20;
print(a)
print(a:totable())
print(a:copy())
print(a:max())
print(a:min())
print('--------------');

for i,v in ipairs(a:totable()) do
	print(i, v)
end




ii = t.IndexRange.new(1,2,5)
print(ii)

b = t.RealVector.new{10,-20,30,-50,60,-70,80}
print(b)
print(b[t.rng(1,2,-1)])
print(b[t.rng(6,1,2)])
print(b[t.rng(-1,-3,1)])


print(vec{10} / vec{20,30})
print(vec{20,30} .. vec{40,50,60})



--[[
a = t.RealVector.new(33)
a = a - 50
b = t.RealVector.new(33)
a[11] = 5;
b[10] = 60;
print(a[5])
print(a)
print(b)
--
print(a+b)
print(a-1)
print(1/a)

print('-----------------');
print(a)
--print((t.RealVector.abs(a)))
print(a:abs():sqrt())

print(#a)
--print(a.abs(a))

--m.exp(a)

--]]
