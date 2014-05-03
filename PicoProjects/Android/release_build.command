#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)
cd "$SRCROOT"

rm -r PicoPicoGamesAndroid
rm -r PicoPicoGamesAndroidJIT
mkdir -p PicoPicoGamesAndroid
mkdir -p PicoPicoGamesAndroidJIT

cp -r ./Android/* ./PicoPicoGamesAndroid
cp -r ./Android/* ./PicoPicoGamesAndroidJIT

cp -r ../Readme/* ./PicoPicoGamesAndroid
cp -r ../Readme/* ./PicoPicoGamesAndroidJIT

cp -r ../../FullBuild/Android2/libs/* ./PicoPicoGamesAndroid/libs
cp -r ../../FullBuild/AndroidJIT/libs/* ./PicoPicoGamesAndroidJIT/libs

cp ../SampleGame/* ./PicoPicoGamesAndroid/assets
cp ../SampleGame/* ./PicoPicoGamesAndroidJIT/assets
