CC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-gcc
AR = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-ar

CFLAGS = -D_WIN32 -DIGNORE_EXPORT -DWIN32 -D_WINDOWS -DNDEBUG -O3 -DWINDOWS

BUILD_DIR = objects

LIBNAME = libvorbis.ar

CFLAGS += \
-I../../sources/libogg-1.3.1/include \
-I../../sources/libvorbis-1.3.3/include \
-I../../sources/libvorbis-1.3.3/lib

VPATH = ../../sources/libogg-1.3.1/src:../../sources/libvorbis-1.3.3/lib:

OBJS = \
bitwise.o \
framing.o \
mdct.o smallft.o block.o envelope.o window.o lsp.o \
lpc.o analysis.o synthesis.o psy.o info.o \
floor1.o floor0.o \
res0.o mapping0.o registry.o codebook.o sharedbook.o \
lookup.o bitrate.o \
vorbisfile.o

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
