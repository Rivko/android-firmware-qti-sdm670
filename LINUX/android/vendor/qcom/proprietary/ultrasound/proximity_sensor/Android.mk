ifneq ($(BUILD_TINY_ANDROID),true)

ROOT_DIR := $(call my-dir)
LOCAL_PATH := $(ROOT_DIR)

common_c_includes := hardware/libhardware/include \
                     $(LOCAL_PATH)/../calculators/stubs
common_src_files := proximity_sensor_hal.cpp \
              proximity_sensor.cpp \
              proximity_sensor_policy.cpp \
              usf_proximity_driver.cpp \
              usf_socket_receiver.cpp \
              keep_alive_driver_proxy.cpp
common_shared_libraries :=  liblog \
                            libcutils
common_cflags := -Wall -Wextra -Werror

# ---------------------------------------------------------------------------------
#                       Make the Shared library (sensors.oem)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE            := sensors.oem
LOCAL_MODULE_TAGS       := optional

# Not break 8974
ifeq ($(call is-board-platform,msm8974),true)
  LOCAL_MODULE_PATH          := $(TARGET_OUT_SHARED_LIBRARIES)/hw
endif

LOCAL_C_INCLUDES        := $(common_c_includes)
LOCAL_SRC_FILES         := $(common_src_files)
LOCAL_SHARED_LIBRARIES  := $(common_shared_libraries)
LOCAL_CFLAGS            := $(common_cflags)

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

# ---------------------------------------------------------------------------------
#                       Make the test library (libusfproxsensorfortest)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE            := libusfproxsensorfortest
LOCAL_MODULE_TAGS       := eng

LOCAL_C_INCLUDES        := $(common_c_includes)
LOCAL_SRC_FILES         := $(common_src_files)
LOCAL_SHARED_LIBRARIES  := $(common_shared_libraries)
LOCAL_CFLAGS            := $(common_cflags)

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

endif #BUILD_TINY_ANDROID
