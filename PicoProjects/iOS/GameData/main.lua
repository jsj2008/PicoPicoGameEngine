ti=require("tileinfo")
g=ppgraph
sc=ppscreen
base=pprect(0,0,320,480)
score_key="shooting-hiscore"

print(ppgame:documentPath());

f = io.open (ppgame:documentPath().."/text.txt","a")
f:write("Hello")
io.close(f)


bang=ppparticle.new()
bang:property({
  startColor={r=240,g=240,b=0,a=255},
  startColorVariance={r=20,g=20,b=0,a=0},
  finishColor={r=250,g=0,b=0,a=0},
  finishColorVariance={r=0,g=0,b=0,a=0},
  animationTime=0.5,
  animationData=ti.tile[6],
  animationLoopPoint=1
})

hiscore=ppgame:getInteger(score_key,0)
score=0
level=1
titlestr="SPACE ATTACK"
enemytile={3,4,5}
leveldata={
  {n=30,t=32,e={2}},
  {n=30,t=16,e={3}},
  {n=30,t=16,e={2,2,3}},
  {n=30,t=16,e={1,2,2}},
  {n=30,t=16,e={2,2,2}},
  {n=30,t=16,e={2,3,3}},
  {n=30,t=16,e={2}},
  {n=30,t=16,e={2,3,3}},
  {n=30,t=16,e={1,2,3}},

  {n=30,t=8,e={2,4}},
  {n=30,t=8,e={3,4}},
  {n=30,t=8,e={2,2,3,1}},
  {n=30,t=8,e={1,2,2,1}},
  {n=30,t=8,e={2,2,2,1}},
  {n=50,t=8,e={2,3,3,1,4}},
  {n=30,t=8,e={2,1}},
  {n=50,t=4,e={2,3,3,1,4}},
  {n=50,t=4,e={1,2,3,1}},
}

