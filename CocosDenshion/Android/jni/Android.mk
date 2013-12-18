LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cocosdenshion
CD_SOURCE_PATH := ../../sources/CocosDenshion/android
CD_INCLUDE_PATH := ../../sources/CocosDenshion/include

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(CD_SOURCE_PATH)/SimpleAudioEngine.cpp \
				$(CD_SOURCE_PATH)/jni/SimpleAudioEngineJni.cpp

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/$(CD_SOURCE_PATH)/ \
                    $(LOCAL_PATH)/$(CD_SOURCE_PATH)/jni/ \
                    $(LOCAL_PATH)/$(CD_INCLUDE_PATH)/ \
                    $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
