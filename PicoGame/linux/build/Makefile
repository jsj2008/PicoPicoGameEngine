CC = gcc
CPP = g++
AR = ar

CPPFLAGS = -D__LUAJIT__ -D_LINUX -O3 -DNO_COCOSDENSHION -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

BUILD_DIR = objects

LIBNAME = libpicogame.ar

CPPFLAGS += \
-I/opt/vc/include \
-I/opt/vc/include/interface/vcos/pthreads \
-I/opt/vc/include/interface/vmcs_host/l\
-I/usr/include/libxml2/\
-I/usr/local/include/luajit-2.0/\
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
-I../../../Box2D/sources/Box2D_v2.2.1/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/Collision/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/Collision/Shapes/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/Common/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/Dynamics/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/Dynamics/Contacts/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/Dynamics/Joints/ \
-I../../../Box2D/sources/Box2D_v2.2.1/Box2D/Rope/ \
-I../../../freetype/sources/ \
-I../../../freetype/sources/freetype-2.4.12/ \
-I../../../freetype/sources/freetype-2.4.12/include/ \
-I../../../png/sources/png/

CFLAGS = $(CPPFLAGS) -std=gnu99

VPATH = \
../../sources/Platforms/linux/: \
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
PPGameButton.o \
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
PPMap.o \
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
QBSound.o \
QBNodeBase64.o \
QBNodeSystemCore.o \
QBNodeSystemPlist.o \
QBNodeSystemUtil.o \
QBNodeSystemValue.o \
PPGameUtilLinux.o \
PPGameLinuxScene.o \
PPGameMapCocos2dx.o \
PPLinuxKey.o

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
