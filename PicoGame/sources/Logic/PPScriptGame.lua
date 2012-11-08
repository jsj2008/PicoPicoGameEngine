pppointImp = {
	length=function(self,x,y)
		return pplength(self,x,y)
	end
}
pppoint_mt = {
	__add = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x + b
			c.y = a.y + b
		else
			c.x = a.x + b.x
			c.y = a.y + b.y
		end
		return pppoint(c)
	end,
	__sub = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x - b
			c.y = a.y - b
		else
			c.x = a.x - b.x
			c.y = a.y - b.y
		end
		return pppoint(c)
	end,
	__div = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x / b
			c.y = a.y / b
		else
			c.x = a.x / b.x
			c.y = a.y / b.y
		end
		return pppoint(c)
	end,
	__mul = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x * b
			c.y = a.y * b
		else
			c.x = a.x * b.x
			c.y = a.y * b.y
		end
		return pppoint(c)
	end,
	__index = pppointImp
}
ppobject.hitCheck = function(self,points,hitCheck)
	local ret=false
	if (hitCheck == nil) then
		hitCheck = function(a,v)
			return a:contain(v)
		end
	end
	if #points > 0 then
		self.hit = false
		for k,v in pairs(points) do
			if hitCheck(self,v) then
				self.hit = true
				if self.pretouchCount == 0 then
					self.trigger = true
				end
				break
			end
		end
	else
		if self.hit then
			self.hit = false
			if self.trigger then
				ret = true
			end
		end
		self.trigger = false
	end
	self.touch = self.trigger and self.hit
	self.pretouchCount = #points
	return ret
end
pprectImp = {
	min = function(self)
		return pppoint(self.x,self.y)
	end,
	max = function(self)
		return pppoint(self.x+self.width,self.y+self.height)
	end,
	center = function(self)
		return pppoint(self.x+self.width/2,self.y+self.height/2)
	end,
	equalToSize = function(self,a)
		return (self.width == a.width and self.height == a.height)
	end,
	equalToRect = function(self,a)
		return (self.x == a.x and self.y == a.y and self.width == a.width and self.height == a.height)
	end,
	isEmpty = function(self)
		return (self.width == 0 and self.height == 0)
	end,
	move = function(self,x,y)
		local p = pppoint(x,y)
		self.x = self.x+p.x
		self.y = self.y+p.y
	end,
	position = function(self,x,y)
		if x~=nil then
			local p = pppoint(x,y)
			self.x = p.x
			self.y = p.y
		end
		return pppoint(self.x,self.y)
	end,
	pos = function(self,x,y)
		if x~=nil then
			local p = pppoint(x,y)
			self.x = p.x
			self.y = p.y
		end
		return pppoint(self.x,self.y)
	end,
	size = function(self,x,y)
		if x~=nil then
			local width=x
			local height=y
			if (type(x) == "table") then
				if (x.width == nil) then
					width = x[1]
				else
					width = x.width
				end
				if (x.height == nil) then
					height = x[2]
				else
					height = x.height
				end
			end
			if width==nil then width=0 end
			if height==nil then height=0 end
			self.width = width
			self.height = height
		end
		return {width=self.width,height=self.height}
	end,

	scale = function(self,a)
		local r = pprect(self)
		r.width  = r.width *a
		r.height = r.height*a
		self.width = r.width
		self.height = r.height
		return r
	end,
	inset = function(self,x,y)
		local r = pprect(self)
		if y==nil then y=x end
		local d = pppoint(x,y)
		r.x = r.x + d.x
		r.y = r.y + d.y
		self.x = r.x
		self.y = r.y
		if (self.width  ~= nil) then
		  r.width  = r.width  - d.x*2
		  self.width = r.width;
		end
		if (self.height ~= nil) then
		  r.height = r.height - d.y*2
		  self.height = r.height;
		end
		return r
	end,

	contain = function(self,x,y)
		local p = pppoint(x,y)
		if y==nil then y=x end
		local min1 = self:min()
		local max1 = self:max()
		return (min1.x <= p.x and min1.y <= p.y and p.x < max1.x and p.y < max1.y)
	end,
	intersect = function(self,r)
		local wx = self.x - r.x + self.width
		local wy = self.y - r.y + self.height
		return (wx >= 0 and wx < self.width+r.width and wy >= 0 and wy < self.height+r.height)
	end,
	union = function(self,r2)
		local min1 = self:min()
		local max1 = self:max()
		local min2 = r2:min()
		local max2 = r2:max()
		if (min1.x > min2.x) then min1.x = min2.x end
		if (min1.y > min2.y) then min1.y = min2.y end
		if (max1.x < max2.x) then max1.x = max2.x end
		if (max1.y < max2.y) then max1.y = max2.y end
		return pprect(min1.x,min1.y,max1.x-min1.x,max1.y-min1.y)
	end,
	length=function(self,x,y)
		return pplength(self,x,y)
	end,
	hitCheck = ppobject.hitCheck
}
pprect_mt = {
	__add = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x + b
			c.y = a.y + b
		else
			c.x = a.x + b.x
			c.y = a.y + b.y
		end
		c.width = a.width
		c.height = a.height
		return pprect(c)
	end,
	__sub = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x - b
			c.y = a.y - b
		else
			c.x = a.x - b.x
			c.y = a.y - b.y
		end
		c.width = a.width
		c.height = a.height
		return pprect(c)
	end,
	__div = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x / b
			c.y = a.y / b
			c.width = a.width / b
			c.height = a.height / b
		else
			c.x = a.x / b.x
			c.y = a.y / b.y
			c.width = a.width / b.x
			c.height = a.height / b.y
		end
		return pprect(c)
	end,
	__mul = function(a,b)
		local c = {}
		if type(b) == "number" then
			c.x = a.x * b
			c.y = a.y * b
			c.width = a.width * b
			c.height = a.height * b
		else
			c.x = a.x * b.x
			c.y = a.y * b.y
			c.width = a.width * b.x
			c.height = a.height * b.y
		end
		return pprect(c)
	end,
	__index = pprectImp
}
ppsprite = ppobject
function ppscreen:from(t)
	local r=pprect(t)
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
function ppscreen:to(t)
	local r=pprect(t)
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
ppgraph.white={255,255,255}
ppgraph.red={255,0,0}
ppgraph.green={0,255,0}
ppgraph.blue={0,0,255}
ppgraph.yellow={255,255,0}
ppgraph.cyan={0,255,255}
ppgraph.magenta={255,0,255}
ppgraph.black={0,0,0}
ppgraph.gray={96,96,96}
ppgraph.lightgray={188,188,188}
ppgraph.orange={255,128,0}
ppgraph.skin={255,216,160}
ppgraph.darkgreen={56,104,0}
ppgraph.lightgreen={152,232,0}
ppgraph.brown={120,64,0}

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
	  ppgraph:locate(p)
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
 if tex==nil then
    tex=pptex:default()
 end
 if type(tile)~="table" then
 	tile=self:tileRect(tile)
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
end

function ppgraph:tileRect(tile,tex)
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
  return r
end
