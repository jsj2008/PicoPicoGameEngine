LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-cocos2d
LOCAL_SRC_FILES := /armeabi/libcocos2d.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-cocosdenshion
LOCAL_SRC_FILES := /armeabi/libcocosdenshion.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-freetype
LOCAL_SRC_FILES := /armeabi/libfreetype.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-lua
LOCAL_SRC_FILES := /armeabi/liblua.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-box2d
LOCAL_SRC_FILES := /armeabi/libbox2d.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-flmml
LOCAL_SRC_FILES := /armeabi/libflmml.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-picogame
LOCAL_SRC_FILES := /armeabi/libpicogame.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-picomain
LOCAL_SRC_FILES := /armeabi/libpicomain.so
include $(PREBUILT_SHARED_LIBRARY)
