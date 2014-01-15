LOCAL_PATH := $(call my-dir)
PM_INCLUDE_PATH := ../..
PM_ENGINE_PATH := ../../../..

ROOT_PATH:=../../../..
ENGINE_PATH:=$(LOCAL_PATH)/$(ROOT_PATH)

ifneq ($(call set_is_member,$(__ndk_modules),cocos2d),$(true))
     include $(CLEAR_VARS)
     LOCAL_MODULE    := cocos2d 
     LOCAL_SRC_FILES := $(ROOT_PATH)/Cocos2dx/libraries/Android/$(TARGET_ARCH_ABI)/libcocos2d.so
     include $(PREBUILT_SHARED_LIBRARY)
endif

include $(CLEAR_VARS)

JNI_CFLAGS := \
                    -DFT_CONFIG_MODULES_H="<ftmodule.h>" \
                    -DFT2_BUILD_LIBRARY \
                    -D__COCOS2DX__ \
                    -DLUA_COMPAT_ALL \
                    -D_ANDROID \
                    -D_STLP_NO_EXCEPTIONS \
                    -D_STLP_USE_SIMPLE_NODE_ALLOC \
                    -DCOCOS2D_DEBUG=0 \
                    -DGL_GLEXT_PROTOTYPES=1

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_MODULE    := picogamejni

SOURCE_PATH := ../../../../PicoGame/sources

LOCAL_SRC_FILES := \
                    $(SOURCE_PATH)/Platforms/Android/picomain.cpp

LOCAL_C_INCLUDES := $(COCOS2D_PATH)/cocos2dx \
                    $(COCOS2D_PATH)/cocos2dx/platform \
                    $(COCOS2D_PATH)/cocos2dx/platform/third_party/android/libxml2 \
                    $(COCOS2D_PATH)/cocos2dx/platform/third_party/android/libpng \
                    $(COCOS2D_PATH)/cocos2dx/include \
                    $(PM_INCLUDE_PATH)/PicoGame/sources/Platforms/Android/ \
                    $(PM_INCLUDE_PATH)/PicoGame/sources/Platforms/Windroid/ \
                    $(PM_INCLUDE_PATH)/PicoGame/sources/PPGame/ \
                    $(PM_INCLUDE_PATH)/PicoGame/sources/PPImage/ \
                    $(PM_INCLUDE_PATH)/FlMML/sources/FlMML/flmml \
                    $(PM_INCLUDE_PATH)/FlMML/sources/FlMML/fmgenAs \
                    $(PM_INCLUDE_PATH)/FlMML/sources/FlMML/other \
                    $(PM_INCLUDE_PATH)/FlMML/include/flmml \
                    $(PM_INCLUDE_PATH)/FlMML/include/other \
                    $(PM_INCLUDE_PATH)/Lua5.2/sources/ \
                    $(LOCAL_PATH)/../../../freetype/sources/ \
                    $(LOCAL_PATH)/../../../freetype/sources/freetype-2.4.12/ \
                    $(LOCAL_PATH)/../../../freetype/sources/freetype-2.4.12/include/

LOCAL_LDLIBS := -L$(call host-path, $(JNI_PATH)/../libs/$(TARGET_ARCH_ABI)) \
                -lGLESv1_CM -llog -lz

LOCAL_STATIC_LIBRARIES := libpicogame
LOCAL_SHARED_LIBRARIES := cocos2d

include $(BUILD_SHARED_LIBRARY)
