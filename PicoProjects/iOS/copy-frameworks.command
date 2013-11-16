#!/bin/bash
cd `dirname $0`

rm -rf ./Frameworks/
mkdir -p ./Frameworks
mkdir -p ./Frameworks/Lua/LuaJIT
mkdir -p ./Frameworks/Lua/Lua5.2
mkdir -p ./Frameworks/Lua/Lua5.2-NoIO
mkdir -p ./Frameworks/PicoGame/Lua5.2
mkdir -p ./Frameworks/PicoGame/LuaJIT

CpMac -r ../../Box2D/iOS/Box2D/Products/* ./Frameworks/
CpMac -r ../../CocosDenshion/iOS/CocosDenshion/Products/* ./Frameworks/
CpMac -r ../../FlMML/iOS/FlMML/Products/* ./Frameworks/
CpMac -r ../../freetype/iOS/freetype/Products/* ./Frameworks/
CpMac -r ../../Lua5.2/iOS/Lua/Products/* ./Frameworks/Lua/Lua5.2/
CpMac -r ../../Lua5.2/iOS/Lua/Products-noIO/* ./Frameworks/Lua/Lua5.2-NoIO/
CpMac -r ../../LuaJIT/iOS/LuaJIT/Products/* ./Frameworks/Lua/LuaJIT/
CpMac -r ../../PicoGame/iOS/PicoGame/Products/* ./Frameworks/PicoGame/Lua5.2/
CpMac -r ../../PicoGame/iOS/PicoGame/ProductsLuaJIT/* ./Frameworks/PicoGame/LuaJIT/
