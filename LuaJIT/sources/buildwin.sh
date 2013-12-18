cd LuaJIT-2.0.2

MINGW=/Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-
CC="$MINGW"gcc
AR="$MINGW"ar

make HOST_CC="gcc -m32 -arch i386" BUILDMODE="static" CROSS="$MINGW" TARGET_SYS=Windows clean all

mkdir ../output-win/include

cp src/luajit.h src/luaconf.h src/lua.h src/lua.hpp src/lauxlib.h src/lualib.h ../output-win/include
cp src/libluajit.a ../output-win/libluajit-5.1.a

cp -r ../output-win/include ../../Win32/LuaJIT/
cp -r ../output-win/libluajit-5.1.a ../../Win32/LuaJIT/libluajit-5.1.a

cd ..