cd LuaJIT-2.0.2

make BUILDMODE="static" PREFIX=@executable_path/../SharedSupport/luajit clean all
cp -p src/libluajit.a ../output-osx/libluajit-x86_64.a

make BUILDMODE="static" CC="clang -m32" PREFIX=@executable_path/../SharedSupport/luajit clean all
cp -p src/libluajit.a ../output-osx/libluajit-i386.a

mkdir ../output-osx/LuaJIT.framework
mkdir ../output-osx/LuaJIT.framework/Headers

cp src/luajit.h src/luaconf.h src/lua.h src/lua.hpp src/lauxlib.h src/lualib.h ../output-osx/LuaJIT.framework/Headers

lipo -create ../output-osx/libluajit-x86_64.a ../output-osx/libluajit-i386.a -output ../output-osx/LuaJIT.framework/LuaJIT 
cp -r ../output-osx/LuaJIT.framework ../../OSX/LuaJIT/Products/

cd ..