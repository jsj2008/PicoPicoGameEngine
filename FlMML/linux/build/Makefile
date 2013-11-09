CC = gcc
CPP = g++
AR = ar

CFLAGS = -O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

BUILD_DIR = objects

LIBNAME = libflmml.ar

CFLAGS += \
-I../../../PicoGame/sources/Platforms/linux/ \
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
	$(CPP) $(CFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
