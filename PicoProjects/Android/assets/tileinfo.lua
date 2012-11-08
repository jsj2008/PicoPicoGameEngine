ppmap.tileInfo = function(self,tileInfo)
	if tileInfo~=nil then
		if self:tileset()[1].name == "sample.png" then
			self._ctileinfo = tileInfo.sub
		else
			self._ctileinfo = tileInfo.main
		end
		self._tileinfo=tileInfo
	end
	return self._tileinfo
end
ppmap.check = function(self,tile,s,e)
	if tile <= 0 then return false end
	if self._ctileinfo==nil then return false end
	if e==nil then e = s end
	if type(s) == "table" then
		for k,v in pairs(s) do
			if self._ctileinfo[v] > 0 then
				if self._ctileinfo[v]==tile then return true end
			end
		end
	else
		for i=s,e do
			if self._ctileinfo[i] > 0 then
				if self._ctileinfo[i]==tile then return true end
			end
		end
	end
	return false
end
local characters = {
	{2},
	{3},
	{4},
	{5},
	{6},
	{7},
	{8,9},
	{10},
	{11},
	{12},
	{12},
	{2},
	{4},
	{5},
	{3},
	{6},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
}

local bgInfoArray1 = {
 18, 21, 19, 20, 22, 23, 72, 24,
 26, 17,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0
}
local bgInfoArray2 = {
  1,  2,  3,  4,  5,  6,  7,  8,
  9, 10, 17, 18, 19, 20, 21, 22,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1
}
local tileInfo = {
	tile=characters,
	main=bgInfoArray1,
	sub=bgInfoArray2,
	loop=function(self,o,s,i)
		o:loopAnime(s,self.tile[i])
	end,
	time=function(self,o,t,i)
		if o.animationTime==nil then o.animationTime=1 end
		if t==nil then
		else
		o.animationTime=t
		end
		if (o.animationTime <= 1) then o.animationTime=1 end
		if (o.animationTime >= #self.tile[i]+1) then o.animationTime = #self.tile[i]+1 end
		return o.animationTime
	end,
	play=function(self,o,s,i)
		return o:playAnime(s,self.tile[i])
	end
}
pptex:default(pptex:load("main.png",false))
return tileInfo
