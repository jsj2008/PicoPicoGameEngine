CC = gcc
CPP = g++
AR = ar

CFLAGS = -O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

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
	$(CPP) $(CFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
