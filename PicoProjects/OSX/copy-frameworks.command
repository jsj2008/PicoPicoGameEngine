#!/bin/bash
cd `dirname $0`

rm -rf ./Frameworks/
mkdir -p ./Frameworks
mkdir -p ./Frameworks/Lua/LuaJIT
mkdir -p ./Frameworks/Lua/Lua5.2
#mkdir -p ./Frameworks/Lua/Lua5.2-NoIO
mkdir -p ./Frameworks/PicoGame/Lua5.2
mkdir -p ./Frameworks/PicoGame/LuaJIT

CpMac -r ../../Box2D/OSX/Box2D/Products/* ./Frameworks/
CpMac -r ../../CocosDenshion/OSX/CocosDenshion/Products/* ./Frameworks/
CpMac -r ../../FlMML/OSX/FlMML/Products/* ./Frameworks/
CpMac -r ../../freetype/OSX/freetype/Products/* ./Frameworks/
CpMac -r ../../Lua5.2/OSX/Lua/Products/* ./Frameworks/Lua/Lua5.2/
#CpMac -r ../../Lua5.2/OSX/Lua/Products-noIO/* ./Frameworks/Lua/Lua5.2-NoIO/
CpMac -r ../../LuaJIT/OSX/LuaJIT/Products/* ./Frameworks/Lua/LuaJIT/
CpMac -r ../../PicoGame/OSX/PicoGame/Products/* ./Frameworks/PicoGame/Lua5.2/
CpMac -r ../../PicoGame/OSX/PicoGame/ProductsLuaJIT/* ./Frameworks/PicoGame/LuaJIT/
