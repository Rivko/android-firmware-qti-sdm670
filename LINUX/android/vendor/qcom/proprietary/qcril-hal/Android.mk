ifneq ($(TARGET_NO_TELEPHONY), true)
LOCAL_DIR := $(call my-dir)

qcril_cflags += -O0 -g
qcril_cppflags += -O0 -g
qcril_ldflags += -Wl,--no-allow-shlib-undefined,--unresolved-symbols=report-all

ifeq ($(ATEL_ENABLE_LLVM_SA),true)
    LLVM_SA_OUTPUT_DIR := $(ANDROID_PRODUCT_OUT)/atel-llvm-sa-results/qcril-hal
    LLVM_SA_FLAG := --compile-and-analyze $(LLVM_SA_OUTPUT_DIR)
    qcril_cflags   += $(LLVM_SA_FLAG)
    qcril_cppflags += $(LLVM_SA_FLAG)
endif

include $(call all-makefiles-under,$(LOCAL_DIR))
endif # TARGET_NO_TELEPHONY
