CC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-gcc
AR = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-ar
RC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-windres

LINKFILE = ./objects/LinkFileList.txt

BUILD_DIR = objects

CFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -D_WIN32 -DIGNORE_EXPORT -DGLEW_NO_GLU -D__MINGW32__ -DGLEW_STATIC -DREGEX_STATIC -DLUA_COMPAT_ALL -DCC_UNDER_WIN32 -DGL_GLEXT_PROTOTYPES=1 -DWIN32 -D_WINDOWS -DNDEBUG -DWINDOWS\
-O1 \
-I../../../CocosDenshion/sources/CocosDenshion/include/ \
-I../../../CocosDenshion/sources/CocosDenshion/ \
-I../../../FlMML/include/flmml/ \
-I../../../FlMML/include/fmgenAs/ \
-I../../../FlMML/include/other/ \
-I../../../FlMML/sources/FlMML/flmml/ \
-I../../../FlMML/sources/FlMML/other/ \
-I../../../FlMML/sources/FlMML/fmgenAs/ \
-I../../../PicoGame/sources/Platforms/Windows/ \
-I../../../PicoGame/sources/Platforms/Windroid/ \
-I../../../PicoGame/sources/Logic/ \
-I../../../PicoGame/sources/PPGame/ \
-I../../../PicoGame/sources/PPImage/ \
-I../../../Lua5.2/sources/ \
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

VPATH = \
../../../PicoGame/sources/Platforms/Windows/:

OBJS = \
main.o

LIBS = \
../../../PicoGame/Win32/build/libpicogame.ar \
../../../CocosDenshion/Win32/build/libcocosdenshion.ar \
../../../Lua5.2/Win32/build/liblua.ar \
../../../freetype/Win32/build/libfreetype.ar \
../../../FlMML/Win32/build/libflmml.ar \
../../../png/Win32/build/libpng.ar \
../../../Box2D/Win32/build/libbox2d.ar

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

picogame.exe: setup $(addprefix $(BUILD_DIR)/,$(OBJS)) $(BUILD_DIR)/PicoGame.o LinkFileList.txt
	$(CC) -Wl,--enable-auto-import -mwindows -mconsole -o $@ -filelist $(LINKFILE) $(LIBS) -lopengl32 -lwinmm -lshlwapi -lstdc++

$(BUILD_DIR)/PicoGame.o: PicoGame.rc
	$(RC) $< $@

LinkFileList.txt:
	ls $(BUILD_DIR)/*.o >> $(LINKFILE)

setup:
	mkdir -p $(BUILD_DIR)
	rm -f $(LINKFILE)

fullbuild:
	cd ../../../Box2D/Win32/build; make clean
	cd ../../../CocosDenshion/Win32/build; make clean
	cd ../../../FlMML/Win32/build; make clean
	cd ../../../freetype/Win32/build; make clean
	cd ../../../lua5.2/Win32/build; make clean
	cd ../../../PicoGame/Win32/build; make clean
	cd ../../../png/Win32/build; make clean
	cd ./; make clean
	cd ../../../Box2D/Win32/build; make
	cd ../../../CocosDenshion/Win32/build; make
	cd ../../../FlMML/Win32/build; make
	cd ../../../freetype/Win32/build; make
	cd ../../../lua5.2/Win32/build; make
	cd ../../../PicoGame/Win32/build; make
	cd ../../../png/Win32/build; make
	cd ./; make

cleanall:
	cd ../../../Box2D/Win32/build; make clean
	cd ../../../CocosDenshion/Win32/build; make clean
	cd ../../../FlMML/Win32/build; make clean
	cd ../../../freetype/Win32/build; make clean
	cd ../../../lua5.2/Win32/build; make clean
	cd ../../../PicoGame/Win32/build; make clean
	cd ../../../png/Win32/build; make clean
	cd ./; make clean

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
