ppobject.hitCheck=function(s,p,hitCheck)
  local ret=false
  local pretrigger=s.trigger
  local pretouch=s.touch
  if (hitCheck ==nil) then
    hitCheck=function(a,v)
      return a:contain(v)
    end
  end
  function findtouch(t,p)
    if t and p then
      for i,v in ipairs(p) do
        if v:length(t)<128 then
          return v
        end
      end
    end
    return nil
  end
  local prehit=s.hit
  s.hit=false
  if p and #p>0 then
    for i,v in ipairs(p) do
      if hitCheck(s,v) then
        s.touchtrackpos=v
        s.hit=true
        break
      end
    end
  end
  if #p~=s.touchCount then
    if not prehit and s.hit then
      s.trigger=true
    end
  end
  if s.hit and s.trigger then
    s.touch=true
  else
    s.touch=false
  end
  local np=findtouch(s.touchtrackpos,p)
  s.touchtrackpos=np
  if not np then
    s.trigger=false
  end
  if pretrigger and not s.trigger then
    if pretouch and not s.touch then
      ret=true
    end
  end
  s.touchCount=#p
  return ret
end
ppsprite = ppobject
function ppscreen:from(...)
	local r=pprect(...)
	r.x = r.x - self:pivot().x
	r.y = r.y - self:pivot().y
	r.x = r.x * self:scale().x
	r.y = r.y * self:scale().y
	r.x = r.x + self:pivot().x + self:offset().x
	r.y = r.y + self:pivot().y + self:offset().y
	r.width = r.width * self:scale().x
	r.height = r.height * self:scale().y
	return r
end
function ppscreen:to(...)
	local r=pprect(...)
	r.x = r.x - self:offset().x
	r.y = r.y - self:offset().y
	r.x = r.x - self:pivot().x
	r.y = r.y - self:pivot().y
	r.x = r.x / self:scale().x
	r.y = r.y / self:scale().y
	r.x = r.x + self:pivot().x
	r.y = r.y + self:pivot().y
	r.width = r.width / self:scale().x
	r.height = r.height / self:scale().y
	return r
end
function ppscreen:arrayto(t)
	local r={}
	for i=1,#t do
		local p=self:to(pprect(t[i]))
		r[i] = p
	end
	return r
end
function ppscreen:arrayfrom(t)
	local r={}
	for i=1,#t do
		local p=self:from(pprect(t[i]))
		r[i] = p
	end
	return r
end
ppgraph.white={r=255,g=255,b=255,a=255}
ppgraph.red={r=255,g=0,b=0,a=255}
ppgraph.green={r=0,g=255,b=0,a=255}
ppgraph.blue={r=0,g=0,b=255,a=255}
ppgraph.yellow={r=255,g=255,b=0,a=255}
ppgraph.cyan={r=0,g=255,b=255,a=255}
ppgraph.magenta={r=255,g=0,b=255,a=255}
ppgraph.black={r=0,g=0,b=0,a=255}
ppgraph.gray={r=96,g=96,b=96,a=255}
ppgraph.lightgray={r=188,g=188,b=188,a=255}
ppgraph.orange={r=255,g=128,b=0,a=255}
ppgraph.skin={r=255,g=216,b=160,a=255}
ppgraph.darkgreen={r=56,g=104,b=0,a=255}
ppgraph.lightgreen={r=152,g=232,b=0,a=255}
ppgraph.brown={r=120,g=64,b=0,a=255}

ppbutton=function(title)
	local a=pprect(0,0,0,0)
	a.title=title
	local s=ppfont:size(title)
	a.titleoffset=pppoint(0,0)
	a.width=s.width+24
	a.height=s.height+24
	a.bgcolor=ppgraph.black
	a.selectcolor=ppgraph.red
	a.color=ppgraph.white
	a.autolayout=false
	a.centerx=false
	a.centery=false
	a.layoutarea=nil
	a.hitrect=pprect(0,0,0,0)
	a.bg={tile=0,texture=nil,edge=4}
	a.idle=function(self,b)
	  if b==nil then b=pptouch() end
	  local ar=self:aabb()
	  local br=self.hitrect
	  br.x=ar.x
	  br.y=ar.y
	  br.width=ar.width
	  br.height=ar.height+16
	  if br:hitCheck(b) then
	    self.hit=br.hit
	    self.touch=br.touch
	    return true
	  end
	  self.hit=br.hit
	  self.touch=br.touch
	  return false
	end
	a.aabb=function(self)
	  local ar=pprect(self)
	  if self.autolayout then
	    local t=ppgraph:layout(
	                    self,
	                    self.centerx,
	                    self.centery,
	                    self.layoutarea)
	    ar:position(t)
	  end
	  return ar
	end
	a.drawbg=function(self,ar)
	  if self.bgcolor~= nil then
	    ppgraph:fill(ar,self.bgcolor)
	  end
	  if self.bg~=nil then
	    if self.bg.tile>0 then
	      ppgraph:stretch(self:aabb(),self.bg.tile,self.bg.edge,self.bg.texture)
	    end
	  end
	end
	a.draw=function(self)
	  local ar=self:aabb()
	  self:drawbg(ar)
	  local fb=ppfont:size(self.title)
	  fb:position(self.titleoffset)
	  local p=ppgraph:layout(fb,true,true,ar)
	  local op=ppgraph:locate()
	  ppgraph:locate(math.floor(p.x),math.floor(p.y))
	  if self.hitrect.touch then
	    ppgraph:print(self.title,self.selectcolor)
	  else
	    ppgraph:print(self.title,self.color)
	  end
	  ppgraph:locate(op)
	end
	a.size=function(self)
	  return pprect(0,0,self.width,self.height)
	end
	a.layout=function(self,auto,cx,cy,rx,ry,rw,rh)
	  self.autolayout=auto
	  self.centerx=cx
	  self.centery=cy
	  if rx==nil then
	    self.layoutarea=nil
	  else 
	    self.layoutarea=pprect(rx,ry,rw,rh)
	  end
	end
	setmetatable(a,{__index=pprectImp})
	return a
