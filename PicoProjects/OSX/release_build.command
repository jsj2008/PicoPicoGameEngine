#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)
cd "$SRCROOT"

cp ../../FullBuild/OSX/libpicoengine.a ./OSX/library
#cp ../../FullBuild/OSX/libpicoengineio.a ./OSX/library
cp ../../FullBuild/OSX/libpicoenginejit.a ./OSX/library

rm ./OSX/include/PicoGame/*
cp ../../PicoGame/OSX/PicoGame/Products/PicoGame.framework/Versions/A/Headers/* ./OSX/include/PicoGame/

cd OSX

xcodebuild -configuration "Release" -target "PicoPicoGamesIO"
xcodebuild -configuration "Release" -target "PicoPicoGamesJIT"
xcodebuild -configuration "Release" -target "PicoPicoGames"

cd ..

rm -r PicoPicoGamesOSX
rm -r PicoPicoGamesOSXJIT
mkdir -p PicoPicoGamesOSX
mkdir -p PicoPicoGamesOSXJIT

cp -r ./OSXAPP/* ./PicoPicoGamesOSX
cp -r ./OSXAPP/* ./PicoPicoGamesOSXJIT

CpMac -r ./OSX/Build/Release/PicoPicoGames.app ./PicoPicoGamesOSX
CpMac -r ./OSX/Build/Release/PicoPicoGamesJIT.app ./PicoPicoGamesOSXJIT
rm -r ./PicoPicoGamesOSX/PicoPicoGames.app/Contents/Resources/GameData
rm -r ./PicoPicoGamesOSXJIT/PicoPicoGamesJIT.app/Contents/Resources/GameData

cp ../SampleGame/* ./PicoPicoGamesOSX/Resources
cp ../SampleGame/* ./PicoPicoGamesOSXJIT/Resources
