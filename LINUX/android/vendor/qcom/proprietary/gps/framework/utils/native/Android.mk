ifneq ($(BUILD_TINY_ANDROID),true)

#$(call print-vars, LOCAL_C_INCLUDES)
# -ggdb -g3 -O0: use these flags to enable on-device gdb debug
#QC_LOC_FW_LOCAL_C_FLAGS += -ggdb -g3 -O0

# enforce no warnings
QC_LOC_FW_LOCAL_C_FLAGS += -Wall -Werror

# -v : show the command line used to invoke gcc
#QC_LOC_FW_LOCAL_C_FLAGS += -v

LOCAL_PATH := $(call my-dir)
include $(call all-makefiles-under,$(LOCAL_PATH))

endif # not BUILD_TINY_ANDROID
