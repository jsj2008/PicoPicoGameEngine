CC = gcc
CPP = g++
AR = ar

LINKFILE = ./objects/LinkFileList.txt

BUILD_DIR = objects

CFLAGS = -D__LUAJIT__ -D_LINUX -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" \
-O3  -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer \
-I/usr/local/include/luajit-2.0/\
-I../../../CocosDenshion/sources/CocosDenshion/include/ \
-I../../../CocosDenshion/sources/CocosDenshion/ \
-I../../../FlMML/include/flmml/ \
-I../../../FlMML/include/fmgenAs/ \
-I../../../FlMML/include/other/ \
-I../../../FlMML/sources/FlMML/flmml/ \
-I../../../FlMML/sources/FlMML/other/ \
-I../../../FlMML/sources/FlMML/fmgenAs/ \
-I../../../PicoGame/sources/Platforms/linux/ \
-I../../../PicoGame/sources/Platforms/Windows/ \
-I../../../PicoGame/sources/Platforms/Windroid/ \
-I../../../PicoGame/sources/Logic/ \
-I../../../PicoGame/sources/PPGame/ \
-I../../../PicoGame/sources/PPImage/ \
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
-I../../../freetype/sources/freetype-2.4.10/ \
-I../../../freetype/sources/freetype-2.4.10/include/ \
-I../../../png/sources/png/

CFLAGS+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

CFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi -fpermissive

VPATH = \
../../../PicoGame/sources/Platforms/linux/:

OBJS = \
QBSoundLinux.o \
pcm.o \
main.o

LIBS = \
../../../PicoGame/linux/build/libpicogame.ar \
../../../freetype/linux/build/libfreetype.ar \
../../../FlMML/linux/build/libflmml.ar \
../../../png/linux/build/libpng.ar \
../../../Box2D/linux/build/libbox2d.ar

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lluajit-5.1 -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lxml2 -lasound

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

picogame.bin: setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(CPP) -o $@ $(LDFLAGS) $(BUILD_DIR)/main.o $(BUILD_DIR)/QBSoundLinux.o $(BUILD_DIR)/pcm.o $(LIBS)

LinkFileList.txt:
	ls $(BUILD_DIR)/*.o >> $(LINKFILE)

setup:
	mkdir -p $(BUILD_DIR)
	rm -f $(LINKFILE)

buildall:
	cd ../../../Box2D/linux/build; make clean
	cd ../../../CocosDenshion/linux/build; make clean
	cd ../../../FlMML/linux/build; make clean
	cd ../../../freetype/linux/build; make clean
	cd ../../../PicoGame/linux/build; make clean
	cd ../../../png/linux/build; make clean
	cd ./; make clean
	cd ../../../Box2D/linux/build; make
	cd ../../../CocosDenshion/linux/build; make
	cd ../../../FlMML/linux/build; make
	cd ../../../freetype/linux/build; make
	cd ../../../PicoGame/linux/build; make
	cd ../../../png/linux/build; make
	cd ./; make

cleanall:
	cd ../../../Box2D/linux/build; make clean
	cd ../../../CocosDenshion/linux/build; make clean
	cd ../../../FlMML/linux/build; make clean
	cd ../../../freetype/linux/build; make clean
	cd ../../../PicoGame/linux/build; make clean
	cd ../../../png/linux/build; make clean
	cd ./; make clean

build:
	cd ../../../Box2D/linux/build; make
	cd ../../../CocosDenshion/linux/build; make
	cd ../../../FlMML/linux/build; make
	cd ../../../freetype/linux/build; make
	cd ../../../PicoGame/linux/build; make
	cd ../../../png/linux/build; make
	cd ./; make

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
