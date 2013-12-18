#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)
cd "$SRCROOT"

rm -r PicoPicoGamesWin
rm -r PicoPicoGamesWinJIT
mkdir -p PicoPicoGamesWin
mkdir -p PicoPicoGamesWinJIT

cp -r ./Windows/* ./PicoPicoGamesWin
cp -r ./Windows/* ./PicoPicoGamesWinJIT

cp ../../FullBuild/Win32/PicoGame/picogame.exe ./PicoPicoGamesWin/PicoPicoGames.exe
cp ../../FullBuild/Win32/PicoGameLuaJIT/picogame.exe ./PicoPicoGamesWinJIT/PicoPicoGamesJIT.exe

cp ../SampleGame/* ./PicoPicoGamesWin/Resources
cp ../SampleGame/* ./PicoPicoGamesWinJIT/Resources
