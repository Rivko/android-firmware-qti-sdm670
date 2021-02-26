APP_ABI               := armeabi-v7a
APP_STL               := gnustl_shared
NDK_TOOLCHAIN_VERSION := 4.9
APP_CPPFLAGS          := -frtti -fexceptions -fpermissive -Wno-deprecated
APP_CFLAGS            += -std=c++14
APP_PLATFORM 		  := android-19
#APP_PROJECT_PATH      := $(shell pwd -P)

