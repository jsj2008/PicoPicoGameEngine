LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_MODULE    := picomain
LOCAL_SRC_FILES := ../../../../PicoGame/sources/Platforms/Android/picomain.cpp

LOCAL_C_INCLUDES := $(COCOS2D_PATH)/cocos2dx \
                    $(COCOS2D_PATH)/cocos2dx/platform \
                    $(COCOS2D_PATH)/cocos2dx/platform/third_party/android/libxml2 \
                    $(COCOS2D_PATH)/cocos2dx/platform/third_party/android/libpng \
                    $(COCOS2D_PATH)/cocos2dx/include \
                  	$(COCOS2D_PATH)/CocosDenshion/include \
                    $(ENGINE_PATH)/PicoGame/sources/Platforms/Android/ \
                    $(ENGINE_PATH)/PicoGame/sources/Platforms/Windroid/ \
                    $(ENGINE_PATH)/PicoGame/sources/PPGame/ \
                    $(ENGINE_PATH)/PicoGame/sources/PPImage/ \
                    $(ENGINE_PATH)/FlMML/sources/FlMML/flmml \
                    $(ENGINE_PATH)/FlMML/sources/FlMML/fmgenAs \
                    $(ENGINE_PATH)/FlMML/sources/FlMML/other \
                    $(ENGINE_PATH)/FlMML/include/flmml \
                    $(ENGINE_PATH)/FlMML/include/other \
                    $(ENGINE_PATH)/lua5.2/sources/ \
                    $(ENGINE_PATH)/freetype/sources/ \
                    $(ENGINE_PATH)/freetype/sources/freetype-2.4.10/ \
                    $(ENGINE_PATH)/freetype/sources/freetype-2.4.10/include/ \
                    $(COCOS2DX_PATH)/lua/cocos2dx_support

#          $(ENGINE_PATH)/LuaJIT/sources/output-android/$(TARGET_ARCH_ABI)/include/luajit-2.0/

LOCAL_LDLIBS := -L$(call host-path, $(JNI_PATH)/../libs/$(TARGET_ARCH_ABI)) \
                -lGLESv1_CM \
                -lflmml -lfreetype -llua -lbox2d -lcocos2d -lcocosdenshion -lpicogame

#                $(JNI_PATH)/$(TARGET_ARCH_ABI)/libluajit-5.1.a

include $(BUILD_SHARED_LIBRARY)
