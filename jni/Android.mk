LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := sundial

LOCAL_SRC_FILES := sundial.c \
				   sun.c \
				   types.c \
				   moon.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
