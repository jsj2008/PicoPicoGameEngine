LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := picogame
SOURCE_PATH := ../../sources/
#COCOS2DX_PATH := $(LOCAL_PATH)/../../../cocos2d-1.0.1-x-0.9.2/

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(SOURCE_PATH)/Logic/PPGameStart.cpp \
				$(SOURCE_PATH)/Logic/PPScriptGame.cpp \
				$(SOURCE_PATH)/PPGame/PPBox2D.cpp \
				$(SOURCE_PATH)/PPGame/PPFlMMLObject.cpp \
				$(SOURCE_PATH)/PPGame/PPFlMMLSeq.cpp \
				$(SOURCE_PATH)/PPGame/PPFont.cpp \
				$(SOURCE_PATH)/PPGame/PPFontTable.cpp \
				$(SOURCE_PATH)/PPGame/PPGameButton.cpp \
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
				$(SOURCE_PATH)/PPGame/PPMap.cpp \
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
				$(SOURCE_PATH)/Platforms/Android/AppDelegate.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPBaseScene.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPGameScene.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPGameUtilCocos2dx.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPGameSoundAndroid.cpp \
				$(SOURCE_PATH)/Platforms/Android/QBSoundAndroid.cpp \
				$(SOURCE_PATH)/Platforms/Android/PPSensorAndroid.cpp \
				$(SOURCE_PATH)/Platforms/Windroid/PPGameMapCocos2dx.cpp \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeBase64.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemCore.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemPlist.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemUtil.c \
				$(SOURCE_PATH)/Platforms/Windroid/QBNodeSystemValue.c \
                ./Cocos2dxLuaLoader.cpp

LOCAL_C_INCLUDES := $(COCOS2D_PATH)/cocos2dx \
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
                  	$(LOCAL_PATH)/../../../Lua5.2/sources/ \
                  	$(LOCAL_PATH)/../../../Lua5.2/sources/lua/ \
                    $(LOCAL_PATH)/../../../Box2D/sources/Box2D_v2.2.1/ \
                  	$(LOCAL_PATH)/../../../freetype/sources/ \
                  	$(LOCAL_PATH)/../../../freetype/sources/freetype-2.4.10/ \
                  	$(LOCAL_PATH)/../../../freetype/sources/freetype-2.4.10/include/ \
                  	$(LOCAL_PATH)/.

LOCAL_CFLAGS += -O3

# it is used for ndk-r5  
# if you build with ndk-r4, comment it  
# because the new Windows toolchain doesn't support Cygwin's drive
# mapping (i.e /cygdrive/c/ instead of C:/)  
LOCAL_LDLIBS := -L$(call host-path, $(JNI_PATH)/../libs/$(TARGET_ARCH_ABI)) \
				-L$(call host-path, $(LOCAL_PATH)/../lib) \
				-L$(call host-path, $(COCOS2D_PATH)/cocos2dx/platform/third_party/android/libraries/$(TARGET_ARCH_ABI)) \
                -lGLESv1_CM -lpng -lflmml -lfreetype -llua -lz -lxml2 -llog -lbox2d -lcocos2d -lcocosdenshion

include $(BUILD_SHARED_LIBRARY)
