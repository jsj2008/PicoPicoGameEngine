LOCAL_PATH := $(call my-dir)
CW_ENGINE_PATH := ../..

include $(CLEAR_VARS)
LOCAL_MODULE    := libcocos2dwrapper

CW_SOURCE_PATH := ../../../PicoGame/sources

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
                    $(CW_SOURCE_PATH)/Platforms/Android/Cocos2dxWrapper.cpp \
                    $(CW_SOURCE_PATH)/Platforms/Android/Cocos2dxLuaLoader.cpp

LOCAL_C_INCLUDES := $(CW_ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx \
                    $(CW_ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/platform \
                    $(CW_ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/platform/third_party/android/libxml2 \
                    $(CW_ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/platform/third_party/android/libpng \
                    $(CW_ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/include \
                    $(CW_ENGINE_PATH)/CocosDenshion/include \
                    $(CW_ENGINE_PATH)/PicoGame/sources/Platforms/Android/ \
                    $(CW_ENGINE_PATH)/PicoGame/sources/Platforms/Windroid/ \
                    $(CW_ENGINE_PATH)/PicoGame/sources/PPGame/ \
                    $(CW_ENGINE_PATH)/PicoGame/sources/PPImage/ \
                    $(CW_ENGINE_PATH)/FlMML/sources/FlMML/flmml \
                    $(CW_ENGINE_PATH)/FlMML/sources/FlMML/fmgenAs \
                    $(CW_ENGINE_PATH)/FlMML/sources/FlMML/other \
                    $(CW_ENGINE_PATH)/FlMML/include/flmml \
                    $(CW_ENGINE_PATH)/FlMML/include/other \
                    $(CW_ENGINE_PATH)/Lua5.2/sources/lua5.2/ \
                    $(CW_ENGINE_PATH)/freetype/sources/ \
                    $(CW_ENGINE_PATH)/freetype/sources/freetype-2.4.10/ \
                    $(CW_ENGINE_PATH)/freetype/sources/freetype-2.4.10/include/

LOCAL_CFLAGS += -O3

include $(BUILD_STATIC_LIBRARY)
