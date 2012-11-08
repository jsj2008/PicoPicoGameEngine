LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := lua
SOURCE_PATH := ../../sources/lua5.2

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(SOURCE_PATH)/lapi.c \
				$(SOURCE_PATH)/lauxlib.c \
				$(SOURCE_PATH)/lbaselib.c \
				$(SOURCE_PATH)/lbitlib.c \
				$(SOURCE_PATH)/lcode.c \
				$(SOURCE_PATH)/lcorolib.c \
				$(SOURCE_PATH)/lctype.c \
				$(SOURCE_PATH)/ldblib.c \
				$(SOURCE_PATH)/ldebug.c \
				$(SOURCE_PATH)/ldo.c \
				$(SOURCE_PATH)/ldump.c \
				$(SOURCE_PATH)/lfunc.c \
				$(SOURCE_PATH)/lgc.c \
				$(SOURCE_PATH)/linit.c \
				$(SOURCE_PATH)/liolib.c \
				$(SOURCE_PATH)/llex.c \
				$(SOURCE_PATH)/lmathlib.c \
				$(SOURCE_PATH)/lmem.c \
				$(SOURCE_PATH)/loadlib.c \
				$(SOURCE_PATH)/lobject.c \
				$(SOURCE_PATH)/lopcodes.c \
				$(SOURCE_PATH)/loslib.c \
				$(SOURCE_PATH)/lparser.c \
				$(SOURCE_PATH)/lstate.c \
				$(SOURCE_PATH)/lstring.c \
				$(SOURCE_PATH)/lstrlib.c \
				$(SOURCE_PATH)/ltable.c \
				$(SOURCE_PATH)/ltablib.c \
				$(SOURCE_PATH)/ltm.c \
				$(SOURCE_PATH)/lundump.c \
				$(SOURCE_PATH)/lvm.c \
				$(SOURCE_PATH)/lzio.c \
				$(SOURCE_PATH)/luaApiHook.c

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3

# it is used for ndk-r5  
# if you build with ndk-r4, comment it  
# because the new Windows toolchain doesn't support Cygwin's drive
# mapping (i.e /cygdrive/c/ instead of C:/)  
                           
include $(BUILD_SHARED_LIBRARY)
