cd LuaJIT-2.0.2

NDK="$ANDROID_NDK_ROOT"

# Android/ARM, armeabi (ARMv5TE soft-float), Android 2.2+ (Froyo)
NDKABI=8
NDKVER=$NDK/toolchains/arm-linux-androideabi-4.6
NDKP=$NDKVER/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-
NDKF="--sysroot $NDK/platforms/android-$NDKABI/arch-arm"
OUTPUT_DIR=`pwd`/../output-android/"armeabi"
make HOST_CC="gcc -m32" CROSS=$NDKP TARGET_FLAGS="$NDKF" TARGET_SYS=Linux clean all
cp src/libluajit.a ../output-android/lib/armeabi/libluajit-5.1.a

# Android/ARM, armeabi-v7a (ARMv7 VFP), Android 4.0+ (ICS)
NDKABI=14
NDKVER=$NDK/toolchains/arm-linux-androideabi-4.6
NDKP=$NDKVER/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-
NDKF="--sysroot $NDK/platforms/android-$NDKABI/arch-arm"
NDKARCH="-march=armv7-a -mfloat-abi=softfp -Wl,--fix-cortex-a8"
OUTPUT_DIR=`pwd`/../output-android/"armeabi-v7a"
make HOST_CC="gcc -m32" CROSS=$NDKP TARGET_FLAGS="$NDKF $NDKARCH" TARGET_SYS=Linux clean all
cp src/libluajit.a ../output-android/lib/armeabi-v7a/libluajit-5.1.a

# Android/MIPS, mips (MIPS32R1 hard-float), Android 4.0+ (ICS)
NDKABI=14
NDKVER=$NDK/toolchains/mipsel-linux-android-4.6
NDKP=$NDKVER/prebuilt/darwin-x86_64/bin/mipsel-linux-android-
NDKF="--sysroot $NDK/platforms/android-$NDKABI/arch-mips"
OUTPUT_DIR=`pwd`/../output-android/"mips"
make HOST_CC="gcc -m32" CROSS=$NDKP TARGET_FLAGS="$NDKF" TARGET_SYS=Linux clean all
cp src/libluajit.a ../output-android/lib/mips/libluajit-5.1.a

# Android/x86, x86 (i686 SSE3), Android 4.0+ (ICS)
NDKABI=14
NDKVER=$NDK/toolchains/x86-4.6
NDKP=$NDKVER/prebuilt/darwin-x86_64/bin/i686-linux-android-
NDKF="--sysroot $NDK/platforms/android-$NDKABI/arch-x86"
OUTPUT_DIR=`pwd`/../output-android/"x86"
make HOST_CC="gcc -m32" CROSS=$NDKP TARGET_FLAGS="$NDKF" TARGET_SYS=Linux clean all
cp src/libluajit.a ../output-android/lib/x86/libluajit-5.1.a

cp src/luajit.h src/luaconf.h src/lua.h src/lua.hpp src/lauxlib.h src/lualib.h ../output-android/include

cp -r ../output-android/include ../../Android/LuaJIT/
cp -r ../output-android/lib ../../Android/LuaJIT/

cd ..