CC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-gcc
AR = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-ar

CPPFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -D_WIN32 \
-DIGNORE_EXPORT -DGLEW_NO_GLU -D__MINGW32__ -DGLEW_STATIC -DREGEX_STATIC \
-DLUA_COMPAT_ALL -DCC_UNDER_WIN32 -DGL_GLEXT_PROTOTYPES=1 -D__USE_OPENGL10__ -DWIN32 -D_WINDOWS -DNDEBUG \
-O3 -DWINDOWS -D__USE_OGG_VORBIS__

BUILD_DIR = objects
BOX2D_DIR = ../../../Box2D/sources/Box2D_v2.3.0

LIBNAME = libpicogame.ar

CPPFLAGS += \
-I../../../CocosDenshion/sources/CocosDenshion/include/ \
-I../../../CocosDenshion/sources/CocosDenshion/ \
-I../../../FlMML/include/flmml/ \
-I../../../FlMML/include/fmgenAs/ \
-I../../../FlMML/include/other/ \
-I../../../FlMML/sources/FlMML/flmml/ \
-I../../../FlMML/sources/FlMML/other/ \
-I../../../FlMML/sources/FlMML/fmgenAs/ \
-I../../sources/Platforms/Windows/ \
-I../../sources/Platforms/Windroid/ \
-I../../sources/Logic/ \
-I../../sources/PPGame/ \
-I../../sources/PPImage/ \
-I../../../Lua5.2/sources/ \
-I$(BOX2D_DIR)/ \
-I$(BOX2D_DIR)/Box2D/ \
-I$(BOX2D_DIR)/Box2D/Collision/ \
-I$(BOX2D_DIR)/Box2D/Collision/Shapes/ \
-I$(BOX2D_DIR)/Box2D/Common/ \
-I$(BOX2D_DIR)/Box2D/Dynamics/ \
-I$(BOX2D_DIR)/Box2D/Dynamics/Contacts/ \
-I$(BOX2D_DIR)/Box2D/Dynamics/Joints/ \
-I$(BOX2D_DIR)/Box2D/Rope/ \
-I../../../freetype/sources/ \
-I../../../freetype/sources/freetype/ \
-I../../../png/sources/png/ \
-I../../../png/sources/zlib/ \
-I../../../Vorbis/sources/libogg-1.3.1/include/ \
-I../../../Vorbis/sources/libvorbis-1.3.3/include/ \
-I../../../Vorbis/sources/libvorbis-1.3.3/lib/

CFLAGS = $(CPPFLAGS) -std=gnu99

VPATH = \
../../sources/Platforms/Windows/: \
../../sources/Platforms/Windroid/: \
../../sources/Logic/: \
../../sources/PPGame/: \
../../sources/PPImage/:

OBJS = \
PPGameStart.o \
PPScriptGame.o \
PPBox2D.o \
PPFlMMLObject.o \
PPFlMMLSeq.o \
PPFont.o \
PPFontTable.o \
PPGameGeometry.o \
PPGameMapData.o \
PPGameMapEvent.o \
PPGamePoly.o \
PPGameRunlength.o \
PPGameSound.o \
PPGameSprite.o \
PPGameStick.o \
PPGameTexture.o \
PPImageFont.o \
PPLuaScript.o \
PPNormalFont.o \
PPObject.o \
PPOffscreenTexture.o \
PPParticle.o \
PPReadError.o \
PPSEMMLObject.o \
PPSensor.o \
PPTMXMap.o \
PPTTFont.o \
PPUIScrollView.o \
PPUIText.o \
PPVertualKey.o \
PPWorld.o \
QBGame.o \
QBStreamSound.o \
QBSound.o \
PPGameMapCocos2dx.o \
QBNodeBase64.o \
QBNodeSystemCore.o \
QBNodeSystemPlist.o \
QBNodeSystemUtil.o \
QBNodeSystemValue.o \
glew.o \
PPGameUtilWin.o \
PPTTFont-Win.o \
PPGameWinScene.o \
PPXMLParser.o \
QBSoundWin.o \
regex.o

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)
	rm -f $(LINKFILE)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
