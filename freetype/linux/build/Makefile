CC = gcc
CPP = g++
AR = ar

CFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

BUILD_DIR = objects

LIBNAME = libfreetype.ar

CFLAGS += \
-I../../sources/freetype-2.4.10/include/ \
-I../../sources/freetype-2.4.10/include/freetype/config/ \
-I../../sources/freetype-2.4.10/src/autofit/ \
-I../../sources/freetype-2.4.10/src/base/ \
-I../../sources/freetype-2.4.10/src/bdf/ \
-I../../sources/freetype-2.4.10/src/bzip2/ \
-I../../sources/freetype-2.4.10/src/cache/ \
-I../../sources/freetype-2.4.10/src/cff/ \
-I../../sources/freetype-2.4.10/src/cid/ \
-I../../sources/freetype-2.4.10/src/gxvalid/ \
-I../../sources/freetype-2.4.10/src/gzip/ \
-I../../sources/freetype-2.4.10/src/lzw/ \
-I../../sources/freetype-2.4.10/src/otvalid/ \
-I../../sources/freetype-2.4.10/src/pcf/ \
-I../../sources/freetype-2.4.10/src/pfr/ \
-I../../sources/freetype-2.4.10/src/psaux/ \
-I../../sources/freetype-2.4.10/src/pshinter/ \
-I../../sources/freetype-2.4.10/src/psnames/ \
-I../../sources/freetype-2.4.10/src/raster/ \
-I../../sources/freetype-2.4.10/src/sfnt/ \
-I../../sources/freetype-2.4.10/src/smooth/ \
-I../../sources/freetype-2.4.10/src/tools/ \
-I../../sources/freetype-2.4.10/src/truetype/ \
-I../../sources/freetype-2.4.10/src/type1/ \
-I../../sources/freetype-2.4.10/src/type42/ \
-I../../sources/freetype-2.4.10/src/winfonts/

VPATH = \
../../sources/freetype-2.4.10/src/autofit/: \
../../sources/freetype-2.4.10/src/base/: \
../../sources/freetype-2.4.10/src/bdf/: \
../../sources/freetype-2.4.10/src/bzip2/: \
../../sources/freetype-2.4.10/src/cache/: \
../../sources/freetype-2.4.10/src/cff/: \
../../sources/freetype-2.4.10/src/cid/: \
../../sources/freetype-2.4.10/src/gxvalid/: \
../../sources/freetype-2.4.10/src/gzip/: \
../../sources/freetype-2.4.10/src/lzw/: \
../../sources/freetype-2.4.10/src/otvalid/: \
../../sources/freetype-2.4.10/src/pcf/: \
../../sources/freetype-2.4.10/src/pfr/: \
../../sources/freetype-2.4.10/src/psaux/: \
../../sources/freetype-2.4.10/src/pshinter/: \
../../sources/freetype-2.4.10/src/psnames/: \
../../sources/freetype-2.4.10/src/raster/: \
../../sources/freetype-2.4.10/src/sfnt/: \
../../sources/freetype-2.4.10/src/smooth/: \
../../sources/freetype-2.4.10/src/tools/: \
../../sources/freetype-2.4.10/src/truetype/: \
../../sources/freetype-2.4.10/src/type1/: \
../../sources/freetype-2.4.10/src/type42/: \
../../sources/freetype-2.4.10/src/winfonts/:

OBJS = \
autofit.o \
bdf.o \
cff.o \
ftbase.o \
ftglyph.o \
ftbitmap.o \
ftbzip2.o \
ftcache.o \
ftgzip.o \
ftinit.o \
ftlzw.o \
ftsystem.o \
gxvalid.o \
otvalid.o \
pcf.o \
pfr.o \
psaux.o \
pshinter.o \
psnames.o \
raster.o \
sfnt.o \
smooth.o \
truetype.o \
ttsbit0.o \
type1.o \
type1cid.o \
type42.o \
winfnt.o

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
