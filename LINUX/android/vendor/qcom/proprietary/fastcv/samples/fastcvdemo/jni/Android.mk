# An Android.mk file must begin with the definition of the LOCAL_PATH variable.
# It is used to locate source files in the development tree. In this example,
# the macro function 'my-dir', provided by the build system, is used to return
# the path of the current directory (i.e. the directory containing the
# Android.mk file itself).
#
LOCAL_PATH := $(call my-dir)

JNI_DIR := $(LOCAL_PATH)
UTILS_DIR := $(LOCAL_PATH)/utils

# The function "$(call all-subdir-makefiles)" returns a list of Android.mk
# files located in all sub-directories of the current 'my-dir' path.
# This function can be used to provide deep-nested source directory
# hierarchies to the build system.
#
include $(call all-subdir-makefiles)


