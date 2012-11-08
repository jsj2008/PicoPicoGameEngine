#!/bin/bash
cd `dirname $0`

rm -rf ./Frameworks/
mkdir -p ./Frameworks
CpMac -r ../../Box2D/OSX/Box2D/Products/* ./Frameworks/
CpMac -r ../../CocosDenshion/OSX/CocosDenshion/Products/* ./Frameworks/
CpMac -r ../../FlMML/OSX/FlMML/Products/* ./Frameworks/
CpMac -r ../../freetype/OSX/freetype/Products/* ./Frameworks/
CpMac -r ../../Lua5.2/OSX/Lua/Products/* ./Frameworks/
CpMac -r ../../PicoGame/OSX/PicoGame/Products/* ./Frameworks/
