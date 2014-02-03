#!/bin/bash
SRCROOT=$(cd $(dirname $0);pwd)
cd "$SRCROOT"

xxd -i -u PPImageFont.png | sed s/PPImageFont_png/EMBEDDED_PPIMAGE_FONT/ | sed s/0X/0x/g > PPImageFontData.h
