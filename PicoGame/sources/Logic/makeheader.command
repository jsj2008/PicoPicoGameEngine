#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)
cd "$SRCROOT"

xxd -i -u PPScriptGame.lua | sed "s/}\;/, 0x00}\;/" | sed s/PPScriptGame_lua/EMBEDDED_LUA_GAME_CODE/ | sed s/0X/0x/g > PPScriptGameEmbbed.h
