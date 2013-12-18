LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libpng
SOURCE_PATH := ../../sources/png

LOCAL_CFLAGS := $(JNI_CFLAGS) \
				-DPNG_NO_MMX_CODE

LOCAL_SRC_FILES := \
    $(SOURCE_PATH)/png.c \
    $(SOURCE_PATH)/pngerror.c \
    $(SOURCE_PATH)/pngget.c \
    $(SOURCE_PATH)/pngmem.c \
    $(SOURCE_PATH)/pngpread.c \
    $(SOURCE_PATH)/pngread.c \
    $(SOURCE_PATH)/pngrio.c \
    $(SOURCE_PATH)/pngrtran.c \
    $(SOURCE_PATH)/pngrutil.c \
    $(SOURCE_PATH)/pngset.c \
    $(SOURCE_PATH)/pngtrans.c \
    $(SOURCE_PATH)/pngwio.c \
    $(SOURCE_PATH)/pngwrite.c \
    $(SOURCE_PATH)/pngwtran.c \
    $(SOURCE_PATH)/pngwutil.c

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/
LOCAL_CFLAGS += -O3

include $(BUILD_STATIC_LIBRARY)
