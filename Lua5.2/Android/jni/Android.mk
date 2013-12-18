LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := liblua
LUA_SOURCE_PATH := ../../sources/lua5.2

JNI_CFLAGS := \
				-DLUA_COMPAT_ALL \
				-D_STLP_NO_EXCEPTIONS \
				-D_STLP_USE_SIMPLE_NODE_ALLOC

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(LUA_SOURCE_PATH)/lapi.c \
				$(LUA_SOURCE_PATH)/lauxlib.c \
				$(LUA_SOURCE_PATH)/lbaselib.c \
				$(LUA_SOURCE_PATH)/lbitlib.c \
				$(LUA_SOURCE_PATH)/lcode.c \
				$(LUA_SOURCE_PATH)/lcorolib.c \
				$(LUA_SOURCE_PATH)/lctype.c \
				$(LUA_SOURCE_PATH)/ldblib.c \
				$(LUA_SOURCE_PATH)/ldebug.c \
				$(LUA_SOURCE_PATH)/ldo.c \
				$(LUA_SOURCE_PATH)/ldump.c \
				$(LUA_SOURCE_PATH)/lfunc.c \
				$(LUA_SOURCE_PATH)/lgc.c \
				$(LUA_SOURCE_PATH)/linit.c \
				$(LUA_SOURCE_PATH)/liolib.c \
				$(LUA_SOURCE_PATH)/llex.c \
				$(LUA_SOURCE_PATH)/lmathlib.c \
				$(LUA_SOURCE_PATH)/lmem.c \
				$(LUA_SOURCE_PATH)/loadlib.c \
				$(LUA_SOURCE_PATH)/lobject.c \
				$(LUA_SOURCE_PATH)/lopcodes.c \
				$(LUA_SOURCE_PATH)/loslib.c \
				$(LUA_SOURCE_PATH)/lparser.c \
				$(LUA_SOURCE_PATH)/lstate.c \
				$(LUA_SOURCE_PATH)/lstring.c \
				$(LUA_SOURCE_PATH)/lstrlib.c \
				$(LUA_SOURCE_PATH)/ltable.c \
				$(LUA_SOURCE_PATH)/ltablib.c \
				$(LUA_SOURCE_PATH)/ltm.c \
				$(LUA_SOURCE_PATH)/lundump.c \
				$(LUA_SOURCE_PATH)/lvm.c \
				$(LUA_SOURCE_PATH)/lzio.c \
				$(LUA_SOURCE_PATH)/luaApiHook.c

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3
                           
include $(BUILD_STATIC_LIBRARY)
