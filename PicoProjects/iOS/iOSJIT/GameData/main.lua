ti=require("tileinfo")
g=ppgraph
sc=ppscreen
base=pprect(0,0,320,480)

sgameover="GAME OVER"
score_key="carrace-hiscore"
stitle="CAR RACE"
seadd="t200o5@1l16q8cde"
seset="t180o4v8@1l16q8e"
sebang="t120o6@1l8q6grgrgrgrg"
sehiscore="t120o6@1l16q6ceg<ceg<ceg"
sestart="t120o5@1l16q8cdercdercdefg4"
bgobj={5,8,9}

scrollspeed=8
hiscore=ppgame:getInteger(score_key,0)
score=0

mv={{0,0},{1,0},{0,-1},{-1,0},{0,1}}

ppsprite.drawAnime = function(self)
  self:loopAnime(0.5,ti.tile[self.anime])
  self:draw()
end
mycar=ppsprite.new()
mycar.anime=1
mycar.hitrect=pprect(6,6,32-12,32-12)
mycar.hitmask=1
bang=ppsprite.new()
bang.anime=7
bang:scale(2)
bang:pivot(16,16)
map=ppmap.new(2)
map[1]:load("map1.tmx")
map[1]:tileInfo(ti)
map[2]:load("map1.tmx")
map[2]:tileInfo(ti)
mapy=0
maph=map[1]:size().height
ecar=ppsprite.new(15)
e={}
e.a=ecar
e.set=function(self)
  return ppforeach(self.a,function(i,v)
    if (not v:isVisible()) then
      v:show()
      v.anime=3
      v.y=-32
      v.x=mycar.x
      v.dx=math.random(-20,20)/10
      v.dy=math.random(10,50)/5
      v.hitrect=pprect(6,6,32-12,32-12)
      v.hitmask=1
      if v.dx<-1.5 or v.dx>1.5 then
        v.anime=4
      end
      if v.dy>8 then
        v.anime=2
      end
      v.pass=false
      return v
    end
  end)
end
e.setblock=function(self)
  for i=0,2 do
    local v=self:set()
    if v==nil then return end
    v.anime=6
    v.dx=0
    v.dy=scrollspeed
    v.y=v.y-i*32
  end
end

function bgcheck(r)
  if checkbgpos(r:center()) then return true end
end

function checkbgpos(p)
  p.y=p.y-mapy
  while p.y<0 do
    p.y=p.y+map[1]:size().height
  end
  return map[1]:check(map[1]:tile(p.x/32,p.y/32),bgobj)
end

function dsc(str,sc)
 if sc>hiscore then str="HI-SCORE" end
 local s=str.." "..sc
  local r=ppfont:size(s)
  r=g:layout(r,true,false,base)
  g:locate(r)
  g:fill(0,0,base.width,ppfont:height(),0,0,0,128)
  g:print(s)
end

function draw()
  map[1].x=(base.width-map[1]:size().width)/2
  map[2].x=map[1].x
  map[1].y=mapy
  map[2].y=mapy-maph
  map[1]:draw()
  map[2]:draw()
  mycar:drawAnime()
  ppforeach(ecar,function(i,v) v:drawAnime() end) 
  local s=string.format("%.1f",ppgame:fps())
  local p=ppfont:size(s)
  p.x=-1;p.y=-1
  g:locate(g:layout(p,false,false,base))
  g:print(s)
end

function reset()
   mapy=0
   bang:hide()
   ppforeach(ecar,function(i,v)
     v:texture(png)
     v.anime=3
     v.hitmask=0
     v:hide()
   end) 
  local r=mycar:aabb()
  r.x=0
  r.y=-100
  mycar:position(g:layout(r,true,false,base))
end

function title()
  reset()
  local b=ppbutton("START")
  b.y=-150
  b:layout(true,true,false,base)
  b.bg.tile=ti.tile[10][1]
  b.bg.edge=8
  while true do
    sc:viewport(sc:layout(base))
    draw()
    dsc("HI-SCORE",hiscore)
    local r=ppfont:size(stitle)
    local p=g:layout(r,true,true,base)
    g:pivot(base.width/2,base.height/2)
    g:locate(p)
    g:move(0,-24)
    g:scale(1.5,3)
    g:print(stitle,0,0,0)
    g:move(-1,-1)
    g:print(stitle,255,0,0)
    g:scale(1)
    g:pivot(0,0)
    if b:idle(sc:arrayto(pptouch())) then break end
    b:draw()
    g:update()
  end
end

function hiscoreget()
  ppsemml[1]:play(sehiscore)
  local tm=240
  while true do
    sc:viewport(sc:layout(base))
    draw()
    dsc("HI-SCORE",score)
    bang:drawAnime()
    g:locate(g:layout(sgameover,true,true,base))
    g:print(sgameover,255,0,0)
    if (tm%32)<16 then
      s="HI-SCORE!"
      g:locate(g:layout(s,true,true,base))
      g:move(0,-48)
      g:print(s,255,0,255)
    end
    tm=tm-1
    if tm<0 then
      return
    end
    g:update()
  end
end

function gameover()
  ppsemml[1]:play(sebang)
  local tm=140
  bang:show()
  bang:position(mycar)
  while true do
    sc:viewport(sc:layout(base))
    draw()
    dsc("SCORE",score)
    bang:drawAnime()
    g:locate(g:layout(sgameover,true,true,base))
    g:print(sgameover,255,0,0)
    tm=tm-1
    if tm<0 then
      if score>hiscore then
        hiscore=score
        ppgame:setInteger(score_key,hiscore)
        hiscoreget()
      end
      return
    end
    g:update()
  end
end

function mainloop()
  local t=0
  local tb=0
  while true do
    sc:viewport(sc:layout(base))
    t=t+1
    if (t%4)==0 then
      ppsemml[2]:stop()
      ppsemml[2]:play(seset)
    end
    if t>15 then
      tb=tb+1
      if tb>15 then
        tb=0
        e:setblock()
      end
      e:set()
      t=0
    end
    mycar:drag(sc:arrayto(pptouch()),base)
    mapy=mapy+scrollspeed
    if mapy>=maph then mapy=0 end
    ppforeach(ecar,function(i,v)
      if v:isVisible() then
        v:move(v.dx,v.dy)
        if v.y>base.height then
          v:hide()
          v.hitmask=0
          return
        end
        if bgcheck(v.hitrect+v:pos()) then
          v.dx=-v.dx
        end
        if v.pass==false then
          if v.y>mycar.y then
            score=score+1
            ppsemml[1]:stop()
            ppsemml[1]:play(seadd)
            v.pass=true
          end
        end
      end
    end)
    if bgcheck(mycar.hitrect+mycar:pos()) then
     return
    end
    local hit=false
    pphitcheck(ecar,{mycar},function(a,b)
      hit=true
    end)
    if hit then return end
    draw()
    dsc("SCORE",score)
    g:update()
  end
end

function start()
  title()
  score=0
  ppflmml[1]:preload(sestart)
  ppflmml[1]:play()
  while ppflmml[1]:isPlaying() do
    sc:viewport(sc:layout(base))
    mycar:drag(sc:arrayto(pptouch()),base)
    draw()
    dsc("SCORE",score)
    g:update()
  end
  
  mainloop()
  gameover()
end
