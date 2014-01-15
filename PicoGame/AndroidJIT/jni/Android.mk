LOCAL_PATH := $(call my-dir)

PG_ENGINE_PATH := ../../..

ifneq ($(call set_is_member,$(__ndk_modules),luajit),$(true))
  include $(CLEAR_VARS)
  LOCAL_MODULE    := libluajit
  LOCAL_SRC_FILES := $(PG_ENGINE_PATH)/LuaJIT/Android/LuaJIT/lib/$(TARGET_ARCH_ABI)/libluajit-5.1.a
  include $(PREBUILT_STATIC_LIBRARY)
endif

ifneq ($(call set_is_member,$(__ndk_modules),png),$(true))
  include $(CLEAR_VARS)
  LOCAL_MODULE    := libpng 
  LOCAL_SRC_FILES := $(PG_ENGINE_PATH)/Cocos2dx/libraries/Android/$(TARGET_ARCH_ABI)/libpng.a
  include $(PREBUILT_STATIC_LIBRARY)
endif

ifneq ($(call set_is_member,$(__ndk_modules),xml2),$(true))
  include $(CLEAR_VARS)
  LOCAL_MODULE    := libxml2 
  LOCAL_SRC_FILES := $(PG_ENGINE_PATH)/Cocos2dx/libraries/Android/$(TARGET_ARCH_ABI)/libxml2.a
  include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := picogame
SOURCE_PATH := ../../sources/
COCOS2D_PATH := $(LOCAL_PATH)/../../../Cocos2dx/sources/cocos2d-1.0.1-x-0.9.2

JNI_CFLAGS := \
        -DFT_CONFIG_MODULES_H="<ftmodule.h>" \
        -DFT2_BUILD_LIBRARY \
        -D__COCOS2DX__ \
        -DLUA_COMPAT_ALL \
        -D_ANDROID \
        -D_STLP_NO_EXCEPTIONS \
        -D_STLP_USE_SIMPLE_NODE_ALLOC \
        -DCOCOS2D_DEBUG=0 \
        -DGL_GLEXT_PROTOTYPES=1 \
        -D__USE_OPENGL10__

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(SOURCE_PATH)/Logic/PPGameStart.cpp \
				$(SOURCE_PATH)/Logic/PPScriptGame.cpp \
				$(SOURCE_PATH)/PPGame/PPBox2D.cpp \
				$(SOURCE_PATH)/PPGame/PPFlMMLObject.cpp \
				$(SOURCE_PATH)/PPGame/PPFlMMLSeq.cpp \
				$(SOURCE_PATH)/PPGame/PPFont.cpp \
				$(SOURCE_PATH)/PPGame/PPFontTable.cpp \
				$(SOURCE_PATH)/PPGame/PPGameGeometry.cpp \
				$(SOURCE_PATH)/PPGame/PPGameMapData.cpp \
				$(SOURCE_PATH)/PPGame/PPGameMapEvent.cpp \
				$(SOURCE_PATH)/PPGame/PPGamePoly.cpp \
				$(SOURCE_PATH)/PPGame/PPGameRunlength.c \
				$(SOURCE_PATH)/PPGame/PPGameSound.cpp \
				$(SOURCE_PATH)/PPGame/PPGameSprite.cpp \
				$(SOURCE_PATH)/PPGame/PPGameStick.cpp \
				$(SOURCE_PATH)/PPGame/PPGameTexture.cpp \
				$(SOURCE_PATH)/PPGame/PPImageFont.cpp \
				$(SOURCE_PATH)/PPGame/PPLuaScript.cpp \
				$(SOURCE_PATH)/PPGame/PPNormalFont.cpp \
				$(SOURCE_PATH)/PPGame/PPObject.cpp \
				$(SOURCE_PATH)/PPGame/PPOffscreenTexture.cpp \
				$(SOURCE_PATH)/PPGame/PPParticle.cpp \
				$(SOURCE_PATH)/PPGame/PPReadError.cpp \
				$(SOURCE_PATH)/PPGame/PPSEMMLObject.cpp \
				$(SOURCE_PATH)/PPGame/PPSensor.cpp \
				$(SOURCE_PATH)/PPGame/PPTMXMap.cpp \
				$(SOURCE_PATH)/PPGame/PPTTFont.cpp \
				$(SOURCE_PATH)/PPGame/PPUIScrollView.cpp \
				$(SOURCE_PATH)/PPGame/PPUIText.cpp \
				$(SOURCE_PATH)/PPGame/PPVertualKey.cpp \
				$(SOURCE_PATH)/PPGame/PPWorld.cpp \
				$(SOURCE_PATH)/PPGame/QBGame.cpp \
				$(SOURCE_PATH)/PPGame/QBSound.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPGameUtilCocos2dx.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPGameSoundAndroid.cpp \
				$(SOURCE_PATH)/Platforms/Android/QBSoundAndroid.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPSensorAndroid.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPTTFont-Android.cpp \
				$(SOURCE_PATH)/Platforms/Windroid/PPGameMapCocos2dx.cpp \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeBase64.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemCore.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemPlist.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemUtil.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemValue.c

LOCAL_C_INCLUDES := \
                    $(COCOS2D_PATH)/cocos2dx \
                    $(COCOS2D_PATH)/cocos2dx/platform \
                    $(COCOS2D_PATH)/cocos2dx/platform/third_party/android/libxml2 \
                    $(COCOS2D_PATH)/cocos2dx/platform/third_party/android/libpng \
                    $(COCOS2D_PATH)/cocos2dx/include \
                  	$(COCOS2D_PATH)/CocosDenshion/include \
                  	$(LOCAL_PATH)/../../../CocosDenshion/sources/CocosDenshion/ \
                  	$(LOCAL_PATH)/../../../FlMML/Android/jni/ \
                  	$(LOCAL_PATH)/../../../FlMML/include/flmml/ \
                  	$(LOCAL_PATH)/../../../FlMML/include/other/ \
                  	$(LOCAL_PATH)/../../../FlMML/sources/ \
                    $(LOCAL_PATH)/../../../FlMML/sources/FlMML/fmgenAs/ \
                    $(LOCAL_PATH)/../../../FlMML/sources/FlMML/other/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/Platforms/Android/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/Platforms/Windroid/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/Logic/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/PPGame/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/PPImage/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/GameLogic/ \
                    $(LOCAL_PATH)/../../../LuaJIT/sources/output-android/ \
                    $(LOCAL_PATH)/../../../LuaJIT/sources/output-android/include/ \
                    $(LOCAL_PATH)/../../../Box2D/sources/Box2D_v2.3.0/Box2D/ \
                  	$(LOCAL_PATH)/../../../freetype/sources/ \
                  	$(LOCAL_PATH)/../../../freetype/sources/freetype-2.4.12/ \
                  	$(LOCAL_PATH)/../../../freetype/sources/freetype-2.4.12/include/ \
                  	$(LOCAL_PATH)/.

LOCAL_CFLAGS += -O3 -D__LUAJIT__

LOCAL_STATIC_LIBRARIES := libluajit libbox2d libflmml libcocos2dwrapper libfreetype
LOCAL_SHARED_LIBRARIES := cocosdenshion

include $(BUILD_STATIC_LIBRARY)
