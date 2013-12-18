LOCAL_PATH:= $(call my-dir)

START_PATH := $(LOCAL_PATH)/../../..

include $(CLEAR_VARS)

subdirs := $(START_PATH)/Box2D/Android/jni/Android.mk
subdirs += $(START_PATH)/freetype/Android/jni/Android.mk
subdirs += $(START_PATH)/png/Android/jni/Android.mk
subdirs += $(START_PATH)/CocosDenshion/Android/jni/Android.mk
subdirs += $(START_PATH)/FlMML/Android/jni/Android.mk
subdirs += $(START_PATH)/Cocos2dx/AndroidJIT/jni/Android.mk
subdirs += $(START_PATH)/PicoGame/AndroidJIT/jni/Android.mk
subdirs += $(LOCAL_PATH)/picomain/Android.mk

include $(subdirs)