function start()
  title()
  score=0
  s.muteki=0
  s:pos(g:layout(s:size(),true,true,base))
  s.y=s.y+100
  map=ppmap.new()
  map:load("map2.tmx")
  map:area(0,-15,10,15)
  initgame()
  s:show()
  level=0
  count=0
  for i=1,120 do
    movefighter()
    mainloop()
    dispstr("READY!",g.red)
    drawfps()
    g:update()
  end
  while true do
    if s.muteki>0 then
      break
    end
    local l=(level % #leveldata)+1
    if count>=leveldata[l].n then
      count=0
      level=level+1
      for i=1,120 do
        if s.muteki>0 then
          break
        end
        setshot()
        movefighter()
        mainloop()
        drawfps()
        g:update()
      end
    end
    setshot()
    setenemy()
    movefighter()
    mainloop()
    drawfps()
    g:update()
  end
  s:hide()
  if score>hiscore then
    hiscore=score
    ppgame:setInteger(score_key,hiscore)
  end
  for i=1,240 do
    setenemy()
    mainloop()
    dispstr("GAME OVER",g.red)
    drawfps()
    g:update()
  end
end

function dispstr(a,c)
  local b=ppfont:size(a)
  b.y=-32
  g:pos(g:layout(b,true,true,base))
  g:print(a,c)
end

function tdraw(t,dx,dy,c)
  local p=g:layout(t,true,true,base)
  g:pivot(base.width/2,base.height/2)
  g:scale(1.5,5)
  g:pos(p.x+dx,p.y+dy)
  g:print(t,c)
  g:scale(1)
  g:pivot(0,0)
end

function title()
  local map=ppmap.new()
  map:load("map1.tmx")
  map:area(0,-15,10,15)
  local b=ppbutton("START")
  b.y=-150
  b:layout(true,true,false,base)
  b.bg.tile=ti.tile[8][1]
  b.bg.edge=8
  while true do
    sc:viewport(sc:layout(base))
    map:draw()
    tdraw(titlestr,0,-16,g.blue)
    tdraw(titlestr,0,-17,g.cyan)
    if b:idle(sc:arrayto(pptouch())) then
      break
    end
    b:draw()
    dsc(hiscore)
	g:pos(0,16)
	g:print(sc:size().width..","..sc:size().height)
	
	local t=sc:arrayto(pptouch())
	if #t > 0 then
		g:move(0,16)
		g:print(t[1].x..","..t[1].y)
	end

    g:update()
  end
  ppsemml[1]:play(playse)
end

function dsc(sc)
 local str="SCORE"
 if sc>=hiscore then str="HI-SCORE" end
 local s=str.." "..sc
 local r=ppfont:size(s)
 r=g:layout(r,true,false,base)
 g:locate(r)
 g:fill(0,0,base.width,ppfont:height(),0,0,0,128)
 g:print(s)
end

s=ppsprite.new()
s.shot=0
s.enemy=0
s.hitmask=1
s.muteki=0
s.hitrect=pprect(4,4,24,24)
t=ppsprite.new(5)
e=ppsprite.new(10)
f=ppsprite.new(10)

function initgame()
  ppforeach(t,function(i,o)
    o:hide()
    o.hitrect=pprect(4,4,24,24)
    o.hitmask=0
  end)
  ppforeach(e,function(i,o)
    o:hide()
    o.hitrect=pprect(4,4,24,24)
    o.hitmask=0
    o.shot=0
  end)
  ppforeach(f,function(i,o)
    o:hide()
    o.hitrect=pprect(8,8,16,16)
    o.hitmask=0
  end)
end

function setbang(o,se)
  if se~=nil then
    ppsemml[1]:stop()
    ppsemml[1]:play(se)
  end
  bang:property({
     sourcePosition=o:pos()+pppoint(16,16)})
  bang:fire()
end

function sethit(o,se)
  if se~=nil then
    ppsemml[2]:stop()
    ppsemml[2]:play(se)
  end
  bang:property({
     sourcePosition=o:pos()+pppoint(16,16)})
  bang:fire()
end

function shot(o)
  local q=ppforeach(f,function(i,o)
    if (not o:isVisible()) then
      return o
    end
  end)
  if q==nil then return end
  q:show()
  q:pos(o:pos())
  q.hitmask=1
  local t=s:pos()-q:pos()
  q.d=t*4/t:length()
end

function setshot()
  s.shot=s.shot+1
  if s.shot>4 then
    s.shot=0
    local o=ppforeach(t,function(i,o)
      if (not o:isVisible()) then return o end
    end)
    if o~=nil then
      o:pos(s)
      o:show()
      o.hitmask=2
      ppsemml[3]:stop()
      ppsemml[3]:play(shotse)
    end
  end
end

movetable={
  function(self)
    self:move(0,2)
  end,
  function(self)
    if level>=#leveldata then
      self:move(0,6)
    else
      self:move(0,4)
    end
  end,
  function(self)
    if self.p.x<32 then self.p.x=32 end
    if self.p.x>base.width-64 then
      self.p.x=base.width-64
    end
    if level>=#leveldata then
      self.p.y=self.p.y+4
      self.p.q=self.p.q+0.12
    else
      self.p.y=self.p.y+2
      self.p.q=self.p.q+0.08
    end
    self.x=self.p.x+math.sin(self.p.q)*96
    self.y=self.p.y
  end
}

function setenemy()
  s.enemy=s.enemy+1
  local l=(level % #leveldata)+1
  if s.enemy>leveldata[l].t then
    s.enemy=0
    local t=math.random(1,#leveldata[l].e)
    local x=math.floor(base.width/32)
    if leveldata[l].e[t]==0 then
    elseif leveldata[l].e[t]==4 then
      x=math.floor((base.width-32*3)/32)
      x=math.random(1,x)-1
      for i=1,3 do
        set(1,(x+i))
      end
    else
      set(leveldata[l].e[t],math.random(1,x)-1)
    end
  end
end

function set(t,x)
  local o=ppforeach(e,function(i,o)
    if (not o:isVisible()) then return o end
  end)
  if o~=nil then
    o:pos(x*32,-32)
    o.p=pppoint(o:pos())
    o.p.q=math.deg(math.random(0,359))
    o:show()
    o.type=t
    o.hitmask=3
    o.shot=math.random(16,32)
    o.idle=movetable[o.type]
    o.life=3
    count=count+1
  end
  return o
end

function mainloop()
  sc:viewport(sc:layout(base))

  map.y=map.y+1
  if map.y>base.height then map.y=0 end
  map:draw()

  ppforeach(t,function(i,o)
    if o:isVisible() then
      o:move(0,-16)
      o:loopAnime(0.5,ti.tile[2])
      o:draw()
      if o.y<-o:size().height then
        o:hide()
        o.hitmask=0
      end
    end
  end)
  
  ppforeach(e,function(i,o)
    if o:isVisible() then
      o:idle()
--      o:move(0,4)
      o:loopAnime(0.5,ti.tile[enemytile[o.type]])
      o:draw()
      if o.shot>0 then
        o.shot=o.shot-1
      else
        if o.type>1 and o.y<s.y then shot(o) end
        o.shot=math.random(16,48)
      end
      if o.y>base.height then
        o:hide()
        o.hitmask=0
      end
    end
  end)
  
  pphitcheck(e,{s},function(a,b)
    if s.muteki==0 then
      setbang(s,bangse)
      s.muteki=16
   end
  end)
  pphitcheck(f,{s},function(a,b)
    if s.muteki==0 then
      setbang(s,bangse)
      s.muteki=16
   end
  end)
  pphitcheck(t,e,function(a,b)
    if a:isVisible() and b:isVisible() then
      a:hide()
      a.hitmask=0
      if b.type==1 then
        b.life=b.life-1
        ppsemml[2]:stop()
        ppsemml[2]:play("@1o7v9cg")
      else
        b.life=0
      end
      if b.life==0 then
        b:hide()
        b.hitmask=0
        
        if b.type==1 then
          score=score+100
          setbang(b,bangse)
        else
          score=score+10
          sethit(b,hitse)
        end
      end
    end
  end)
  ppforeach(f,function(i,o)
    if o:isVisible() then
      o:pivot(16,16)
      o:rotate(o:rotate()+0.2)
      o:move(o.d)
      o:loopAnime(0.5,ti.tile[7])
      o:draw()
      if (not base:intersect(o:aabb())) then
        o:hide()
        o.hitmask=0
      end
    end
  end)
  s:loopAnime(0.5,ti.tile[1])
  s:draw()
  bang:idle()
  bang:draw()
  dsc(score)
end

function movefighter()
  s.muteki=s.muteki-1
  if s.muteki<0 then s.muteki=0 end
  s:drag(sc:arrayto(pptouch()),base)
end

function drawfps()
  local s=string.format("%.1f",ppgame:fps())
  local b=ppfont:size(s)
  b.x,b.y=-1,-1
  g:locate(g:layout(b,false,false,base))
  g:print(s)
end

initse=[[
T300
o7v9
]]

ppsemml[1]:play(initse)
ppsemml[2]:play(initse)
ppsemml[3]:play(initse)

playse=[[
@15-1o6v15g4<l16crcrc
]]

bangse=[[
@7
o1v15l8dcdcv7cv5cv3cv1c
]]

hitse=[[
@15-1o8l16v9cgabgdc
]]

shotse=[[
@15-1o4l16v9cd
]]
