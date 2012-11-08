CC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-gcc
AR = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-ar

CFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -D_WIN32 -DIGNORE_EXPORT -DGLEW_NO_GLU -D__MINGW32__ -DGLEW_STATIC -DREGEX_STATIC -DLUA_COMPAT_ALL -DCC_UNDER_WIN32 -DGL_GLEXT_PROTOTYPES=1 -DWIN32 -D_WINDOWS -DNDEBUG \
-O3 -DWINDOWS

BUILD_DIR = objects

LIBNAME = libflmml.ar

CFLAGS += \
-I../../../PicoGame/sources/Platforms/Windows/ \
-I../../sources/FlMML/flmml/ \
-I../../sources/FlMML/fmgenAs/ \
-I../../sources/FlMML/other/

VPATH = ../../sources/FlMML/flmml/:../../sources/FlMML/fmgenAs/:../../sources/FlMML/other/:../../../PicoGame/sources/Platforms/Windroid/:

OBJS = \
MSequencer.o \
FlMML_Regex.o \
FlMML_StaticDef.o

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)
	rm -f $(LINKFILE)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
