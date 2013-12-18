LOCAL_PATH:= $(call my-dir)
CW_ENGINE_PATH := ../..

include $(CLEAR_VARS)

JNI_CFLAGS := \
          -DFT_CONFIG_MODULES_H="<ftmodule.h>" \
          -DFT2_BUILD_LIBRARY \
          -D__COCOS2DX__ \
          -DLUA_COMPAT_ALL \
          -DUSE_FILE32API \
          -D_ANDROID \
          -D_STLP_NO_EXCEPTIONS \
          -D_STLP_USE_SIMPLE_NODE_ALLOC \
          -DGL_GLEXT_PROTOTYPES=1 \
          -DCOCOS2D_DEBUG=0 \
          -DPNG_NO_MMX_CODE \
          -DCC_RETINA_DISPLAY_SUPPORT=0

LOCAL_MODULE    := libcocos2dwrapper

LOCAL_CFLAGS := $(JNI_CFLAGS)
LOCAL_CFLAGS += -O3
#LOCAL_CFLAGS += -Werror

ENGINE_PATH := $(LOCAL_PATH)/../..
SOURCE_PATH := ../../PicoGame/sources
COCOS2DX_PATH := ../../sources/cocos2d-1.0.1-x-0.9.2/cocos2dx

LOCAL_SRC_FILES := \
                    $(COCOS2DX_PATH)/cocoa/CCObject.cpp \
                    $(COCOS2DX_PATH)/cocoa/CCAutoreleasePool.cpp \
                    $(COCOS2DX_PATH)/cocoa/CCGeometry.cpp \
                    $(COCOS2DX_PATH)/platform/CCCommon.cpp \
                    $(COCOS2DX_PATH)/platform/CCFileUtils.cpp \
                    $(COCOS2DX_PATH)/platform/CCSAXParser.cpp \
                    $(COCOS2DX_PATH)/support/CCUserDefault.cpp \
                    $(COCOS2DX_PATH)/platform/android/jni/SystemInfoJni.cpp \
                    $(COCOS2DX_PATH)/platform/android/jni/JniHelper.cpp \
                    $(COCOS2DX_PATH)/platform/CCImage.cpp \
                    $(COCOS2DX_PATH)/support/zip_support/ioapi.cpp \
                    $(COCOS2DX_PATH)/support/zip_support/unzip.cpp \
                    $(COCOS2DX_PATH)/support/zip_support/ZipUtils.cpp \
                    MessageJni.cpp \
                    Cocos2dxWrapper.cpp \
                    Cocos2dxLuaLoader.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    $(LOCAL_PATH)/$(COCOS2DX_PATH)/include/ \
                    $(LOCAL_PATH)/../../include/libxml2/ \
                    $(LOCAL_PATH)/../../include/libjpeg/ \
                    $(LOCAL_PATH)/../../include/libpng/ \
                    $(LOCAL_PATH)/$(COCOS2DX_PATH)/ \
                    $(LOCAL_PATH)/$(COCOS2DX_PATH)/platform/ \
                    $(LOCAL_PATH)/$(COCOS2DX_PATH)/platform/android/ \
                    $(LOCAL_PATH)/$(COCOS2DX_PATH)/platform/android/jni/ \
                    $(CW_ENGINE_PATH)/Lua5.2/sources/ \
                  	$(CW_ENGINE_PATH)/Lua5.2/sources/lua/

LOCAL_STATIC_LIBRARIES := libpng libxml2 libjpeg

include $(BUILD_STATIC_LIBRARY)
