CC = gcc
CPP = g++
AR = ar

CFLAGS = -O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

BUILD_DIR = objects

LIBNAME = libcocosdenshion.ar

CFLAGS += \
-I../../sources/CocosDenshion/include/ \
-I../../sources/CocosDenshion/third_party/fmod/api/inc/

VPATH = ../../sources/CocosDenshion/Linux/:

OBJS = \
FmodAudioPlayer.o \
SimpleAudioEngine.o

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/$@

$(BUILD_DIR)/%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
