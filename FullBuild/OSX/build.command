#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)

cd "$SRCROOT"
rm libpicoengine.a
rm libpicoenginejit.a

cd "$SRCROOT"
cd ../../Box2D/OSX/Box2D/
xcodebuild -target Universal

cd "$SRCROOT"
cd ../../CocosDenshion/OSX/CocosDenshion/
xcodebuild -target Universal

cd "$SRCROOT"
cd ../../FlMML/OSX/FlMML/
xcodebuild -target Universal

cd "$SRCROOT"
cd ../../freetype/OSX/freetype/
xcodebuild -target Universal

cd "$SRCROOT"
cd ../../Lua5.2/OSX/Lua/
xcodebuild -target Universal

#cd "$SRCROOT"
#cd ../../Lua5.2/OSX/Lua/
#xcodebuild -target "Universal no I/O"

cd "$SRCROOT"
cd ../../PicoGame/OSX/PicoGame/
xcodebuild -target Universal

cd "$SRCROOT"
cd ../../PicoGame/OSX/PicoGame/
xcodebuild -target UniversalLuaJIT

cd "$SRCROOT"
cd ../../Vorbis/OSX/Vorbis/
xcodebuild -configuration "Release" -target Vorbis

cd "$SRCROOT"
root_path="../.."

Box2D="$root_path/Box2D/OSX/Box2D/Products/Box2D.framework/Versions/A/Box2D"
CocosDenshion="$root_path/CocosDenshion/OSX/CocosDenshion/Products/CocosDenshion.framework/Versions/A/CocosDenshion"
FlMML="$root_path/FlMML/OSX/FlMML/Products/FlMML.framework/Versions/A/FlMML"
Lua="$root_path/Lua5.2/OSX/Lua/Products/Lua.framework/Versions/A/Lua"
#LuaNoIO="$root_path/Lua5.2/OSX/Lua/Products-noIO/Lua.framework/Versions/A/Lua"
LuaJIT="$root_path/LuaJIT/OSX/LuaJIT/Products/LuaJIT.framework/LuaJIT"
PicoGame="$root_path/PicoGame/OSX/PicoGame/Products/PicoGame.framework/Versions/A/PicoGame"
PicoGameJIT="$root_path/PicoGame/OSX/PicoGame/ProductsLuaJIT/PicoGame.framework/Versions/A/PicoGame"
freetype="$root_path/freetype/OSX/freetype/Products/freetype.framework/Versions/A/freetype"
Vorbis="$root_path/Vorbis/OSX/Vorbis/Products/Vorbis.framework/Versions/A/Vorbis"

#libtool -static -o libpicoengineio.a "$Box2D" "$CocosDenshion" "$FlMML" "$Lua" "$PicoGame" "$freetype" "$Vorbis"
libtool -static -o libpicoengine.a "$Box2D" "$CocosDenshion" "$FlMML" "$Lua" "$PicoGame" "$freetype" "$Vorbis"
libtool -static -o libpicoenginejit.a "$Box2D" "$CocosDenshion" "$FlMML" "$LuaJIT" "$PicoGameJIT" "$freetype" "$Vorbis"
