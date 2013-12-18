LOCAL_PATH:= $(call my-dir)

BASE_PATH:=../../../..
ENGINE_PATH:=$(LOCAL_PATH)/$(BASE_PATH)

ifneq ($(call set_is_member,$(__ndk_modules),jpeg),$(true))
     include $(CLEAR_VARS)
     LOCAL_MODULE    := libjpeg 
     LOCAL_SRC_FILES := $(BASE_PATH)/Cocos2dx/libraries/Android/$(TARGET_ARCH_ABI)/libjpeg.a
     include $(PREBUILT_STATIC_LIBRARY)
endif

ifneq ($(call set_is_member,$(__ndk_modules),png),$(true))
     include $(CLEAR_VARS)
     LOCAL_MODULE    := libpng 
     LOCAL_SRC_FILES := $(BASE_PATH)/png/Android/obj/local/$(TARGET_ARCH_ABI)/libpng.a
     include $(PREBUILT_STATIC_LIBRARY)
endif

ifneq ($(call set_is_member,$(__ndk_modules),xml2),$(true))
     include $(CLEAR_VARS)
     LOCAL_MODULE    := libxml2 
     LOCAL_SRC_FILES := $(BASE_PATH)/Cocos2dx/libraries/Android/$(TARGET_ARCH_ABI)/libxml2.a
     include $(PREBUILT_STATIC_LIBRARY)
endif

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
          -DPNG_NO_MMX_CODE

LOCAL_MODULE    := picogamejni

LOCAL_CFLAGS := $(JNI_CFLAGS)
LOCAL_CFLAGS += -O3 -D__LUAJIT__
LOCAL_CFLAGS += -Werror

LOCAL_SRC_FILES := \
                    ./picogamejni.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    $(ENGINE_PATH)/Cocos2dx/include/libxml2/ \
                    $(ENGINE_PATH)/Cocos2dx/include/libjpeg/ \
                    $(ENGINE_PATH)/Cocos2dx/include/libpng/ \
                    $(ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/ \
                    $(ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/include/ \
                    $(ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/platform/ \
                    $(ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/platform/android/ \
                    $(ENGINE_PATH)/Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2/cocos2dx/platform/android/jni/ \
                    $(ENGINE_PATH)/CocosDenshion/sources/CocosDenshion/include \
                    $(ENGINE_PATH)/CocosDenshion/sources/CocosDenshion \
                    $(ENGINE_PATH)/PicoGame/sources/Platforms/Android/ \
                    $(ENGINE_PATH)/PicoGame/sources/Platforms/Windroid/ \
                    $(ENGINE_PATH)/PicoGame/sources/Logic/ \
                    $(ENGINE_PATH)/PicoGame/sources/PPGame/ \
                    $(ENGINE_PATH)/PicoGame/sources/PPImage/ \
                    $(ENGINE_PATH)/FlMML/sources/FlMML/flmml \
                    $(ENGINE_PATH)/FlMML/sources/FlMML/fmgenAs \
                    $(ENGINE_PATH)/FlMML/sources/FlMML/other \
                    $(ENGINE_PATH)/FlMML/include/flmml \
                    $(ENGINE_PATH)/FlMML/include/other \
                    $(ENGINE_PATH)/FlMML/Android/jni \
                    $(ENGINE_PATH)/LuaJIT/sources/output-android/ \
                    $(ENGINE_PATH)/LuaJIT/sources/output-android/include/ \
                    $(ENGINE_PATH)/Box2D/sources/Box2D_v2.2.1/ \
                    $(ENGINE_PATH)/freetype/sources/ \
                    $(ENGINE_PATH)/freetype/sources/freetype-2.4.12/ \
                    $(ENGINE_PATH)/freetype/sources/freetype-2.4.12/include/

LOCAL_LDLIBS :=     -L$(call host-path, $(LOCAL_PATH)/../libs/$(TARGET_ARCH_ABI)) \
                    -llog -lGLESv1_CM -lz

LOCAL_STATIC_LIBRARIES := libpicogame

include $(BUILD_SHARED_LIBRARY)
