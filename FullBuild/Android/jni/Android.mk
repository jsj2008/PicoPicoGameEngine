LOCAL_PATH := $(call my-dir)
JNI_PATH := $(LOCAL_PATH)
JNI_CFLAGS := -DANDROID_NDK_NONE \
				-DFT_CONFIG_MODULES_H="<ftmodule.h>" \
				-DFT2_BUILD_LIBRARY \
				-D__COCOS2DX__ \
				-DLOCAL_LOG \
				-DLUA_COMPAT_ALL \
				-D_ANDROID \
				-DBOOST_EXCEPTION_DISABLE \
				-D_STLP_NO_EXCEPTIONS -DOS_ANDROID \
				-D_STLP_USE_SIMPLE_NODE_ALLOC \
				-DUSE_FILE32API \
				-DCOCOS2D_DEBUG=0 \
				-DGL_GLEXT_PROTOTYPES=1

ENGINE_PATH := $(LOCAL_PATH)/../../../
COCOS2D_PATH := $(LOCAL_PATH)/../../../cocos2d-1.0.1-x-0.9.2

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-cocos2d
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libcocos2d.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := prebuild-cocosdenshion
LOCAL_SRC_FILES := /lib/$(TARGET_ARCH_ABI)/libcocosdenshion.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

subdirs := $(ENGINE_PATH)/Box2D/Android/jni/Android.mk
subdirs += $(ENGINE_PATH)/PicoGame/Android/jni/Android.mk
subdirs += $(ENGINE_PATH)/FlMML/Android/jni/Android.mk
subdirs += $(ENGINE_PATH)/freetype/Android/jni/Android.mk
subdirs += $(ENGINE_PATH)/Lua5.2/Android/jni/Android.mk
subdirs += $(LOCAL_PATH)/picomain/Android.mk

include $(subdirs)
