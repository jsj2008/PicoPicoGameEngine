CC = gcc
CPP = g++
AR = ar

CFLAGS = -O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

BUILD_DIR = objects

LIBNAME = libpng.ar

CFLAGS +=  \
-I../../sources/png/ \
-I../../sources/zlib/

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
pngwutil.o \
trees.o \
uncompr.o \
zutil.o

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
