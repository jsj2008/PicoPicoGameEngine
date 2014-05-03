#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)
cd "$SRCROOT"

rm -r PicoPicoGamesiOS
mkdir -p PicoPicoGamesiOS
rm -r PicoPicoGamesiOSJIT
mkdir -p PicoPicoGamesiOSJIT

rm ./iOS/include/PicoGame/*

cp ../../PicoGame/sources/Platforms/Cocoa/PPGame.h ./iOS/include/PicoGame/
cp ../../PicoGame/sources/Platforms/iOS/PPGameView.h ./iOS/include/PicoGame/
cp ../../PicoGame/sources/Platforms/iOS/PPGameViewController.h ./iOS/include/PicoGame/
cp ../../PicoGame/sources/PPGame/PPGameDef.h ./iOS/include/PicoGame/
cp ../../PicoGame/sources/PPGame/PPGameUtil.h ./iOS/include/PicoGame/

rm ./iOS/library/*

rm ./iOSJIT/include/PicoGame/*

cp ../../PicoGame/sources/Platforms/Cocoa/PPGame.h ./iOSJIT/include/PicoGame/
cp ../../PicoGame/sources/Platforms/iOS/PPGameView.h ./iOSJIT/include/PicoGame/
cp ../../PicoGame/sources/Platforms/iOS/PPGameViewController.h ./iOSJIT/include/PicoGame/
cp ../../PicoGame/sources/PPGame/PPGameDef.h ./iOSJIT/include/PicoGame/
cp ../../PicoGame/sources/PPGame/PPGameUtil.h ./iOSJIT/include/PicoGame/

rm ./iOSJIT/library/*

cp ../../FullBuild/iOS/libpicoengine.a ./iOS/library
cp ../../FullBuild/iOS/libpicoenginejit.a ./iOSJIT/library

cp -r ./iOS/* ./PicoPicoGamesiOS
cp -r ./iOSJIT/* ./PicoPicoGamesiOSJIT

cp -r ../Readme/* ./PicoPicoGamesiOS
cp -r ../Readme/* ./PicoPicoGamesiOSJIT

rm -r ./PicoPicoGamesiOS/PicoPicoGame.xcodeproj/project.xcworkspace
rm -r ./PicoPicoGamesiOS/PicoPicoGame.xcodeproj/xcuserdata

rm -r ./PicoPicoGamesiOSJIT/PicoPicoGame.xcodeproj/project.xcworkspace
rm -r ./PicoPicoGamesiOSJIT/PicoPicoGame.xcodeproj/xcuserdata

rm ./PicoPicoGamesiOS/GameData/*
cp ../SampleGame/* ./PicoPicoGamesiOS/GameData

rm ./PicoPicoGamesiOSJIT/GameData/*
cp ../SampleGame/* ./PicoPicoGamesiOSJIT/GameData
