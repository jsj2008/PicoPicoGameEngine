LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-cocos2d
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/libcocos2d.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-cocosdenshion
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/libcocosdenshion.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-freetype
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/libfreetype.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-lua
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/liblua.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-box2d
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/libbox2d.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-flmml
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/libflmml.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-picogame
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/libpicogame.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-picomain
LOCAL_SRC_FILES := /$(TARGET_ARCH_ABI)/libpicomain.so
include $(PREBUILT_SHARED_LIBRARY)
