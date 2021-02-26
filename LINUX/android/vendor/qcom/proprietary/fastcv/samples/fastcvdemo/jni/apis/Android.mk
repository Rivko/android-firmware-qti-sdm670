# The function "$(call all-subdir-makefiles)" returns a list of Android.mk
# files located in all sub-directories of the current 'my-dir' path.
# This function can be used to provide deep-nested source directory
# hierarchies to the build system.
#
include $(call all-subdir-makefiles)


