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
}

local bgInfoArray1 = {
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
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0
}
local bgInfoArray2 = {
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
