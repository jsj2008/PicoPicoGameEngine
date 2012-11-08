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

LOCAL_LDLIBS := -L$(call host-path, $(JNI_PATH)/../libs/armeabi) \
                -lGLESv1_CM -lcocos2d -llog -lfreetype -llua -lbox2d -lflmml -lpicogame -lcocosdenshion

include $(BUILD_SHARED_LIBRARY)
