# sdllvm-selection.mk
#
# Decide which version of SDLLVM to use depends on the availability
# and update the LOCAL_SDCLANG and LOCAL_SDCALNG_2
#

# Use SDCLANG if its version is >= 4.
# Else use SDCLANG_2 if it exists and its version is >= 4.
# Else use SDCLANG.

ifeq ($(CAMERA_USE_SDCLANG),)
  CAMERA_USE_SDCLANG   := true
  CAMERA_USE_SDCLANG_2 := false

  # Append Android build top if path is not absolute.
  ifneq ($(SDCLANG_PATH),$(filter /%,$(SDCLANG_PATH)))
    CAMERA_SDCLANG_ABS_PATH := $(ANDROID_BUILD_TOP)/$(SDCLANG_PATH)
  else
    CAMERA_SDCLANG_ABS_PATH := $(SDCLANG_PATH)
  endif

  ifneq ($(wildcard $(CAMERA_SDCLANG_ABS_PATH)),)
    CAMERA_SDCLANG_VERSION := $(shell $(CAMERA_SDCLANG_ABS_PATH)/llvm-config --version)

    ifneq ($(shell expr $(CAMERA_SDCLANG_VERSION) \>= 4), 1)
      # Append Android build top if path is not absolute.
      ifneq ($(SDCLANG_PATH_2),$(filter /%,$(SDCLANG_PATH_2)))
      CAMERA_SDCLANG_ABS_PATH_2 := $(ANDROID_BUILD_TOP)/$(SDCLANG_PATH_2)
      else
      CAMERA_SDCLANG_ABS_PATH_2 := $(SDCLANG_PATH_2)
      endif

      ifneq ($(wildcard $(CAMERA_SDCLANG_ABS_PATH_2)),)
        CAMERA_SDCLANG_VERSION_2 := $(shell $(CAMERA_SDCLANG_ABS_PATH_2)/llvm-config --version)
        ifeq ($(shell expr $(CAMERA_SDCLANG_VERSION_2) \>= 4), 1)
          CAMERA_USE_SDCLANG   := false
          CAMERA_USE_SDCLANG_2 := true
        endif
      else
        # CAMERA_SDCLANG_ABS_PATH_2 does not exist.
        CAMERA_USE_SDCLANG_2 := false
      endif
    endif

  else
  # CAMERA_SDCLANG_ABS_PATH does not exist.
  CAMERA_USE_SDCLANG := false
  endif
endif

LOCAL_SDCLANG     := $(CAMERA_USE_SDCLANG)
LOCAL_SDCLANG_2   := $(CAMERA_USE_SDCLANG_2)
