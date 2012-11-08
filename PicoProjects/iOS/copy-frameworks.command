#!/bin/bash
cd `dirname $0`

rm -rf ./Frameworks/
mkdir -p ./Frameworks
CpMac -r ../../Box2D/iOS/Box2D/Products/* ./Frameworks/
CpMac -r ../../CocosDenshion/iOS/CocosDenshion/Products/* ./Frameworks/
CpMac -r ../../FlMML/iOS/FlMML/Products/* ./Frameworks/
CpMac -r ../../freetype/iOS/freetype/Products/* ./Frameworks/
CpMac -r ../../Lua5.2/iOS/Lua/Products/* ./Frameworks/
CpMac -r ../../PicoGame/iOS/PicoGame/Products/* ./Frameworks/
