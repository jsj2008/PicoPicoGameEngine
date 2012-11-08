ti=require("tileinfo")
g=ppgraph
sc=ppscreen
base=pprect(0,0,320,480)

sp=ppsprite:new()
pptex:default(pptex:load("main.png"))
sp:tile(2)

mycar=ppsprite.new()

function start()
	sc:viewport(sc:layout(base))
	ppgraph:locate(0,0)
	ppgraph:print("HEELO")
	sp:draw()
end
