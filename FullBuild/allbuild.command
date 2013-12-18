#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)
cd "$SRCROOT"

cd "Android"
ndk-build clean
ndk-build
cd ..

cd "Android2"
ndk-build clean
ndk-build
cd ..

cd "AndroidJIT"
ndk-build clean
ndk-build
cd ..

cd "iOS"
build.command
cd ..

cd "OSX"
build.command
cd ..

cd "Win32"
cd PicoGame
make buildall
cd ../PicoGameLuaJIT
make buildall
cd ..
cd ..
