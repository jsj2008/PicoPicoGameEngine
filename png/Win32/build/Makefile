CC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-gcc
AR = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-ar

CFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -D_WIN32 -DIGNORE_EXPORT -DGLEW_NO_GLU -D__MINGW32__ -DGLEW_STATIC -DREGEX_STATIC -DLUA_COMPAT_ALL -DCC_UNDER_WIN32 -DGL_GLEXT_PROTOTYPES=1 -DWIN32 -D_WINDOWS -DNDEBUG \
-O3 -DWINDOWS

BUILD_DIR = objects

LIBNAME = libpng.ar

CFLAGS +=  \
-I../../sources/zlib/ \
-I../../sources/png/

VPATH = ../../sources/png/:../../sources/zlib/:

OBJS = \
adler32.o \
compress.o \
crc32.o \
deflate.o \
infback.o \
inffast.o \
inflate.o \
inftrees.o \
trees.o \
uncompr.o \
zutil.o \
png.o \
pngerror.o \
pngget.o \
pngmem.o \
pngpread.o \
pngread.o \
pngrio.o \
pngrtran.o \
pngrutil.o \
pngset.o \
pngtrans.o \
pngwio.o \
pngwrite.o \
pngwtran.o \
pngwutil.o

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
