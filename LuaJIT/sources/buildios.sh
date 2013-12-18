cd LuaJIT-2.0.2

IXCODE=`xcode-select -print-path`
ISDK=$IXCODE/Platforms/iPhoneOS.platform/Developer
ISDKVER=iPhoneOS7.0.sdk

ISDKP=$IXCODE/Toolchains/XcodeDefault.xctoolchain/usr/bin/
ISDKF="-arch armv7 -isysroot $ISDK/SDKs/$ISDKVER"
make CC="clang" BUILDMODE="static" HOST_CC="clang -m32 -arch i386" CROSS=$ISDKP TARGET_FLAGS="$ISDKF" TARGET_SYS=iOS clean all
cp -p src/libluajit.a ../output-ios/libluajit-armv7.a

ISDKF="-arch armv7s -isysroot $ISDK/SDKs/$ISDKVER"
make CC="clang" BUILDMODE="static" HOST_CC="clang -m32 -arch i386" CROSS=$ISDKP TARGET_FLAGS="$ISDKF" TARGET_SYS=iOS clean all
cp -p src/libluajit.a ../output-ios/libluajit-armv7s.a

make CC="clang -m32 -arch i386" clean all
cp -p src/libluajit.a ../output-ios/libluajit-i386.a

mkdir ../output-ios/LuaJIT.framework
mkdir ../output-ios/LuaJIT.framework/Headers

cp src/luajit.h src/luaconf.h src/lua.h src/lua.hpp src/lauxlib.h src/lualib.h ../output-ios/LuaJIT.framework/Headers

lipo -create ../output-ios/libluajit-*.a -output ../output-ios/LuaJIT.framework/LuaJIT
cp -r ../output-ios/LuaJIT.framework ../../iOS/LuaJIT/Products/

cd ..