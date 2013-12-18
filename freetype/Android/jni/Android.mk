LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libfreetype
FRT_SOURCE_PATH := ../../sources/freetype-2.4.12
FRT_INCLUDE_PATH := $(LOCAL_PATH)/../../sources/freetype-2.4.12

JNI_PATH := $(LOCAL_PATH)
JNI_CFLAGS := \
				-DFT_CONFIG_MODULES_H="<ftmodule.h>" \
				-DFT2_BUILD_LIBRARY

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
				$(FRT_SOURCE_PATH)/src/autofit/autofit.c \
				$(FRT_SOURCE_PATH)/src/bdf/bdf.c \
				$(FRT_SOURCE_PATH)/src/cff/cff.c \
				$(FRT_SOURCE_PATH)/src/base/ftbase.c \
				$(FRT_SOURCE_PATH)/src/base/ftglyph.c \
				$(FRT_SOURCE_PATH)/src/base/ftbitmap.c \
				$(FRT_SOURCE_PATH)/src/bzip2/ftbzip2.c \
				$(FRT_SOURCE_PATH)/src/cache/ftcache.c \
				$(FRT_SOURCE_PATH)/src/gzip/ftgzip.c \
				$(FRT_SOURCE_PATH)/src/base/ftinit.c \
				$(FRT_SOURCE_PATH)/src/lzw/ftlzw.c \
				$(FRT_SOURCE_PATH)/src/base/ftsystem.c \
				$(FRT_SOURCE_PATH)/src/gxvalid/gxvalid.c \
				$(FRT_SOURCE_PATH)/src/otvalid/otvalid.c \
				$(FRT_SOURCE_PATH)/src/pcf/pcf.c \
				$(FRT_SOURCE_PATH)/src/pfr/pfr.c \
				$(FRT_SOURCE_PATH)/src/psaux/psaux.c \
				$(FRT_SOURCE_PATH)/src/pshinter/pshinter.c \
				$(FRT_SOURCE_PATH)/src/psnames/psnames.c \
				$(FRT_SOURCE_PATH)/src/raster/raster.c \
				$(FRT_SOURCE_PATH)/src/sfnt/sfnt.c \
				$(FRT_SOURCE_PATH)/src/smooth/smooth.c \
				$(FRT_SOURCE_PATH)/src/truetype/truetype.c \
				$(FRT_SOURCE_PATH)/src/sfnt/ttsbit0.c \
				$(FRT_SOURCE_PATH)/src/type1/type1.c \
				$(FRT_SOURCE_PATH)/src/cid/type1cid.c \
				$(FRT_SOURCE_PATH)/src/type42/type42.c \
				$(FRT_SOURCE_PATH)/src/winfonts/winfnt.c

LOCAL_C_INCLUDES := \
					$(FRT_INCLUDE_PATH)/include/ \
					$(FRT_INCLUDE_PATH)/include/freetype/ \
					$(FRT_INCLUDE_PATH)/include/freetype/config/ \
					$(FRT_INCLUDE_PATH)/include/freetype/internal/ \
					$(FRT_INCLUDE_PATH)/src/autofit/ \
					$(FRT_INCLUDE_PATH)/src/base/ \
					$(FRT_INCLUDE_PATH)/src/bdf/ \
					$(FRT_INCLUDE_PATH)/src/bzip2/ \
					$(FRT_INCLUDE_PATH)/src/cache/ \
					$(FRT_INCLUDE_PATH)/src/cff/ \
					$(FRT_INCLUDE_PATH)/src/cid/ \
					$(FRT_INCLUDE_PATH)/src/gxvalid/ \
					$(FRT_INCLUDE_PATH)/src/gzip/ \
					$(FRT_INCLUDE_PATH)/src/lzw/ \
					$(FRT_INCLUDE_PATH)/src/otvalid/ \
					$(FRT_INCLUDE_PATH)/src/pcf/ \
					$(FRT_INCLUDE_PATH)/src/pfr/ \
					$(FRT_INCLUDE_PATH)/src/psaux/ \
					$(FRT_INCLUDE_PATH)/src/pshinter/ \
					$(FRT_INCLUDE_PATH)/src/psnames/ \
					$(FRT_INCLUDE_PATH)/src/raster/ \
					$(FRT_INCLUDE_PATH)/src/sfnt/ \
					$(FRT_INCLUDE_PATH)/src/smooth/ \
					$(FRT_INCLUDE_PATH)/src/tools/ \
					$(FRT_INCLUDE_PATH)/src/truetype/ \
					$(FRT_INCLUDE_PATH)/src/type1/ \
					$(FRT_INCLUDE_PATH)/src/type42/ \
					$(FRT_INCLUDE_PATH)/src/winfonts/ \
					$(LOCAL_PATH)/

LOCAL_CFLAGS += -O3
                           
include $(BUILD_STATIC_LIBRARY)
