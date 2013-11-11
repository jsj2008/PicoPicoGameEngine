CC = gcc
CPP = g++
AR = ar

CFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

BUILD_DIR = objects

FREETYPE_VER = freetype-2.4.12

LIBNAME = libfreetype.ar

CFLAGS += \
-I../../sources/$(FREETYPE_VER)/include/ \
-I../../sources/$(FREETYPE_VER)/include/freetype/config/ \
-I../../sources/$(FREETYPE_VER)/src/autofit/ \
-I../../sources/$(FREETYPE_VER)/src/base/ \
-I../../sources/$(FREETYPE_VER)/src/bdf/ \
-I../../sources/$(FREETYPE_VER)/src/bzip2/ \
-I../../sources/$(FREETYPE_VER)/src/cache/ \
-I../../sources/$(FREETYPE_VER)/src/cff/ \
-I../../sources/$(FREETYPE_VER)/src/cid/ \
-I../../sources/$(FREETYPE_VER)/src/gxvalid/ \
-I../../sources/$(FREETYPE_VER)/src/gzip/ \
-I../../sources/$(FREETYPE_VER)/src/lzw/ \
-I../../sources/$(FREETYPE_VER)/src/otvalid/ \
-I../../sources/$(FREETYPE_VER)/src/pcf/ \
-I../../sources/$(FREETYPE_VER)/src/pfr/ \
-I../../sources/$(FREETYPE_VER)/src/psaux/ \
-I../../sources/$(FREETYPE_VER)/src/pshinter/ \
-I../../sources/$(FREETYPE_VER)/src/psnames/ \
-I../../sources/$(FREETYPE_VER)/src/raster/ \
-I../../sources/$(FREETYPE_VER)/src/sfnt/ \
-I../../sources/$(FREETYPE_VER)/src/smooth/ \
-I../../sources/$(FREETYPE_VER)/src/tools/ \
-I../../sources/$(FREETYPE_VER)/src/truetype/ \
-I../../sources/$(FREETYPE_VER)/src/type1/ \
-I../../sources/$(FREETYPE_VER)/src/type42/ \
-I../../sources/$(FREETYPE_VER)/src/winfonts/

VPATH = \
../../sources/$(FREETYPE_VER)/src/autofit/: \
../../sources/$(FREETYPE_VER)/src/base/: \
../../sources/$(FREETYPE_VER)/src/bdf/: \
../../sources/$(FREETYPE_VER)/src/bzip2/: \
../../sources/$(FREETYPE_VER)/src/cache/: \
../../sources/$(FREETYPE_VER)/src/cff/: \
../../sources/$(FREETYPE_VER)/src/cid/: \
../../sources/$(FREETYPE_VER)/src/gxvalid/: \
../../sources/$(FREETYPE_VER)/src/gzip/: \
../../sources/$(FREETYPE_VER)/src/lzw/: \
../../sources/$(FREETYPE_VER)/src/otvalid/: \
../../sources/$(FREETYPE_VER)/src/pcf/: \
../../sources/$(FREETYPE_VER)/src/pfr/: \
../../sources/$(FREETYPE_VER)/src/psaux/: \
../../sources/$(FREETYPE_VER)/src/pshinter/: \
../../sources/$(FREETYPE_VER)/src/psnames/: \
../../sources/$(FREETYPE_VER)/src/raster/: \
../../sources/$(FREETYPE_VER)/src/sfnt/: \
../../sources/$(FREETYPE_VER)/src/smooth/: \
../../sources/$(FREETYPE_VER)/src/tools/: \
../../sources/$(FREETYPE_VER)/src/truetype/: \
../../sources/$(FREETYPE_VER)/src/type1/: \
../../sources/$(FREETYPE_VER)/src/type42/: \
../../sources/$(FREETYPE_VER)/src/winfonts/:

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
