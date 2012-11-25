g=ppgraph
t=pptex
sc=ppscreen
s=ppscreen:size()
base=pprect(0,0,320,480)

button=ppsprite.new()
button:texture("button.png")
button:tileInfo({size=button:texture().imageSize})
button:pos(128+32,96)
local count=0
local count2=0

function initVKey(center_x,center_y,width,height)
 local v=ppvkey
 v.width=width
 v.height=height
 v.x=center_x+width/2
 v.y=center_y+height/2
 v:center(v)
 v:fixed(true)
 v.area=pprect(v)
 v.area=v.area+pppoint(-v.width/2,-v.height/2)

 v.draw=function(v)
  g:scale(2)
  g:pos(v:center()/2)
  g:move(- 8,- 8)
  g:print("○")
  g:move(  0,-32)
  g:print("↑")
  g:move( 32, 32)
  g:print("→")
  g:move(-32, 32)
  g:print("↓")
  g:move(-32,-32)
  g:print("←")
  g:scale(1)
  g:box(v.area)
 end

 return v
end

v=initVKey(0,base.height-240,240,240)

t:default(t:load("sample.png"))
ship=ppsprite.new()
ship:tile(2)
ship:pos(base.width/2-16,base.height/2-16)

mv4={{0,0},{1,0},{0,-1},{-1,0},{0,1}}
mv8={{0,0},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}}

function start()
 sc:layout(base)

 ship:move(mv8[v:dir(8,v.area)+2])
-- ship:move(mv8[v:dir(8)+2])

 g:fill(base,g.green)
 ship:draw()

-- sc:layout(sc:size())
 g:print(v:dir(8,v.area))
 v:draw()
 g:pos(0,64)
 local c=#pptouch()
 g:print(c)
 button.prehit=button.hit
 if button:hitCheck(sc:arrayto(pptouch())) then
  count2=count2+1
 end
 if button.hit and (not button.prehit) then
  count=count+1
 else
  button:alpha(255)
 end
 g:move(0,16)
 g:print("count  "..count)
 g:move(0,16)
 g:print("count2 "..count2)
 if v:touch() then
  g:put(ship:pos()+v:delta(),2)
 end
 button:draw()
end
