LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libflmml
FLM_SOURCE_PATH := ../../sources/FlMML

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(FLM_SOURCE_PATH)/flmml/MSequencer.cpp \
				$(FLM_SOURCE_PATH)/other/FlMML_Regex.cpp \
				$(FLM_SOURCE_PATH)/other/FlMML_StaticDef.cpp

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/$(FLM_SOURCE_PATH)/flmml \
                    $(LOCAL_PATH)/$(FLM_SOURCE_PATH)/fmgenAs \
                    $(LOCAL_PATH)/$(FLM_SOURCE_PATH)/other \
                    $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3
                           
include $(BUILD_STATIC_LIBRARY)
