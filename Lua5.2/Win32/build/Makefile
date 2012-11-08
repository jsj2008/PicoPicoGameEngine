CC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-gcc
AR = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-ar

CFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -D_WIN32 -DIGNORE_EXPORT -DGLEW_NO_GLU -D__MINGW32__ -DGLEW_STATIC -DREGEX_STATIC -DLUA_COMPAT_ALL -DCC_UNDER_WIN32 -DGL_GLEXT_PROTOTYPES=1 -DWIN32 -D_WINDOWS -DNDEBUG \
-O3 -DWINDOWS

BUILD_DIR = objects

LIBNAME = liblua.ar

CFLAGS += \
-I../../sources/lua5.2

VPATH = ../../sources/lua5.2:

OBJS = \
lapi.o \
lauxlib.o \
lbaselib.o \
lbitlib.o \
lcode.o \
lcorolib.o \
lctype.o \
ldblib.o \
ldebug.o \
ldo.o \
ldump.o \
lfunc.o \
lgc.o \
linit.o \
liolib.o \
llex.o \
lmathlib.o \
lmem.o \
loadlib.o \
lobject.o \
lopcodes.o \
loslib.o \
lparser.o \
lstate.o \
lstring.o \
lstrlib.o \
ltable.o \
ltablib.o \
ltm.o \
lundump.o \
lvm.o \
lzio.o \
luaApiHook.o

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
