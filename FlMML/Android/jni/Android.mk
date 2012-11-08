LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := flmml
SOURCE_PATH := ../../sources/FlMML
INCLUDE_PATH := ../../Win32/include

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(SOURCE_PATH)/flmml/MSequencer.cpp \
				$(SOURCE_PATH)/other/FlMML_Regex.cpp \
				$(SOURCE_PATH)/other/FlMML_StaticDef.cpp \
				./regex.c

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/flmml \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/fmgenAs \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/other \
                    $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3

# it is used for ndk-r5  
# if you build with ndk-r4, comment it  
# because the new Windows toolchain doesn't support Cygwin's drive
# mapping (i.e /cygdrive/c/ instead of C:/)  
                           
include $(BUILD_SHARED_LIBRARY)
