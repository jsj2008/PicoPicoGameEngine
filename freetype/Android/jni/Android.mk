LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := freetype
SOURCE_PATH := ../../sources/freetype-2.4.10

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(SOURCE_PATH)/src/autofit/autofit.c \
				$(SOURCE_PATH)/src/bdf/bdf.c \
				$(SOURCE_PATH)/src/cff/cff.c \
				$(SOURCE_PATH)/src/base/ftbase.c \
				$(SOURCE_PATH)/src/base/ftglyph.c \
				$(SOURCE_PATH)/src/base/ftbitmap.c \
				$(SOURCE_PATH)/src/bzip2/ftbzip2.c \
				$(SOURCE_PATH)/src/cache/ftcache.c \
				$(SOURCE_PATH)/src/gzip/ftgzip.c \
				$(SOURCE_PATH)/src/base/ftinit.c \
				$(SOURCE_PATH)/src/lzw/ftlzw.c \
				$(SOURCE_PATH)/src/base/ftsystem.c \
				$(SOURCE_PATH)/src/gxvalid/gxvalid.c \
				$(SOURCE_PATH)/src/otvalid/otvalid.c \
				$(SOURCE_PATH)/src/pcf/pcf.c \
				$(SOURCE_PATH)/src/pfr/pfr.c \
				$(SOURCE_PATH)/src/psaux/psaux.c \
				$(SOURCE_PATH)/src/pshinter/pshinter.c \
				$(SOURCE_PATH)/src/psnames/psnames.c \
				$(SOURCE_PATH)/src/raster/raster.c \
				$(SOURCE_PATH)/src/sfnt/sfnt.c \
				$(SOURCE_PATH)/src/smooth/smooth.c \
				$(SOURCE_PATH)/src/truetype/truetype.c \
				$(SOURCE_PATH)/src/sfnt/ttsbit0.c \
				$(SOURCE_PATH)/src/type1/type1.c \
				$(SOURCE_PATH)/src/cid/type1cid.c \
				$(SOURCE_PATH)/src/type42/type42.c \
				$(SOURCE_PATH)/src/winfonts/winfnt.c

LOCAL_C_INCLUDES := \
					$(LOCAL_PATH)/$(SOURCE_PATH)/include/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/include/freetype/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/include/freetype/config/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/include/freetype/internal/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/autofit/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/base/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/bdf/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/bzip2/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/cache/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/cff/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/cid/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/gxvalid/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/gzip/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/lzw/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/otvalid/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/pcf/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/pfr/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/psaux/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/pshinter/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/psnames/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/raster/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/sfnt/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/smooth/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/tools/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/truetype/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/type1/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/type42/ \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/src/winfonts/ \
                    $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3

# it is used for ndk-r5  
# if you build with ndk-r4, comment it  
# because the new Windows toolchain doesn't support Cygwin's drive
# mapping (i.e /cygdrive/c/ instead of C:/)  
                           
include $(BUILD_SHARED_LIBRARY)
