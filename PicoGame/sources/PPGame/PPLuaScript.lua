___scriptIdleFunc=nil
function __startIdleFunc(func)
	if ___scriptIdleFunc~=nil then
		___scriptCorutine = coroutine.create(___scriptIdleFunc)
		return true
	end
	return false
end
function __resumeScript()
	if ___scriptCorutine~=nil then
		if coroutine.status(___scriptCorutine) == "dead" then
		else
			local f,m = coroutine.resume(___scriptCorutine)
			if (not f) then __PPLuaScriptSetErrorMessage(m) end
		end
	end
	return 0
end
function __statusScript()
	if ___scriptCorutine~=nil then
		if coroutine.status(___scriptCorutine) == "dead" then
			return 0
		else
			return 1
		end
	end
	return 0
end