end

function ppgraph:stretch(rect,tile,edge,tex)
 local deftex=nil
 if tex~=nil then
  deftex=pptex:default()
  pptex:default(tex)
 end
 local left=nil
 local top=nil
 local right=nil
 local bottom=nil
 if type(edge)=="table" then
   left=edge.left
   top=edge.top
   right=edge.right
   bottom=edge.bottom
 else
   left,top,right,bottom=edge,edge,edge,edge
 end
 local g=self
 local ot=g:tileInfo()
 local r=pprect(rect)
 local p=r:position()
 if type(tile)~="table" then
 	tile=self:tileRect(tile,tex)
 end
 if type(tile)=="table" then
  if left==nil then left=0 end
  if top==nil then top=0 end
  if right==nil then right=0 end
  if bottom==nil then bottom=0 end
  local ts=tile:size()
  local tsize=pppoint(ts.width,ts.height)
  local tp=tile:position()
  
  if (tsize.x-(left+right))<=0 then return end
  if (tsize.y-(top+bottom))<=0 then return end

  g:tileInfo({offset=tp,size={width=left,height=top}})
  g:put(p.x,p.y,1)

  g:tileInfo({offset=tp+pppoint(tsize.x-right,0),size={width=right,height=top}})
  g:put(p.x+r.width-right,p.y,1)

  g:tileInfo({offset=tp+pppoint(tsize.x-right,tsize.y-bottom),size={width=right,height=bottom}})
  g:put(p.x+r.width-right,p.y+r.height-bottom,1)

  g:tileInfo({offset=tp+pppoint(0,tsize.y-bottom),size={width=left,height=bottom}})
  g:put(p.x,p.y+r.height-bottom,1)

  g:tileInfo({offset=tp+pppoint(left,top),size={width=tsize.x-(right+left),height=tsize.y-(top+bottom)}})
  g:put({p.x+left,p.y+top,r.width-(right+left),r.height-(top+bottom)},1)

  g:tileInfo({offset=tp+pppoint(left,0),size={width=tsize.x-(right+left),height=top}})
  g:put({p.x+left,p.y,r.width-(right+left),top},1)

  g:tileInfo({offset=tp+pppoint(0,top),size={width=left,height=tsize.y-(top+bottom)}})
  g:put({p.x,p.y+top,left,r.height-(top+bottom)},1)

  g:tileInfo({offset=tp+pppoint(tsize.x-right,top),size={width=right,height=tsize.y-(top+bottom)}})
  g:put({p.x+r.width-right,p.y+top,right,r.height-(top+bottom)},1)

  g:tileInfo({offset=tp+pppoint(left,tsize.y-bottom),size={width=tsize.x-(right+left),height=bottom}})
  g:put({p.x+left,p.y+r.height-bottom,r.width-(right+left),bottom},1)

 end
 g:tileInfo(ot)
 if deftex~=nil then
  pptex:default(deftex)
 end
end

function ppgraph:tileRect(tile,tex)
  local deftex=nil
  if tex~=nil then
    deftex=pptex:default()
    pptex:default(tex)
  end
  if tex==nil then
    tex=pptex:default()
  end
  local t=self:tileInfo()
  t.psize=pppoint(t.size.width+t.stride.x,t.size.height+t.stride.y)
  local r=pprect()
  local tw=math.floor(tex.size.width/t.psize.x)
  local i=tile-1
  r:position((pppoint(i%tw,math.floor(i/tw))*t.psize)+t.offset)
  r.width=t.size.width
  r.height=t.size.height
  if deftex~=nil then
    pptex:default(deftex)
   end
  return r
end

b2World.debug_pretouch = false
b2World.debug_dragMouseJoint = nil

b2World.debugDrag=function(world)
  if world.ground == nil then
    world.ground=world:createBody()
  end
  local tch=pptouch()
  local scale=world:drawScale()
  if (#tch > 0) then
    tch=ppscreen:arrayto(tch)
    if (not world.debug_pretouch) then
      world.debug_pretouch = true
      local pos = tch[1]/scale
      local hitfixture = nil
      local function callBack(fixture)
        local body=fixture.body
        if body.type == b2Body.dynamicBody then
          if fixture:testPoint(pos) then
            hitfixture = fixture
            return false
          end
        else
        end
        return true
      end
      local p1=pppoint(pos-pppoint(4,4)/scale)
      local p2=pppoint(pos+pppoint(4,4)/scale)
      world:queryAABB(p2,p1,callBack)

      if hitfixture then
        local body = hitfixture.body
        local joint=b2Joint.mouse(world.ground,body,{
          target=pos,
          maxForce=1000*body.mass})
        world.debug_dragMouseJoint=world:createJoint(joint)
        body.awake=true
      end
    else
      if world.debug_dragMouseJoint then
        local pos = tch[1]/scale
        world.debug_dragMouseJoint:target(pos)
      end
    end
  else
    if world.debug_dragMouseJoint then
      world:destroyJoint(world.debug_dragMouseJoint)
      world.debug_dragMouseJoint=nil
    end
    world.debug_pretouch = false
  end
end
